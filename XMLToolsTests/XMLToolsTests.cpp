#include "pch.h"
#include "CppUnitTest.h"

#include "../XmlParser.h"
#include "../XmlParser.cpp"  // required, to avoid unresolved linked symbol error
#include "../XmlFormater.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XMLToolsTests {
	TEST_CLASS(XMLToolsTests) {
	public:
		TEST_METHOD(ParserTest1) {
			XmlParser parser("<?xml?><foo x='a' y=\"b\"><bar/>test</foo>");
			std::string tokens = parser.dumpTokens();
			Assert::IsTrue(tokens == "INSTRUCTION/TAG_OPENING/WHITESPACE/ATTR_NAME/EQ/ATTR_VALUE/WHITESPACE/ATTR_NAME/EQ/ATTR_VALUE/TAG_OPENING_END/TAG_OPENING/TAG_SELFCLOSING_END/TEXT/TAG_CLOSING/TAG_CLOSING_END");
		}
	};
}
