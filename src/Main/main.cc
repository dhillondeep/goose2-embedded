#include <Arduino.h>
#include <Timer/TimerPool.h>
#include <Watchdog/Watchdog.h>
#include <WSerial.h>
#include <JsonHandler.h>
#include <MemoryFree.h>
#include "Config.h"
#include "Controller.h"
#include "SensorReader.h"

TimerPool pool{config.numTimers};

Watchdog watchdog;
State s;
bool flags[3] = {false, false, false};

Controller *controller;
SensorReader *reader;
CommandListener *listener;

// pod tasks
void taskControl();
void taskCmdListen();
void taskSensorRead();
void taskConnectionCheck();
void taskWatchdogReset();

using namespace wlp;

/*!
 * Restarts the board and either save the state or not
 * @param save true or false based on if to save the state
 */
void resetBoard(bool save);

/*!
 * Initializes system components
 */
void initialize();

/*!
 * Checks the System thoroughly and report back stats
 */
void systemCheck();

void setup(){
	watchdog.disable();

	wlp::serial.begin(config.bandRate);

	// add tasks to execute and time interrupt for them
	pool.addTimer(new TimerPool::Timer(config.controlRate, taskControl));
	pool.addTimer(new TimerPool::Timer(config.cmdListenRate, taskCmdListen));
	pool.addTimer(new TimerPool::Timer(config.sensorReadRate, taskSensorRead));
	pool.addTimer(new TimerPool::Timer(config.connectionCheck, taskConnectionCheck));
	pool.addTimer(new TimerPool::Timer(config.watchdogReset, taskWatchdogReset));

	initialize();
	systemCheck();

	watchdog.setup();
}

void loop(){
	pool.handleTimers();
	watchdog.reset();
}

void resetBoard(bool save){
	s.restart = s.restartNone;

	serial << json.encodeMessage("Restarted arduino!");

	if (save) watchdog.restartBoard(s);
	else watchdog.restartBoard();
}

void initialize(){
	s = watchdog.get();

	reader = new SensorReader{s};
	controller = new Controller{s, *reader, resetBoard};
	listener = new CommandListener{s};

	//watchdog.setup();
}

void systemCheck(){
	serial << json.encodeMessage("Memory: " + String(freeMemory())) << endl;
}

// tasks
void taskControl(){
	controller->control();
	flags[0] = true;
}

void taskCmdListen(){
	listener->listen();
	flags[1] = true;
}

void taskSensorRead(){
	reader->Read();
	//serial << "read" << endl;
	flags[2] = true;
}

void taskConnectionCheck(){
	//Serial.println(analogRead(A0));
	//controller->check();
}

void taskWatchdogReset(){
	return;

	if (flags[0] && flags[1] && flags[2]){
		//watchdog.reset();

		//flags[0] = flags[1] = flags[2] = false;
	} else{
		if (!flags[0])
			serial << json.encodeMessage("controller malfunctioned") << endl;
		if (!flags[1])
			serial << json.encodeMessage("listener malfunctioned") << endl;
		if (!flags[2])
			serial << json.encodeMessage("sensor reader malfunctioned") << endl;
	}
}

/*
ISR(WDT_vect) {
	serial << json.encodeMessage("watchdog restarting arduino") << endl;
	//watchdog.add(s);
}
 */