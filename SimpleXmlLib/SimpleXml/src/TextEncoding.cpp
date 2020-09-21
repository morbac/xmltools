#include "TextEncoding.h"

#include "windows.h"


std::wstring utf82ws(std::string utf8) {
	std::wstring ret;
	ret.resize(::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.size(), 0, 0));
	::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.size(), &ret[0], (int)ret.size());
	return ret;
}

std::string ws2utf8(std::wstring ws) {
	std::string ret;
	ret.resize(::WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(), 0, 0,0,0));
	::WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(), &ret[0], (int)ret.size(),0,0);
	return ret;
}
