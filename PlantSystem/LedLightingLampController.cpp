// 
// 
// 

#include "LedLightingLampController.h"

void LedLightingLampControllerClass::init(char controlPin, byte initBrightness)
{
	this->controlPin = controlPin;
	Timer1.pwm(9, 0);
	currentBrightness = initBrightness;
	off();
}

void LedLightingLampControllerClass::setBrightness(byte persents)
{
	if (false == isOff)
	{
		currentBrightness = persents;
		setDutyByBrightness(persents);
	}
}

void LedLightingLampControllerClass::on()
{
	if (true == isOff)
	{
		isOff = false;
		setDutyByBrightness(currentBrightness);
	}
}

void LedLightingLampControllerClass::off()
{
	if (false == isOff)
	{
		isOff = true;
		setDutyByBrightness(0);
	}
}

byte LedLightingLampControllerClass::getCurrentBrightness()
{
	return isOff ? 0 : currentBrightness;
}

String LedLightingLampControllerClass::getFormattedBrightnessValue()
{
	return String(F("BRIGHT=")) + getCurrentBrightness();
}

boolean LedLightingLampControllerClass::isOn()
{
	return false == isOff;
}

int LedLightingLampControllerClass::calculateDutyFromPercents(byte percents)
{
	unsigned long duty = MIN_DUTY; // long for calculations
	if (percents > 100) {
		duty = MAX_DUTY;
	} 
	else if (percents < 0)
	{
		duty = MIN_DUTY;
	} 
	else 
	{
		duty = percents;
		duty = duty * MAX_DUTY;
		duty = duty / 100;
	}
	return duty;
}

void LedLightingLampControllerClass::setDutyByBrightness(byte brightness)
{
	Timer1.setPwmDuty(controlPin, calculateDutyFromPercents(brightness));
}
