#include <fstream>
#include <filesystem>
#include <functional>

namespace fs = std::filesystem;

#include "gtest/gtest.h"
#include "PrettyPrinter.h"
#include "EmbeddedResources.h"

using namespace SimpleXml;

bool testPrettyPrint(PrettyPrintParms parms, std::string &input, std::string &expectedOutput) {
    ChunkedStream stream(input.c_str(), input.size());
    auto prettyPrinter = PrettyPrinter(stream, parms);
    prettyPrinter.Convert();
    const std::string& output = prettyPrinter.Stream()->str();
    return expectedOutput == output;
}

/*
    Until C++ 20, we will use a constructor function to initialize the structure.
    Otherwise we would need to be very careful about initialization order which is brittle.
*/

PrettyPrintParms parmsDefault() {
    PrettyPrintParms parms;
    parms.eol = "\r\n";
    parms.tab = "\t";
    parms.autocloseEmptyElements = true;
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = true;

    return parms;
}

PrettyPrintParms parmsIndentAttribute() {
    PrettyPrintParms parms;
    parms.eol = "\r\n";
    parms.tab = "\t";
    parms.autocloseEmptyElements = false;
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = true;
    parms.indentAttributes = true;

    return parms;
}

PrettyPrintParms parmsIndentAttributeSpace() {
    PrettyPrintParms parms;
    parms.eol = "\r\n";
    parms.tab = "  ";
    parms.autocloseEmptyElements = false;
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = true;
    parms.indentAttributes = true;

    return parms;
}

PrettyPrintParms parmsIndentAttributeTab() {
    PrettyPrintParms parms;
    parms.eol = "\r\n";
    parms.tab = "\t";
    parms.autocloseEmptyElements = false;
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = true;
    parms.indentAttributes = true;

    return parms;
}

PrettyPrintParms parmsIndentOnly() {
    PrettyPrintParms parms;
    parms.eol = "\r\n";
    parms.tab = "\t";
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = true;
    parms.autocloseEmptyElements = true;
    parms.keepExistingBreaks = true;

    return parms;
}

TEST(PrettyPrint, IndentAttrSpace) {
    auto in = LoadEmbeddedResourceText(RC_test_pp_attributes_space_indented_in);
    auto out = LoadEmbeddedResourceText(RC_test_pp_attributes_space_indented_out);

    ASSERT_TRUE(testPrettyPrint(parmsIndentAttributeSpace(), in, out));
}

TEST(PrettyPrint, IndentAttrTab) {
    auto in = LoadEmbeddedResourceText(RC_test_pp_attributes_tab_indented_in);
    auto out = LoadEmbeddedResourceText(RC_test_pp_attributes_tab_indented_out);

    ASSERT_TRUE(testPrettyPrint(parmsIndentAttributeTab(),in, out));
}

TEST(PrettyPrint, IndentOnly) {
    auto in = LoadEmbeddedResourceText(RC_test_pp_indent_input_1);
    auto out = LoadEmbeddedResourceText(RC_test_pp_indent_output_1);

    ASSERT_TRUE(testPrettyPrint(parmsIndentOnly(), in, out));
}

TEST(PrettyPrint, AutoCloseEmptyElement) {
    auto in = LoadEmbeddedResourceText(RC_test_pp_AutoCloseEmptyElement_in);
    auto out = LoadEmbeddedResourceText(RC_test_pp_AutoCloseEmptyElement_out);

    ASSERT_TRUE(testPrettyPrint(parmsDefault(),in, out));
}

TEST(PrettyPrint, AutoCloseEmptyElementChild) {
    auto in = LoadEmbeddedResourceText(RC_test_pp_AutoCloseEmptyElement_child_in);
    auto out = LoadEmbeddedResourceText(RC_test_pp_AutoCloseEmptyElement_child_out);

    ASSERT_TRUE(testPrettyPrint(parmsDefault(),in, out));
}

TEST(PrettyPrint, Comment_After_PI) {
    auto in = LoadEmbeddedResourceText(RC_test_pp_Comment_After_PI_in);
    auto out = LoadEmbeddedResourceText(RC_test_pp_Comment_After_PI_out);

    ASSERT_TRUE(testPrettyPrint(parmsDefault(),in, out));
}
TEST(PrettyPrint, KeepTextWhitespace) {
    auto in = LoadEmbeddedResourceText(RC_test_pp_FullTest_in);
    auto out = LoadEmbeddedResourceText(RC_test_pp_FullTest_out);

    ASSERT_TRUE(testPrettyPrint(parmsDefault(),in, out));
}

TEST(PrettyPrint, markupdecl) {
    auto in = LoadEmbeddedResourceText(RC_test_pp_markupdecl_in);
    auto out = LoadEmbeddedResourceText(RC_test_pp_markupdecl_out);

    ASSERT_TRUE(testPrettyPrint(parmsDefault(),in, out));
}
TEST(PrettyPrint, Test1) {
    auto in = LoadEmbeddedResourceText(RC_test_pp_Test1_in);
    auto out = LoadEmbeddedResourceText(RC_test_pp_Test1_out);

    ASSERT_TRUE(testPrettyPrint(parmsDefault(),in, out));
}
TEST(PrettyPrint, xmltag_not_closed) {
    auto in = LoadEmbeddedResourceText(RC_test_pp_xmltag_not_closed_in);
    auto out = LoadEmbeddedResourceText(RC_test_pp_xmltag_not_closed_out);

    ASSERT_TRUE(testPrettyPrint(parmsDefault(),in, out));
}

