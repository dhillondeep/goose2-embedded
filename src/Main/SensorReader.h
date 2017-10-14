#ifndef CONTROL_SENSORREADER_H
#define CONTROL_SENSORREADER_H

// Header files created by us that are part of the project
// should use quotes instead of <> (used for pre-existing
// system headers).
#include "sensors/mag/Rpm.h"

class State;

class SensorReader{

public:
	SensorReader(State &rState);
	
	~SensorReader();
	
	void Read(); // public functions should be capitalized

private: // Access keywords should always be declared regardless of default

	// It is  uncommon to have reference member variables, is there a reason this
	// is not a pointer instead?
	State &m_rState; // m_ indicates a member variable, r indicates this is a reference

	// Do you know if any library for Arduino supports smart pointers?
	Rpm *m_pFlmRpm; // p indicates this is a pointer
	Rpm *m_pFrmRpm;
	Rpm *m_pRlmRpm; // Initialzation should happen in constructor.
	Rpm *m_pRrmRpm;

};

#endif //CONTROL_SENSORREADER_H
