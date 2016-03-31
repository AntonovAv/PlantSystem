// 
// 
// 

#include "HumiditySensors.h"

HumiditySensorsClass::HumiditySensorsClass(int powerPin, int pinSens1, int pinSens2)
{
	pinMode(powerPin, OUTPUT);
	digitalWrite(powerPin, LOW);
	this->sensPowerPin = powerPin;
	this->sens1Pin = pinSens1;
	this->sens2Pin = pinSens2;
}

HumiditySensorsData HumiditySensorsClass::getSensorValues()
{
	int max_repeats = 5;
	int sens1Value = 0;
	int sens2Value = 0;
	//enable power
	digitalWrite(sensPowerPin, HIGH);
	delay(10);
	for (int repeat = 0; repeat < max_repeats; repeat++)
	{
		sens1Value += analogRead(sens1Pin);
		sens2Value += analogRead(sens2Pin);
	}
	//disable power
	digitalWrite(sensPowerPin, LOW);
	
	HumiditySensorsData data;
	data.sens1 = sens1Value / max_repeats;
	data.sens2 = sens2Value / max_repeats;
	return data;
}

HumiditySensorsData HumiditySensorsClass::getPercentValues()
{
	HumiditySensorsData data = getSensorValues();
	long sens1 = 1023 - data.sens1;
	sens1 = (sens1 * 100) >> 10; // delete on 1024
	long sens2 = 1023 - data.sens2;
	sens2 = (sens2 * 100) >> 10; // delete on 1024
	data.sens1 = sens1;
	data.sens2 = sens2;
	return data;
}

String HumiditySensorsClass::getFormattedValues()
{
	HumiditySensorsData sensorsData = getPercentValues();
	return String(F("HUM1=")) + sensorsData.sens1 + String(F("&HUM2=")) + sensorsData.sens2;
}
