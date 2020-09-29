#include "pch.h"
#include "CppUnitTest.h"

#include <windows.h>
#include <fstream>
#include <string>
#include <streambuf>




#include "../XmlParser.h"
#include "../XmlParser.cpp"  // required, to avoid unresolved linked symbol error
#include "../XmlFormater.h"
#include "../XmlFormater.cpp"  // required, to avoid unresolved linked symbol error

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XMLToolsTests {
	TEST_CLASS(XMLToolsTests) {
		std::string readFile(std::wstring filepath) {
			std::ifstream ifs(filepath);
			std::string content((std::istreambuf_iterator<char>(ifs)),
				                (std::istreambuf_iterator<char>()));
			return content;
		}
		void writeFile(std::string data, std::wstring filepath) {
			std::ofstream ofs(filepath);
			ofs << data;
			ofs.close();
		}
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

		TEST_METHOD(ParserTest3) {
			std::string xml("<p><x> </x>  <y/><x> </x> z <y/><x/>  <y/><x/> z <y/></p>");
			std::string res("<p>\r\n\t<x> </x>\r\n\t<y/>\r\n\t<x> </x> z <y/>\r\n\t<x/>\r\n\t<y/>\r\n\t<x/> z <y/>\r\n</p>");
			
			PrettyPrintParamsType params;
			params.indentChars = "\t";
			params.eolChars = "\r\n";
			params.maxIndentLevel = 255;
			params.trimWhitespaceAroundText = false;
			params.autoCloseTags = false;
			params.indentAttributes = false;
			params.indentOnly = false;

			XmlFormater formater(xml.c_str(), xml.length(), params);
			std::stringstream* out = formater.prettyPrint();
			std::string tmp = out->str();
			Assert::IsTrue(0 == res.compare(tmp));
		}

		TEST_METHOD(ParserTest4) {
			PrettyPrintParamsType params;
			params.indentChars = "\t";
			params.eolChars = "\r\n";
			params.maxIndentLevel = 255;
			params.trimWhitespaceAroundText = false;
			params.autoCloseTags = true;
			params.indentAttributes = false;
			params.indentOnly = false;

			std::string xml = readFile(L"C:\\Users\\nc\\Desktop\\xmltools\\UnitTests\\TestFiles\\PrettyPrint\\FullTest.in.xml");
			std::string ref = readFile(L"C:\\Users\\nc\\Desktop\\xmltools\\UnitTests\\TestFiles\\PrettyPrint\\FullTest.out.xml");

			XmlFormater formater(xml.c_str(), xml.length(), params);
			std::stringstream* out = formater.prettyPrint();
			std::string tmp(out->str());
			tmp.append(params.eolChars);	// the sample has as added final CRLF
			writeFile(tmp, L"C:\\Users\\nc\\Desktop\\xmltools\\UnitTests\\TestFiles\\PrettyPrint\\FullTest.test.xml");

			Assert::IsTrue(0 == tmp.compare(ref.c_str()));
		}
	};
}
