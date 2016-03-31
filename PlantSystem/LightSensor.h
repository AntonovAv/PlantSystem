// LightSensor.h

#ifndef _LIGHTSENSOR_h
#define _LIGHTSENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class LightSensorClass
{

public:
	LightSensorClass(int sensPin);
	
	int getValue();
	int getPercentValue();
	String getFormattedValue();

private:
	int sensPin;
	
};


#endif

