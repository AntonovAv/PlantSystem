// LedLightingLamp.h

#ifndef _LEDLIGHTINGLAMP_h
#define _LEDLIGHTINGLAMP_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <TimerOne.h>

#define MAX_DUTY 1024
#define MIN_DUTY 0

class LedLightingLampControllerClass
{

public:
	LedLightingLampControllerClass() { currentBrightness = 0; controlPin = 0; isOff == true; };
	void init(char controlPin, byte initBrightness);
	void setBrightness(byte persents);
	void on();
	void off();
	byte getCurrentBrightness();
	String getFormattedBrightnessValue();
	boolean isOn();

private:
	int calculateDutyFromPercents(byte percents);
	void setDutyByBrightness(byte brightness);

	int controlPin;
	byte currentBrightness;
	boolean isOff;

};

#endif

