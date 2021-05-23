#pragma once

#include <sstream>
#include <vector>
#include "XmlParser.h"

namespace QuickXml {
	struct XmlFormaterParamsType {
		std::string indentChars = "\t";		// indentation char(s)
		std::string eolChars = "\n";		// end of line char(s)
		size_t maxIndentLevel = 255;		// max indent level (0 == unlimited)
		bool ensureConformity = true;		// make the formater respect conformity
		bool autoCloseTags = false;			// make the formater change tags like <a></a> into <a/>
		bool indentAttributes = false;		// make the formater display attributes on separated lines
		bool indentOnly = false;			// make the formater keep the existing linebreaks and only adjust indentation
		bool applySpacePreserve = false;    // make the formater apply the xml:space="preserve" when defined
	};

	class XmlFormater {
		XmlParser* parser = NULL;

		XmlFormaterParamsType params;

		std::stringstream out;
		size_t indentLevel;                 // the real applied indent level
		size_t levelCounter;                // the level counter

		/*
		* Adds an EOL char to output stream
		*/
		void writeEOL();

		/*
		* Write indentations to output stream. The indentation depends on indentLevel variable.
		*/
		void writeIndentation();

		/*
		* Adds a custom string into output stream. The string can be added several times by
		* specifying the num parameter
		* @param str The string to add
		* @param num The number of times the str should be added
		*/
		void writeElement(std::string str, size_t num = 1);

		/*
		* Change the current indentLevel. The function maintains the level in limits [0 .. params.maxIndentLevel]
		* @param change The direction of change. Value +1 increase the indent level; value -1 decrease the indent level.
		*/
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
		*/
		void init(const char* data, size_t length);

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
		* @param detailed Indicates that output should include data
		* @return A string-reprensentation of all data tokens
		*/
		std::string debugTokens(std::string separator = "/", bool detailed = false);

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

		/*
		* Construct the path of given position
		* @param posiiton The reference position to construct path for
		* @param keepNamespace Flag that indicates if namespace must be added in output path
		* @return The constructed path
		*/
		std::stringstream* currentPath(size_t position, bool keepNamespace = true);

		/*
		* Construct a default formater parameters object
		* @return A default parameters set for formater
		*/
		XmlFormaterParamsType getDefaultParams();
	};
}