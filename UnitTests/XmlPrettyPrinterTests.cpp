#include "StdAfx.h"

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include "../XmlPrettyPrinter.h"

static bool startsWith(const char* str, const char* pre) {
    size_t lenstr = strlen(str), lenpre = strlen(pre);
    return lenstr < lenpre ? false : memcmp(pre, str, lenpre) == 0;
}

static bool endsWith(const std::string& str, const std::string& suffix) { // waiting for C++20....
    return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

static bool contains(const char* str, const char* word) {
    return (strstr(str, word) != NULL);
}

static std::string replace(std::string& s, const std::string& find, const char *replace) {
    auto pos = s.find(find, find.size());
    if (pos != std::string::npos) {
        s.replace(pos, find.size(), replace);
    }
    return s;
}

char* readFile(const fs::directory_entry& entry) {
    auto fsize = entry.file_size();
    if (fsize > INT_MAX - 1)
        return NULL;
    size_t size = fsize;

    std::ifstream inputStream(entry.path().string(), std::ios_base::binary | std::ios_base::in);
    if (!inputStream.is_open()) {
        return NULL;
    }
    char* input = new char[(size + 1)];
    input[size] = 0;
    inputStream.read(input, size);
    if (inputStream.fail()) {
        delete[] input;
        return NULL;;
    }
    
    return input;
}

void iterateFolder(const wchar_t * path, bool (*action)(const char* input, const char* expectedOutput)) {
    wchar_t CURPATH[_MAX_PATH];
    if (NULL == _wgetcwd(CURPATH, _MAX_PATH)) {
        FAIL() << L"Unable to get current working directory";
    }
    wcscat_s(CURPATH, L"\\TestFiles\\");
    wcscat_s(CURPATH, path);

    if (!fs::is_directory(CURPATH)) {
        FAIL() << CURPATH <<  L" is not a valid directory";
        return;
        }

    bool filesProcessed = false;
    for (const auto& entry : fs::directory_iterator(CURPATH)) {
        std::string inpath = entry.path().string();
        if (endsWith(inpath, ".in.xml")) {
            fs::path outpath = replace(inpath, ".in.xml", ".out.xml");
            auto outEntry = fs::directory_entry(outpath);

            auto input = readFile(entry);
            auto output = readFile(outEntry);

            if (input != NULL && output != NULL) {
                bool ok = action(input, output);
                filesProcessed = true;
                if (!ok){
                    FAIL() << entry.path().filename().string() << " FAILED";
                }
            }

            if (input) delete[] input;
            if (output) delete[] output;
        }
    }
    EXPECT_TRUE(filesProcessed) << "No files processed";
}

bool testPrettyPrint(const char* input, const char* expectedOutput) {

    PrettyPrintParms parms;
    parms.eol = "\r\n";
    parms.tab = "\t";
    parms.autocloseEmptyElements = true;
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = true;

    auto prettyPrinter = XmlPrettyPrinter(input, strlen(input), parms);
    prettyPrinter.Convert();
    const std::string& output = prettyPrinter.Stream()->str();
    return 0 == strcmp(expectedOutput, output.c_str());
}

TEST(PrettyPrint, Default) {
    iterateFolder(L"PrettyPrint", testPrettyPrint);
}

bool testPrettyPrintIndentAttributes(const char* input, const char* expectedOutput) {

    PrettyPrintParms parms;
    parms.eol = "\r\n";
    parms.tab = "\t";
    parms.autocloseEmptyElements = false;
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = true;
    parms.indentAttributes = true;

    if (contains(input, "<!-- SPACE INDENTED / Do not remove this comment -->")) {
        parms.tab = "  ";
    }

    auto prettyPrinter = XmlPrettyPrinter(input, strlen(input), parms);
    prettyPrinter.Convert();
    const std::string& output = prettyPrinter.Stream()->str();
    return 0 == strcmp(expectedOutput, output.c_str());
}

TEST(PrettyPrint, IndentAttr) {
    iterateFolder(L"PrettyPrintIndentAttributes", testPrettyPrintIndentAttributes);
}

bool testPrettyPrintIndentOnly(const char* input, const char* expectedOutput) {

    PrettyPrintParms parms;
    parms.eol = "\r\n";
    parms.tab = "\t";
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = true;
    parms.autocloseEmptyElements = true;
    parms.keepExistingBreaks = true;

    auto prettyPrinter = XmlPrettyPrinter(input, strlen(input), parms);
    prettyPrinter.Convert();
    const std::string& output = prettyPrinter.Stream()->str();
    return 0 == strcmp(expectedOutput, output.c_str());
}

TEST(PrettyPrint, IndentOnly) {
    iterateFolder(L"PrettyPrintIndentOnly", testPrettyPrintIndentOnly);
}