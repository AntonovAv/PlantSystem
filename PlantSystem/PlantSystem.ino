#include <EEPROM.h>
#include <LedLightingLampController.h>
#include <TimerOne.h>
#include <TimerThree.h>
#include "HumiditySensors.h"
#include "LightSensor.h"
#include <Adafruit_ESP8266.h>
#include "OneWireTempSensor.h"
#include <Wire.h>
#include <RTClib.h>
#include <OneWire.h>
#include <WiFiModule.h>


#define xstr(s) str(s)
#define str(s) #s

#define LIGHT_SENS_ANALOG_PIN 0
#define HUMIDITY_POWER_PIN 5
#define HUMIDITY_SENS1_ANALOG_PIN 1
#define HUMIDITY_SENS2_ANALOG_PIN 2
#define LED_LAMP_CONTROLLER_PIN 9
#define BUTTON_PIN 6

#define TICKS_IN_SECOND 100 // n tick for timer 3

#define HOUR_WHEN_STOP_LIGHTING 3 // 3 hours of night
#define HOUR_WHEN_START_LIGHTING 8 // 8 hours of morning
#define LIGHT_THRESHOLD_FOR_START 40 // persents
#define LIGHT_THRESHOLD_FOR_STOP 60 // persents

#define ADDRESS_FOR_LAMP_BRIGHTNESS 0

unsigned long  LIGHT_CHECKING_INTERVAL = TICKS_IN_SECOND * 60;
unsigned long  DATA_SENDING_INTERVAL = TICKS_IN_SECOND * 600;

RTC_DS1307 RTC;
WiFiModuleClass wifi;
LightSensorClass lightSensor(LIGHT_SENS_ANALOG_PIN);
HumiditySensorsClass humiditySensors(HUMIDITY_POWER_PIN, HUMIDITY_SENS1_ANALOG_PIN, HUMIDITY_SENS2_ANALOG_PIN);
LedLightingLampControllerClass lampController;

TimeSpan tzOffset = TimeSpan(0, 3, 0, 0);

TimeSpan syncTimeInterval = TimeSpan(7,0,0,0); // 7 days
DateTime lastSyncDate = DateTime(0);


void setup(void)
{
	pinMode(BUTTON_PIN, INPUT);

	Serial.begin(115200);
	Serial1.begin(115200);
	Wire.begin();
	RTC.begin();
	if (!RTC.isrunning())
	{
		Serial.println(F("RTC error"));
	}

	Timer1.initialize(100); // 100us 10khz pwm
	Timer1.start();
	lampController.init(LED_LAMP_CONTROLLER_PIN, EEPROM.read(ADDRESS_FOR_LAMP_BRIGHTNESS));

	wifi.initWifi(&Serial1, &Serial);
	if (!wifi.connectToAP(F("internet"), F("654qwerty123")))
	{
		Serial.println(F("ololololo"));
	}
//	lastSyncDate = syncTimeWithServer();
	
	Timer3.initialize(10000); // 10 ms ticks
	Timer3.attachInterrupt(ticksFunction);
	Timer3.start();
	//wifi.debugLoop();
}

boolean isNeedPerformDataSending = true;

boolean isNeedPerformLightChecking = true;
boolean isAllowingLightingPrevState = true;
void loop(void)
{
	if (true == isNeedPerformDataSending) {
		isNeedPerformDataSending = false;

		DateTime now = RTC.now();
		Serial.print(F("cur time: ")); printDateTime(now);
		// check if neeed sync time
		if ((lastSyncDate + syncTimeInterval).unixtime() < now.unixtime())
		{
			Serial.println("Sync data");
			lastSyncDate = syncTimeWithServer();
		}

		sendingDataToServer(now);
	}

	if (true == isNeedPerformLightChecking) {
		isNeedPerformLightChecking = false;

		boolean isAllowLighting = isAllowToLighting();
		if (isAllowingLightingPrevState == isAllowLighting)
		{
			isAllowLighting ? lampController.on() : lampController.off();
		}
		isAllowingLightingPrevState = isAllowLighting;
	}

	delay(1);
}

//--------------------------------------------------------//

// function - scheduler and buttons checker
long dataSendingCount = 0;
long checkLightingCount = 0;

boolean buttonPrevState = HIGH;
boolean buttonPressed = false;
void ticksFunction(void)
{
	if (++dataSendingCount >= (DATA_SENDING_INTERVAL))
	{ 
		dataSendingCount = 0;
		isNeedPerformDataSending = true;
	}
	if (++checkLightingCount >= (LIGHT_CHECKING_INTERVAL))
	{
		checkLightingCount = 0;
		isNeedPerformLightChecking = true;
	}

	if (buttonPrevState == LOW) {
		buttonPressed = digitalRead(BUTTON_PIN) == HIGH;
	}
	else {
		buttonPressed = false;
	}
	buttonPrevState = digitalRead(BUTTON_PIN);

	// update brightness
	if (buttonPressed == true) {
		byte newBrightness = lampController.getCurrentBrightness() + 5; // step up by 5
		if (newBrightness > 100) {
			newBrightness = 0;
		}
		lampController.setBrightness(newBrightness);
		EEPROM.update(ADDRESS_FOR_LAMP_BRIGHTNESS, newBrightness);
	}
}

// send data to server
void sendingDataToServer(DateTime now) {
	
	String request = String(F("/saveValues?TS=")) + (now - tzOffset).unixtime() + 
		"&" + lightSensor.getFormattedValue() +
		"&" + humiditySensors.getFormattedValues() + 
		"&" + lampController.getFormattedBrightnessValue(); // create request

	boolean isRequestPrepared = wifi.prepareRequest(request, F("192.168.0.2"));
	if (false == isRequestPrepared) {
		Serial.println(F("req not prepareed. try connect to server"));
		// try to connect to server
		if (wifi.connectTCP(F("192.168.0.2"), F("8085"))) {
			isRequestPrepared = wifi.prepareRequest(request, F("192.168.0.2"));
		}
		else
		{
			Serial.println(F("Can't connect to server"));
		}
	}

	boolean success = false;
	// check if  request prepared
	if (true == isRequestPrepared)
	{
		char buffer[10] = { 0 };
		wifi.readRequestResponce(request, buffer, 10);
		success = String(buffer).equalsIgnoreCase(F("SAVED"));
	}
	if (false == success)
	{
		Serial.println(F("bad bad bad request"));
	}
}

// sync time with server
DateTime syncTimeWithServer()
{
	DateTime syncDate = DateTime(0);
	
	char buffer[20];
	String request = String(F("/time"));
	String time = String();
	
	boolean isReqPrepared = wifi.prepareRequest(request, F("192.168.0.2"));
	if (false == isReqPrepared)
	{
		if (wifi.connectTCP(F("192.168.0.2"), F("8085"))) {
			isReqPrepared = wifi.prepareRequest(request, F("192.168.0.2"));
		}
	} 

	if (true == isReqPrepared)
	{
		int readedChars = wifi.readRequestResponce(request, buffer, 20);
		time.concat(buffer);
		time.trim();

		syncDate = DateTime(time.toInt()) + tzOffset;
		RTC.adjust(syncDate);
	}
	else
	{
		Serial.println(F("time are not synced"));
	}
	return syncDate;
}

// check if allowing to on lamp
boolean isAllowToLighting()
{
	byte curHour = RTC.now().hour();
	boolean isAllowLighting = false;
	if (HOUR_WHEN_STOP_LIGHTING <= curHour && curHour < HOUR_WHEN_START_LIGHTING)
	{
		isAllowLighting = false;
	}
	else
	{
		int threshold = lampController.isOn()  ? LIGHT_THRESHOLD_FOR_STOP : LIGHT_THRESHOLD_FOR_START;

		int lightValue = lightSensor.getPercentValue();
		isAllowLighting = lightValue < threshold;
	}
	return isAllowLighting;
}

void printDateTime(DateTime& date)
{
	Serial.print(date.hour()); Serial.print(F(":")); Serial.print(date.minute()); Serial.print(F(":")); Serial.println(date.second());
}
