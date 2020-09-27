#pragma once

#include <sstream>
#include "XmlParser.h"

struct prettyPrintParamsType {
	std::string indentChars;
	std::string eolChars;
	size_t maxIndentLevel;
	bool allowWhitespaceTrim;
	bool autoCloseTags;
};

class XmlFormater {
	XmlParser* parser;

	prettyPrintParamsType prettyPrintParams;

	std::stringstream out;
	size_t indentLevel;

	void writeEOL();
	void writeIndentation();
	void updateIndentLevel(int change);
public:
	/*
	* Constructor
	* @param data The source data
	*/
	XmlFormater(char* data);

	/*
	/ Destructor
	*/
	~XmlFormater();

	void reset();

	std::string prettyPrint();
};

