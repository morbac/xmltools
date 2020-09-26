#include "XmlParser.h"

XmlParser::XmlParser(char* data) {
	this->cursor = data;
	this->srcLength = strlen(data);

	this->cursor++;
}

XmlToken XmlParser::nextToken() {
	this->token = XmlToken::EndOfFile;
	return this->token;
}

const char* XmlParser::getTokenName() {
	switch (this->token) {
		case XmlToken::EndOfFile: return "EndOfFile"; break;
		default: "Undefined";
	}
}