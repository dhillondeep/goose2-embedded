#include <Arduino.h>
#include <JsonHandler.h>
#include <Watchdog/State.h>
#include "SensorReader.h"
#include "Config.h"

// Global variables are not recommended unless absolutely recommended.
// Avoiding global variables will reduce name conflicts in the future.
// This can be moved to the Read function and made static, or simply
// made another member variable.
long glLastTime = millis(); // g means global, l means long

SensorReader::SensorReader(State &rState) :
	m_rState(rState), // Long initialization lists are easier to read this way
	m_pFlmRpm(nullptr),
	m_pFrmRpm(nullptr),
	m_pRlmRpm(nullptr),
	m_pRrmRpm(nullptr)
{
	// A value of 260 is over the max of uint8_t (that is 255).
	// Fix warning by using the max defined by UINT8_MAX.
	m_pFlmRpm = new Rpm("rpmflm", UINT8_MAX, config.flmSensor); // No good reason for list initialization
	m_pFrmRpm = new Rpm("rpmfrm", UINT8_MAX, config.frmSensor);
}

void SensorReader::Read(){
	m_rState.currRpmFL = m_pFlmRpm->read();

	if (millis() - glLastTime >= 200){
		//wlp::serial << *m_pFlmRpm;

		// glLastTime = millis(); // Doesn't glLastTime need to be updated here?
	}
}

SensorReader::~SensorReader(){
	delete m_pFlmRpm;
	delete m_pFrmRpm;
	delete m_pRlmRpm;
	delete m_pRrmRpm;
}
