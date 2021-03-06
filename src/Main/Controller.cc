#include <Watchdog/State.h>
#include <WSerial.h>
#include <JsonHandler.h>
#include "Controller.h"
#include "Config.h"
#include "SensorReader.h"

using namespace wlp;

Controller::Controller(State &s, SensorReader &reader, void(*restart)(bool))
		: state(s), reader(reader), restart{restart}{
	setup();
}

void Controller::setup(){
	ballValve = new BallValve{config.ballValveRelayA, config.ballValveRelayB};
	dpr = new DPR{config.dprRelay};
	magWheels = new MagWheels{state, config.magFlp, config.magFrp, config.magRlp,
	                          config.magRrp};
	driveTrain = new DriveTrain{config.driveRelay, config.drivePin,
	                            config.dropRelay};
	brakes = new ECBrakes{config.brakesRelay};
	magController = new MagController{*magWheels, state};
}

Controller::~Controller(){
	// delete all the subsystems
	delete ballValve;
	delete dpr;
	delete magWheels;
	delete driveTrain;
	delete brakes;
}

void Controller::emergencyProtocol(){
	serial << "E" << endl;
}

void Controller::handleManual(){
	ballValve->control(Subsystem::getState(state.ballValve));
	dpr->control(Subsystem::getState(state.dpr));
	driveTrain->drop(Subsystem::getState(state.drop));
	brakes->control(Subsystem::getState(state.brakes));

	driveTrain->control(state.driveSpeed);
	magController->control(state.magSpeed);
}

void Controller::handleScript(){
	serial << "S" << endl;
}

void Controller::handleAutonomous(){
	serial << "A" << endl;
}

void Controller::stop(){
	state.driveSpeed = 50;
	state.magSpeed = 0;
	state.brakes = true;
	
	driveTrain->control(state.driveSpeed);
	magController->control(state.magSpeed);
	brakes->control(Subsystem::getState(state.brakes));
}

void Controller::check(){
	if (state.stop) return;

	if (state.check){
		state.heartbeatMiss = 0;
		state.check = (uint8_t) false;
	} else if (state.heartbeatMiss >= 2){
		serial << json.encodeMessage("check missed: pod stopped") << endl;
		state.stop = (uint8_t) true;
	} else
		++state.heartbeatMiss;
}

void Controller::control(){
	// restart the pod if instructed to do so
	if (state.restart == state.restartSaved) restart(true);
	else if (state.restart == state.restartUnsaved) restart(false);

	// if there is emergency, handle accordingly and do not do anything else
	if (state.emergency){
		emergencyProtocol();
		return;
	}

	// stop the pod if instructed to do so
	if (state.stop){
		stop();
		return;
	}

	// use the appropriate mode
	if (state.mode == state.manual) handleManual();
	else if (state.mode == state.script) handleScript();
	else if (state.mode == state.autonomous) handleAutonomous();
}
