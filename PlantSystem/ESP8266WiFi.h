// ESP8266WiFi.h

#ifndef _ESP8266WIFI_h
#define _ESP8266WIFI_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class ESP8266WiFiClass
{
 protected:


 public:
	void init();
};

extern ESP8266WiFiClass ESP8266WiFi;

#endif

