// OneWireTempSensor.h

#ifndef _ONEWIRETEMPSENSOR_h
#define _ONEWIRETEMPSENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class OneWireTempSensorClass
{
 protected:


 public:
	void init();
};

extern OneWireTempSensorClass OneWireTempSensor;

#endif

