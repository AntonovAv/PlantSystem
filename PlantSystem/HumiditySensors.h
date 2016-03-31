// HumiditySensors.h

#ifndef _HUMIDITYSENSORS_h
#define _HUMIDITYSENSORS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
struct HumiditySensorsData
{
	int sens1;
	int sens2;
};

class HumiditySensorsClass
{

public:
	HumiditySensorsClass(int powerPin, int pinSens1, int pinSens2);
	HumiditySensorsData getSensorValues();
	HumiditySensorsData getPercentValues();
	String getFormattedValues();


private:
	int sensPowerPin;

	int sens1Pin;
	int sens2Pin;
};

#endif

