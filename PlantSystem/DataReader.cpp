// 
// Class allows to handle response from server that resieved by wifi module 
// 

#include "DataReader.h"


char DataReader_::handleNextCharWithIPD(char &c) {
	switch (state) {
		// find marker to read lenght of part of message
	case FIND_LEN:
		if (IPD_COM_TEMPLATE.charAt(char_count) == c) {
			char_count += 1;
			if (char_count == IPD_COM_TEMPLATE.length()) {
				char_count = 0;
				state = READ_LEN;
			}
		}
		else {
			char_count = 0;
			if (IPD_COM_TEMPLATE.charAt(char_count) == c) {
				char_count += 1;
			}
		}
		break;
		// read lenght of part of message and parse to int
	case READ_LEN:
		if (c == ':' ) {
			data_len = data_len_not_parse.toInt();
			state = READ_DATA;
		}
		else {
			data_len_not_parse += c;
		}
		break;
		// read raw data from module (max 1460 chars)
	case READ_DATA:
		data_len -= 1; // read next char
		if (-1 < data_len) {
			return c;
		}
		else {
			state = FIND_LEN;
		}
		break;
	}
	return SKIP_CHAR;
}

char DataReader_::handleNextCharWithChunked(char &c) {
	switch (chunkedState) {

	case READ_CHUNKED_LEN:
		if (c == '\n') {
			chunked_data_len_not_parse.trim(); // delete \r\n characters 
			chunked_data_len = hexToDec(chunked_data_len_not_parse); // convert lenght to int
			chunked_data_len_not_parse = "";
			chunkedState = READ_CHUNKED_DATA;
			if (chunked_data_len == 0) {
				return END_OF_DATA_CHAR;
			}
		}
		else {
			chunked_data_len_not_parse += c;
		}
		break;
	case READ_CHUNKED_DATA:
		chunked_data_len -= 1;
		if (-1 < chunked_data_len) { // first tine data_len == 0 -> go to read len 
			return c; // raw data without chuncked
		}
		else {
			char_count += 1; // escape \r\n last chars in chuncked data
			if (char_count > 1) {
				char_count = 0;
				chunkedState = READ_CHUNKED_LEN;
			}
		}
		break;
	}
	return SKIP_CHAR;
}

boolean DataReader_::handleNextChar(char &c) {
	c = handleNextCharWithIPD(c);
	if (SKIP_CHAR == c) {
		return false;
	}

	if (false == isReadHeader) { // if not read header
		(*tempHeader) += c;
		if (END_OF_HEADER_TEMPLATE.charAt(char_count) == c) {
			char_count += 1;
			if (char_count == END_OF_HEADER_TEMPLATE.length()) {
				char_count = 0;
						
				if ((*tempHeader).indexOf(CHUNKED_TEMPLATE) != -1) {
					isChunked = true;
					chunkedState = READ_CHUNKED_LEN;
				} else {
					// if not chuncked - read content length
					int ind = (*tempHeader).indexOf(CONTENT_LEN_TEMPLATE);
					ind += CONTENT_LEN_TEMPLATE.length();
					String cont_len_not_parse = "";
					while (tempHeader->charAt(ind) != '\r') {
						cont_len_not_parse += (*tempHeader).charAt(ind);
						ind++;
					}
					content_length = cont_len_not_parse.toInt();
				}

				if (!saveHeader && tempHeader != 0) { // fix?!
					delete tempHeader; // release memory
					tempHeader = 0;
				}
				isReadHeader = true;
				// need to implement return header
			}
		}
		else {
			char_count = 0;
			if (END_OF_HEADER_TEMPLATE.charAt(char_count) == c) {
				char_count += 1;
			}
		}
	}
	else {
		if (true == isChunked) { // if data encoding is chucked 
			c = handleNextCharWithChunked(c);
			if (c == END_OF_DATA_CHAR)
			{
				c = SKIP_CHAR;
				return true;
			}
			else {
				return false;
			}
		}
		else {
			content_length--;
			if (content_length <= 0) {
				return true;
			}
			return false; // data if encoding is not chunked
		}
	}
	c = SKIP_CHAR;
	return false;
}

unsigned int DataReader_::hexToDec(String &hexString) {

	unsigned int decValue = 0;
	int nextInt;

	for (int i = 0; i < hexString.length(); i++) {

		nextInt = int(hexString.charAt(i));
		if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
		if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
		if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
		nextInt = constrain(nextInt, 0, 15);

		decValue = (decValue * 16) + nextInt;
	}

	return decValue;
}

void DataReader_::initRead(boolean isNeedHeader) {
	saveHeader = isNeedHeader;
	char_count = 0;
	isReadHeader = false;
	isChunked = false;
	state = FIND_LEN;
	data_len = 0;
	chunked_data_len = 0;
	content_length = 0;
	tempHeader = new String;
}

DataReader_::DataReader_(boolean saveHeader) {
	initRead(saveHeader);
}

DataReader_::~DataReader_() {
	if (tempHeader != 0) {
		delete tempHeader;
	}
}

String & DataReader_::getLastHeader() {
	return lastResponseHeader;
}
