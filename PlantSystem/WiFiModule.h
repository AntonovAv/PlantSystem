// WiFiModule.h

#ifndef _WIFIMODULE_h
#define _WIFIMODULE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <DataReader.h>

#define ESP_RECEIVE_TIMEOUT   1000L
#define ESP_RESET_TIMEOUT     5000L
#define ESP_CONNECT_TIMEOUT  8000L
#define ESP_RESPONCE_TIMEOUT 20000L

class WiFiModuleClass
{

 public:
	void initWifi(Stream *moduleStream, Stream *debugStream = NULL);
	
	boolean connectToAP(const String& ssid, const String& password);
	boolean disconnectAP();

	boolean softReset();

	boolean connectTCP(const String& host, const String& port);
	boolean closeTCP();

	boolean prepareRequest(String &request, const String& host);
	int readRequestResponce(const String& request, char* buffer, int size);
	
	void debugLoop();

private:
	boolean findModuleResp(const String& strForFind, int timeOut = ESP_RECEIVE_TIMEOUT);

	int freeRam();

	Stream *moduleStream;
	Stream *debug;
};

#endif

