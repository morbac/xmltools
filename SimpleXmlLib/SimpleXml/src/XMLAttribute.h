#ifndef XMLATTRIBUTE_HEADER_FILE_H
#define XMLATTRIBUTE_HEADER_FILE_H

#include <string>

namespace SimpleXml {

	struct XMLAttribute {
	public:
		std::string_view name;
		std::string_view value;

	};

}

#endif