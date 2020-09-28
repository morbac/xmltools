#pragma once

#include <sstream>
#include <vector>
#include "XmlParser.h"

struct PrettyPrintParamsType {
	std::string indentChars;
	std::string eolChars;
	size_t maxIndentLevel;
	bool trimWhitespaceAroundText;
	bool autoCloseTags;
	bool indentAttributes;
	bool indentOnly;
};

enum class TrimModeType {
	NONE,
	LEFT,
	RIGHT,
	BOTH
};

class XmlFormater {
	XmlParser* parser = NULL;

	PrettyPrintParamsType prettyPrintParams;

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
	XmlFormater(const char* data, size_t length, PrettyPrintParamsType params);

	/*
	/ Destructor
	*/
	~XmlFormater();

	void reset();

	/*
	* Performs pretty print formating
	* @return A reference string stream containing the formated string
	*/
	std::stringstream* prettyPrint();
};

