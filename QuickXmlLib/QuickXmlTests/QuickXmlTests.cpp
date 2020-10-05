#include "CppUnitTest.h"

#include <windows.h>
#include <fstream>
#include <string>
#include <streambuf>

#include "XmlParser.h"
#include "XmlParser.cpp"  // required, to avoid unresolved linked symbol error
#include "XmlFormater.h"
#include "XmlFormater.cpp"  // required, to avoid unresolved linked symbol error

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace QuickXml;

namespace QuickXmlTests {
	std::string xmltestsfileshome("D:\\Progs\\C++\\xmltools\\SimpleXmlLib\\SimpleXmlTests\\TestFiles\\");

	TEST_CLASS(QuickXmlTests) {
		std::string readFile(std::string filepath) {
			std::ifstream ifs(xmltestsfileshome + filepath);
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

		void testIndentAttr(std::string xml, std::string ref, XmlFormaterParamsType params) {
			params.indentAttributes = true;

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

		void testIndentOnly(std::string xml, std::string ref, XmlFormaterParamsType params) {
			params.indentOnly = true;

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

		void testLinearize(std::string xml, std::string ref, XmlFormaterParamsType params) {
			XmlFormater formater(xml.c_str(), xml.length(), params);
			std::stringstream* out = formater.linearize();
			std::string tmp = out->str();

			Assert::IsTrue(0 == ref.compare(tmp));

			// check that a second linearize keeps structure unchanged
			formater.init(tmp.c_str(), tmp.length(), params);
			out = formater.linearize();
			tmp = out->str();

			Assert::IsTrue(0 == ref.compare(tmp));

			// now we verify that pretty printing the orignal and the linearized
			// version produce the same when conformity enforcement is activated
			if (params.ensureConformity) {
				formater.init(tmp.c_str(), tmp.length(), params);
				out = formater.prettyPrint();
				std::string tmp2 = tmp = out->str();

				formater.init(xml.c_str(), xml.length(), params);
				out = formater.prettyPrint();
				tmp = out->str();

				Assert::IsTrue(0 == tmp2.compare(tmp));
			}
		}

	public:

		// Parser tests

		TEST_METHOD(ParserTest01) {
			std::string xml("<?xml?><foo x='a' y z =\n\"b\"><!--test-->\n<bar/> te<![CDATA[...]]>st </foo>");
			std::string ref("INSTRUCTION/TAG_OPENING/WHITESPACE/ATTR_NAME/EQUAL/ATTR_VALUE/WHITESPACE/ATTR_NAME/WHITESPACE/ATTR_NAME/WHITESPACE/EQUAL/LINEBREAK/ATTR_VALUE/TAG_OPENING_END/COMMENT/TEXT/TAG_OPENING/TAG_SELFCLOSING_END/TEXT/CDATA/TEXT/TAG_CLOSING/TAG_CLOSING_END");
			testParser(xml, ref);
		}

		TEST_METHOD(ParserTest02) {
			std::string xml("<p><b></b>text</p>");
			std::string ref("TAG_OPENING/TAG_OPENING_END/TAG_OPENING/TAG_OPENING_END/TAG_CLOSING/TAG_CLOSING_END/TEXT/TAG_CLOSING/TAG_CLOSING_END");
			testParser(xml, ref);
		}

		//--------------------------------------------------------------------------------------------

		// Pretty print

		TEST_METHOD(PrettyPrintTest01) {
			std::string xml("<p> <x> </x>  <y/> <x> </x> z <y/> <x/>  <y/> <x/> z <y/> </p>");
			std::string ref("<p>\n\t<x> </x>\n\t<y/>\n\t<x> </x> z <y/>\n\t<x/>\n\t<y/>\n\t<x/> z <y/>\n</p>");

			XmlFormaterParamsType params;
			params.indentChars = "\t";
			params.eolChars = "\n";
			params.maxIndentLevel = 255;
			params.ensureConformity = true;
			params.autoCloseTags = false;
			params.indentAttributes = false;
			params.indentOnly = false;

			testPrettyPrint(xml, ref, params);
		}

		TEST_METHOD(PrettyPrintTest02) {
			XmlFormaterParamsType params;
			params.indentChars = "\t";
			params.eolChars = '\n';
			params.maxIndentLevel = 255;
			params.ensureConformity = true;
			params.autoCloseTags = true;
			params.indentAttributes = false;
			params.indentOnly = false;

			std::string xml = readFile("PrettyPrint\\FullTest.in.xml");
			std::string ref = readFile("PrettyPrint\\FullTest.out.xml");

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

		//--------------------------------------------------------------------------------------------

		// Indent attributes
		TEST_METHOD(IndentAttrTest01) {
			std::string xml("<p xmlns:ns=\"x\" a=\"1\" b c=\"2\"><x ns:d ns:e=\"x\"/><y ns:d ns:e=\"y\">z</y></p>");
			std::string ref("<p xmlns:ns=\"x\"\n   a=\"1\"\n   b\n   c=\"2\">\n\t<x ns:d\n\t   ns:e=\"x\"/>\n\t<y ns:d\n\t   ns:e=\"y\">z</y>\n</p>");

			XmlFormaterParamsType params;
			params.indentChars = "\t";
			params.eolChars = "\n";
			params.maxIndentLevel = 255;
			params.ensureConformity = true;
			params.autoCloseTags = false;
			params.indentAttributes = true;
			params.indentOnly = false;

			testIndentAttr(xml, ref, params);
		}

		//--------------------------------------------------------------------------------------------

		// Indent only
		TEST_METHOD(IndentOnlyTest01) {
			std::string xml("<p xmlns:ns=\"x\"\na=\"1\"\nb\nc=\"2\">\n<x ns:d\n\tns:e=\"x\"/>\n\t<y ns:d\n\tns:e=\"y\">z</y>\n</p>");
			std::string ref("<p xmlns:ns=\"x\"\n   a=\"1\"\n   b\n   c=\"2\">\n\t<x ns:d\n\t   ns:e=\"x\"/>\n\t<y ns:d\n\t   ns:e=\"y\">z</y>\n</p>");

			XmlFormaterParamsType params;
			params.indentChars = "\t";
			params.eolChars = "\n";
			params.maxIndentLevel = 255;
			params.ensureConformity = true;
			params.autoCloseTags = false;
			params.indentAttributes = false;
			params.indentOnly = true;

			testIndentOnly(xml, ref, params);
		}

		// Indent only
		TEST_METHOD(IndentOnlyTest02) {
			std::string xml("<p xmlns:ns=\"x\"   a=\"1\" b c=\"2\">\n<x ns:d ns:e=\"x\"/>\n<y ns:d\n\tns:e=\"y\">z</y>\n</p>");
			std::string ref("<p xmlns:ns=\"x\" a=\"1\" b c=\"2\">\n\t<x ns:d ns:e=\"x\"/>\n\t<y ns:d\n\t   ns:e=\"y\">z</y>\n</p>");

			XmlFormaterParamsType params;
			params.indentChars = "\t";
			params.eolChars = "\n";
			params.maxIndentLevel = 255;
			params.ensureConformity = true;
			params.autoCloseTags = false;
			params.indentAttributes = false;
			params.indentOnly = true;

			testIndentOnly(xml, ref, params);
		}

		//--------------------------------------------------------------------------------------------

		// Linearize

		TEST_METHOD(LinearizeTest01) {
			std::string xml("<p> <x> </x>  <y/> <x> </x> z <y/> <x/>  <y/> <x/> z <y/> </p>");
			std::string ref("<p><x> </x><y/><x> </x> z <y/><x/><y/><x/> z <y/></p>");

			XmlFormaterParamsType params;
			params.indentChars = "\t";
			params.eolChars = "\n";
			params.maxIndentLevel = 255;
			params.ensureConformity = true;
			params.autoCloseTags = false;
			params.indentAttributes = false;
			params.indentOnly = false;

			testLinearize(xml, ref, params);
		}

		TEST_METHOD(LinearizeTest02) {
			std::string xml("<p> <x> </x>  <y/> <x> </x> z <y/> <x/>  <y/> <x/> z <y/> </p>");
			std::string ref("<p><x></x><y/><x></x>z<y/><x/><y/><x/>z<y/></p>");

			XmlFormaterParamsType params;
			params.indentChars = "\t";
			params.eolChars = "\n";
			params.maxIndentLevel = 255;
			params.ensureConformity = false;
			params.autoCloseTags = false;
			params.indentAttributes = false;
			params.indentOnly = false;

			testLinearize(xml, ref, params);
		}

		TEST_METHOD(LinearizeTest03) {
			std::string xml("<p> <x> </x>  <y/> <x> </x> z <y/> <x/>  <y/> <x/> z <y/> <a></a></p>");
			std::string ref("<p><x> </x><y/><x> </x> z <y/><x/><y/><x/> z <y/><a/></p>");

			XmlFormaterParamsType params;
			params.indentChars = "\t";
			params.eolChars = "\n";
			params.maxIndentLevel = 255;
			params.ensureConformity = true;
			params.autoCloseTags = true;
			params.indentAttributes = false;
			params.indentOnly = false;

			testLinearize(xml, ref, params);
		}

		TEST_METHOD(LinearizeTest04) {
			std::string xml("<p> <x> </x>  <y/> <x> </x> z <y/> <x/>  <y/> <x/> z <y/> </p>");
			std::string ref("<p><x/><y/><x/>z<y/><x/><y/><x/>z<y/></p>");

			XmlFormaterParamsType params;
			params.indentChars = "\t";
			params.eolChars = "\n";
			params.maxIndentLevel = 255;
			params.ensureConformity = false;
			params.autoCloseTags = true;
			params.indentAttributes = false;
			params.indentOnly = false;

			testLinearize(xml, ref, params);
		}

		//--------------------------------------------------------------------------------------------

		// Current path

		TEST_METHOD(CurrentPathTest01) {
			std::string xml("<x:a xmlns:x=\"sample\"><x:b><c/><d></d></x:b></x:a>"); 
			std::string ref("/x:a/x:b/d");

			XmlFormater formater(xml.c_str(), xml.length());
			std::stringstream* out = formater.currentPath(34, true);
			std::string tmp(out->str());

			Assert::IsTrue(0 == tmp.compare(ref.c_str()));

			ref = "/a/b/d";
			out = formater.currentPath(34, false);
			tmp = out->str();

			Assert::IsTrue(0 == tmp.compare(ref.c_str()));
		}

		TEST_METHOD(CurrentPathTest02) {
			std::string xml("<x:a xmlns:x=\"sample\"><x:b><c/><d></d></x:b></x:a>");
			std::string ref("/x:a/x:b/d");

			XmlFormater formater(xml.c_str(), xml.length());
			std::stringstream* out = formater.currentPath(33, true);
			std::string tmp(out->str());

			Assert::IsTrue(0 == tmp.compare(ref.c_str()));

			ref = "/a/b/d";
			out = formater.currentPath(33, false);
			tmp = out->str();

			Assert::IsTrue(0 == tmp.compare(ref.c_str()));
		}

		TEST_METHOD(CurrentPathTest03) {
			std::string xml("<x:a xmlns:x=\"sample\"><x:b><c/><d></d></x:b></x:a>");
			std::string ref("/x:a/x:b");

			XmlFormater formater(xml.c_str(), xml.length());
			std::stringstream* out = formater.currentPath(31, true);
			std::string tmp(out->str());

			Assert::IsTrue(0 == tmp.compare(ref.c_str()));

			ref = "/a/b";
			out = formater.currentPath(31, false);
			tmp = out->str();

			Assert::IsTrue(0 == tmp.compare(ref.c_str()));
		}

		TEST_METHOD(CurrentPathTest04) {
			std::string xml("<x:a xmlns:x='x' x='1' y='2'><x:b x='3'><c x='4'/><d x:x='5'></d></x:b></x:a>");
			std::string ref("/x:a/x:b/d/@x:x");

			XmlFormater formater(xml.c_str(), xml.length());
			std::stringstream* out = formater.currentPath(58, true);
			std::string tmp(out->str());

			Assert::IsTrue(0 == tmp.compare(ref.c_str()));

			ref = "/a/b/d/@x";
			out = formater.currentPath(58, false);
			tmp = out->str();

			Assert::IsTrue(0 == tmp.compare(ref.c_str()));
		}
	};
}
