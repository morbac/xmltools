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
		std::string readFile(std::string filepath) {
			std::ifstream ifs(filepath);
			std::string res((std::istreambuf_iterator<char>(ifs)),
				(std::istreambuf_iterator<char>()));
			return res;
		}

		void writeFile(std::string data, std::wstring filepath) {
			std::ofstream ofs(filepath);
			ofs << data;
			ofs.close();
		}

		void testParser(std::string xml, std::string ref) {
			XmlFormater formater(xml.c_str(), xml.length());
			std::string tmp = formater.debugTokens();
			Assert::IsTrue(0 == ref.compare(tmp));
		}

		void testPrettyPrint(std::string xml, std::string ref, XmlFormaterParamsType params) {
			XmlFormater formater(xml.c_str(), xml.length(), params);
			std::stringstream* out = formater.prettyPrint();
			std::string tmp = out->str();

			Assert::IsTrue(0 == ref.compare(tmp));

			// check that a second pretty print keeps structure unchanged
			formater.init(tmp.c_str(), tmp.length(), params);
			out = formater.prettyPrint();
			tmp = out->str();

			Assert::IsTrue(0 == ref.compare(tmp));
		}

	public:
		TEST_METHOD(ParserTest1) {
			std::string xml("<?xml?><foo x='a' y z =\n\"b\"><!--test-->\n<bar/> te<![CDATA[...]]>st </foo>");
			std::string ref("INSTRUCTION/TAG_OPENING/WHITESPACE/ATTR_NAME/EQUAL/ATTR_VALUE/WHITESPACE/ATTR_NAME/WHITESPACE/ATTR_NAME/WHITESPACE/EQUAL/LINEBREAK/ATTR_VALUE/TAG_OPENING_END/COMMENT/TEXT/TAG_OPENING/TAG_SELFCLOSING_END/TEXT/CDATA/TEXT/TAG_CLOSING/TAG_CLOSING_END");
			testParser(xml, ref);
		}

		TEST_METHOD(ParserTest2) {
			std::string xml("<p><b></b>text</p>");
			std::string ref("TAG_OPENING/TAG_OPENING_END/TAG_OPENING/TAG_OPENING_END/TAG_CLOSING/TAG_CLOSING_END/TEXT/TAG_CLOSING/TAG_CLOSING_END");
			testParser(xml, ref);	
		}

		TEST_METHOD(ParserTest3) {
			std::string xml("<p> <x> </x>  <y/> <x> </x> z <y/> <x/>  <y/> <x/> z <y/> </p>");
			std::string ref("<p>\n\t<x> </x>\n\t<y/>\n\t<x> </x> z <y/>\n\t<x/>\n\t<y/>\n\t<x/> z <y/>\n</p>");
			
			XmlFormaterParamsType params;
			params.indentChars = "\t";
			params.eolChars = "\n";
			params.maxIndentLevel = 255;
			params.enforceConformity = true;
			params.autoCloseTags = false;
			params.indentAttributes = false;
			params.indentOnly = false;

			testPrettyPrint(xml, ref, params);
		}

		TEST_METHOD(ParserTest4) {
			XmlFormaterParamsType params;
			params.indentChars = "\t";
			params.eolChars = '\n';
			params.maxIndentLevel = 255;
			params.enforceConformity = true;
			params.autoCloseTags = true;
			params.indentAttributes = false;
			params.indentOnly = false;

			std::string xml = readFile("D:\\Progs\\C++\\xmltools\\SimpleXmlLib\\SimpleXmlTests\\TestFiles\\PrettyPrint\\FullTest.in.xml");
			std::string ref = readFile("D:\\Progs\\C++\\xmltools\\SimpleXmlLib\\SimpleXmlTests\\TestFiles\\PrettyPrint\\FullTest.out.xml");

			XmlFormater formater(xml.c_str(), xml.length(), params);
			std::stringstream* out = formater.prettyPrint();
			std::string tmp(out->str());
			tmp.append(params.eolChars);	// the sample has as added final CRLF
			//writeFile(tmp, L"D:\\Progs\\C++\\xmltools\\SimpleXmlLib\\SimpleXmlTests\\TestFiles\\PrettyPrint\\FullTest.test.xml");

			Assert::IsTrue(0 == tmp.compare(ref.c_str()));

			// check that a second pretty print keeps structure unchanged
			formater.init(tmp.c_str(), tmp.length(), params);
			out = formater.prettyPrint();
			tmp = out->str();

			Assert::IsTrue(0 == tmp.compare(ref.c_str()));
		}
	};
}
