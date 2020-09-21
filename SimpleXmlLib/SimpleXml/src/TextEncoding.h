#ifndef TEXTENCODING_HEADER_FILE_H
#define TEXTENCODING_HEADER_FILE_H

#include <string>

std::wstring utf82ws(std::string utf8);
std::string ws2utf8(std::wstring ws);

#endif