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
	* @param parser A reference to a parser object
	*/
	XmlFormater(XmlParser* parser);

	void destroy();

	void reset();

	std::string prettyPrint();
};

