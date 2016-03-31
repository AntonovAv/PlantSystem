// DataReader.h

#ifndef _DATAREADER_h
#define _DATAREADER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define END_OF_DATA_CHAR -2
#define SKIP_CHAR -1

class DataReader_ {
public:

	DataReader_() {};
	DataReader_(boolean saveHeader);
	~DataReader_();

	void initRead(boolean isNeedHeader);
	// handle next char (return false if not last char /not modify char if data or modify to skip char)
	boolean handleNextChar(char &c);
	void clearMemory();
	String & getLastHeader();

private:

	unsigned int hexToDec(String &hexString);
	char handleNextCharWithIPD(char &c);
	char handleNextCharWithChunked(char &c);

	String lastResponseHeader;
	String* tempHeader;
	boolean saveHeader;

	byte state;
	byte chunkedState;

	static const byte FIND_LEN = 0;
	static const byte READ_LEN = 1;
	static const byte READ_DATA = 2;
	static const byte READ_HEADER = 3;

	static const byte READ_CHUNKED_LEN = 0;
	static const byte READ_CHUNKED_DATA = 1;

	boolean isReadHeader = false; // if header already read
	boolean isChunked = false;  // if data encoding is chuncked
	
	String CHUNKED_TEMPLATE = "chunked";
	String IPD_COM_TEMPLATE = "+IPD,";
	String END_OF_HEADER_TEMPLATE = "\r\n\r\n";
	String CONTENT_LEN_TEMPLATE = "Content-Length: ";
	
	int char_count; // temp counter when need to find string

	String data_len_not_parse; // len of data after IPD
	int data_len;
	String chunked_data_len_not_parse; // len of data if chuncked encoding
	int chunked_data_len;
	
	int content_length; // need for count of content length if encoding not chuncked
};

#endif