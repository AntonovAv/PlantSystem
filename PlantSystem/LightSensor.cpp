// 
// 
// 

#include "LightSensor.h"

LightSensorClass::LightSensorClass(int inputPin)
{
	this->sensPin = inputPin;
}

int LightSensorClass::getValue()
{
	int max_repeats = 5;
	int result = 0;
	for (int repeat = 0; repeat < max_repeats; repeat++)
	{
		result += analogRead(sensPin);
	}
	return result/max_repeats;
}

int LightSensorClass::getPercentValue()
{
	long value = getValue();
	return ((1023 - value) * 100) >> 10 ;//delete on 1024
}

String LightSensorClass::getFormattedValue()
{
	return String(F("LIGHT=")) + getPercentValue();
}
