#pragma once

#include <sstream>
#include <vector>
#include "XmlParser.h"

namespace QuickXml {
	struct XmlFormaterParamsType {
		std::string indentChars;	   // indentation char(s)
		std::string eolChars;		   // end of line char(s)
		size_t maxIndentLevel;		   // max indent level (0 == unlimited)
		bool enforceConformity;		   // make the formater respect conformity
		bool autoCloseTags;			   // make the formater change tags like <a></a> into <a/>
		bool indentAttributes;		   // make the formater display attributes on separated lines
		bool indentOnly;			   // make the formater keep the existing linebreaks and only adjust indentation
	};

	class XmlFormater {
		XmlParser* parser = NULL;

		XmlFormaterParamsType params;

		std::stringstream out;
		size_t indentLevel;

		void writeEOL();
		void writeIndentation();
		void updateIndentLevel(int change);
	public:
		/*
		* Constructor
		* @param data The source data
		* @param length The source data length
		*/
		XmlFormater(const char* data, size_t length);

		/*
		* Constructor
		* @param data The source data
		* @param length The source data length
		* @param params The formater params
		*/
		XmlFormater(const char* data, size_t length, XmlFormaterParamsType params);

		/*
		/ Destructor
		*/
		~XmlFormater();

		/*
		* Initialize the formater with input data
		* @param data The source data
		* @param length The source data length
		* @param params The formater params
		*/
		void init(const char* data, size_t length, XmlFormaterParamsType params);

		/*
		* Make internal parameters ready for formating
		*/
		void reset();

		/*
		* Generates a string containing a list of recognized tokens
		* This method has no other goal that help for debug
		* @param separator The tokens names separator (default is '/')
		* @return A string-reprensentation of all data tokens
		*/
		std::string debugTokens(std::string separator = "/");

		/*
		* Performs linearize formating
		* @return A reference string stream containing the formated string
		*/
		std::stringstream* linearize();

		/*
		* Performs pretty print formating
		* @return A reference string stream containing the formated string
		*/
		std::stringstream* prettyPrint();
	};
}