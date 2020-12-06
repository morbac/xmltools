#include "CppUnitTest.h"

#include <windows.h>
#include <fstream>
#include <string>
#include <streambuf>

#include "StringXml.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace StringXml;

namespace StringXmlTests {
	std::string xmltestsfileshome("D:\\Progs\\C++\\xmltools\\SimpleXmlLib\\SimpleXmlTests\\TestFiles\\");

	TEST_CLASS(StringXmlTests) {
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

		void testPrettyPrint(std::string xml, std::string ref, XmlFormaterParamsType params) {
			XmlFormater formater(xml.c_str(), xml.length(), params);
			std::string &out = formater.prettyPrint();
			std::string tmp = out;

			Assert::IsTrue(0 == ref.compare(tmp));

			// check that a second pretty print keeps structure unchanged
			formater.init(tmp.c_str(), tmp.length(), params);
			out = formater.prettyPrint();
			tmp = out;

			Assert::IsTrue(0 == ref.compare(tmp));
		}

		void testIndentAttr(std::string xml, std::string ref, XmlFormaterParamsType params) {
			params.indentAttributes = true;

			XmlFormater formater(xml.c_str(), xml.length(), params);
			std::string &out = formater.prettyPrintAttr();
			std::string tmp = out;

			Assert::IsTrue(0 == ref.compare(tmp));

			// check that a second pretty print keeps structure unchanged
			formater.init(tmp.c_str(), tmp.length(), params);
			out = formater.prettyPrint();
			tmp = out;

			Assert::IsTrue(0 == ref.compare(tmp));
		}

		void testIndentOnly(std::string xml, std::string ref, XmlFormaterParamsType params) {
			params.indentOnly = true;

			XmlFormater formater(xml.c_str(), xml.length(), params);
			std::string &out = formater.prettyPrintIndent();
			std::string tmp = out;

			Assert::IsTrue(0 == ref.compare(tmp));

			// check that a second pretty print keeps structure unchanged
			formater.init(tmp.c_str(), tmp.length(), params);
			out = formater.prettyPrint();
			tmp = out;

			Assert::IsTrue(0 == ref.compare(tmp));
		}

		void testLinearize(std::string xml, std::string ref, XmlFormaterParamsType params) {
			XmlFormater formater(xml.c_str(), xml.length(), params);
			std::string &out = formater.linearize();
			std::string tmp = out;

			Assert::IsTrue(0 == ref.compare(tmp));

			// check that a second linearize keeps structure unchanged
			formater.init(tmp.c_str(), tmp.length(), params);
			out = formater.linearize();
			tmp = out;

			Assert::IsTrue(0 == ref.compare(tmp));

			// now we verify that pretty printing the orignal and the linearized
			// version produce the same when conformity enforcement is activated
			if (params.ensureConformity) {
				formater.init(tmp.c_str(), tmp.length(), params);
				out = formater.prettyPrint();
				std::string tmp2 = tmp = out;

				formater.init(xml.c_str(), xml.length(), params);
				out = formater.prettyPrint();
				tmp = out;

				Assert::IsTrue(0 == tmp2.compare(tmp));
			}
		}

	public:

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
			std::string xml("<a>\n<b/>\n\n<!--test-->\n<d/>\n</a>");
			std::string ref("<a>\n\t<b/>\n\t<!--test-->\n\t<d/>\n</a>");

			XmlFormaterParamsType params;
			params.indentChars = "\t";
			params.eolChars = "\n";
			params.maxIndentLevel = 255;
			params.ensureConformity = false;
			params.autoCloseTags = true;
			params.indentAttributes = false;
			params.indentOnly = false;

			testPrettyPrint(xml, ref, params);
		}

		TEST_METHOD(PrettyPrintTest03) {
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
			std::string &out = formater.prettyPrint();
			std::string tmp(out);
			tmp.append(params.eolChars);	// the sample has as added final CRLF
			//writeFile(tmp, L"D:\\Progs\\C++\\xmltools\\SimpleXmlLib\\SimpleXmlTests\\TestFiles\\PrettyPrint\\FullTest.test.xml");

			Assert::IsTrue(0 == tmp.compare(ref.c_str()));

			// check that a second pretty print keeps structure unchanged
			formater.init(tmp.c_str(), tmp.length(), params);
			out = formater.prettyPrint();
			tmp = out;

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
	};
}
