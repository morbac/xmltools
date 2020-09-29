#include "pch.h"
#include "CppUnitTest.h"

#include "../XmlParser.h"
#include "../XmlParser.cpp"  // required, to avoid unresolved linked symbol error
#include "../XmlFormater.h"
#include "../XmlFormater.cpp"  // required, to avoid unresolved linked symbol error

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XMLToolsTests {
	TEST_CLASS(XMLToolsTests) {
	public:
		TEST_METHOD(ParserTest1) {
			std::string xml("<?xml?><foo x='a' y z =\r\n\"b\"><!--test-->\r\n<bar/> te<![CDATA[...]]>st </foo>");
			XmlFormater formater(xml.c_str(), xml.length());
			std::string tokens = formater.debugTokens();
			Assert::IsTrue(tokens == "INSTRUCTION/TAG_OPENING/WHITESPACE/ATTR_NAME/EQUAL/ATTR_VALUE/WHITESPACE/ATTR_NAME/WHITESPACE/ATTR_NAME/WHITESPACE/EQUAL/LINEBREAK/ATTR_VALUE/TAG_OPENING_END/COMMENT/TEXT/TAG_OPENING/TAG_SELFCLOSING_END/TEXT/CDATA/TEXT/TAG_CLOSING/TAG_CLOSING_END");
		}

		TEST_METHOD(ParserTest2) {
			std::string xml("<p><b></b>text</p>");
			XmlFormater formater(xml.c_str(), xml.length());
			std::string tokens = formater.debugTokens();
			Assert::IsTrue(tokens == "TAG_OPENING/TAG_OPENING_END/TAG_OPENING/TAG_OPENING_END/TAG_CLOSING/TAG_CLOSING_END/TEXT/TAG_CLOSING/TAG_CLOSING_END");
		}
	};
}
