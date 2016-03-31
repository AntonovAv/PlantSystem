// 
// 
// 

#include "WiFiModule.h"


void WiFiModuleClass::initWifi(Stream *moduleStream, Stream *debugStream)
{
	this->moduleStream = moduleStream;
	this->debug = debugStream;	

	softReset();
}

boolean WiFiModuleClass::connectToAP( const String & ssid, const String & password)
{
	moduleStream->setTimeout(ESP_RECEIVE_TIMEOUT);
	moduleStream->println(F("AT+CWMODE=1")); // WiFi mode = Sta

	String resp = moduleStream->readString();
	if (-1 == resp.indexOf(F("OK")) || -1 != resp.indexOf(F("no change")))
	{
		return false;
	}
		
	moduleStream->print(F("AT+CWJAP=\"")); // Join access point
	moduleStream->print(ssid);
	moduleStream->print(F("\",\""));
	moduleStream->print(password);
	moduleStream->println('\"');

	boolean found = findModuleResp(F("OK"), ESP_CONNECT_TIMEOUT);
	if (found) {
		moduleStream->println(F("AT+CIPMUX=0"));
		found = findModuleResp(F("OK"));   // Await 'OK'
	}
	return found;
}

boolean WiFiModuleClass::disconnectAP()
{
	moduleStream->println(F("AT+CWQAP")); // Quit access point
	return findModuleResp(F("OK"));		  // Purge 'OK'
}

boolean WiFiModuleClass::softReset()
{
	boolean  found = false;
	moduleStream->println(F("AT+RST"));  // Issue soft-reset command
	if (findModuleResp(F("ready"), ESP_RESET_TIMEOUT)) {           // Wait for boot message
		moduleStream->println(F("ATE0"));       // Turn off echo
		found = findModuleResp(F("OK"));        // OK?
	}
	return found;
}

boolean WiFiModuleClass::connectTCP(const String & host, const String & port)
{
	moduleStream->print(F("AT+CIPSTART=\"TCP\",\""));
	moduleStream->print(host);
	moduleStream->print(F("\","));
	moduleStream->println(port);

	if ( findModuleResp(F("OK"))) {
		return true;
	}
	return false;
}

boolean WiFiModuleClass::closeTCP()
{
	moduleStream->println(F("AT+CIPCLOSE"));
	return findModuleResp(F("CLOSED"), 10000);
}

boolean WiFiModuleClass::prepareRequest(String &request, const String &host )
{
	moduleStream->print(F("AT+CIPSEND="));
	moduleStream->println(25 + request.length() + host.length());
	if (findModuleResp(F("> "))) { // Wait for prompt
		//             4                      17                            4
		request = "GET " + request + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
		return true;
	}
	return false;
}

int WiFiModuleClass::readRequestResponce(const String& request, char * buffer, int size)
{
	moduleStream->print(request);

	DataReader_ dataReader = DataReader_(false);
		
	int time = ESP_RESPONCE_TIMEOUT;
	boolean breaker = false;

	int currentElement = 0;

	while (time > 0) {
		while (moduleStream->available() > 0) {
			
			char c = moduleStream->read();
			boolean isEndChar = dataReader.handleNextChar(c);
			
			if (SKIP_CHAR != c) {
				
				if (currentElement < size)
				{
					buffer[currentElement++] = c;
				}
			}
			if (isEndChar == true) {
				breaker = true;
				break;
			}
		}
		if (true == breaker) {
			break;
		}
		time -= 1;
		delay(1);
	}

	if (false == breaker) {
		if (debug)
		{
			debug->println(F("Connection timeout"));
			return 0;
		}
	}
	else {
		if (findModuleResp(F("OK")))
		{
			return 0;
		}
		else
		{
			return currentElement - 1;
		}
	}
}

void WiFiModuleClass::debugLoop()
{
	if (!debug) for (;;); // If no debug connection, nothing to do.

	debug->println(F("\n========================"));
	for (;;) {
		if (debug->available())  moduleStream->write(debug->read());
		if (moduleStream->available()) debug->write(moduleStream->read());
	}
}

boolean WiFiModuleClass::findModuleResp(const String& strForFind, int timeOut)
{
	int strLen = strForFind.length();
	char* temp = new char[strLen + 1];
	strForFind.toCharArray(temp, strLen + 1);	
	moduleStream->setTimeout(timeOut);
	boolean isFounded = moduleStream->find(temp, strLen);
	delete[] temp;
	return isFounded;/* moduleStream->readString().indexOf(strForFind) != -1;*/
}

int WiFiModuleClass::freeRam()
{
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
