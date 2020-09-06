#pragma once
#include <string>

namespace SimpleXml {

	struct InlineString {

		// if you use this variable directly you should reconsider
		const char* __text;

		// if you use this variable directly you should reconsider
		size_t __size;

		inline const char* text() const { return __text; }
		inline size_t size() const { return __size; }
		//inline void setEnd(const char* end) { __size = end - __text; }
		inline const char * end() const { return __text + __size; }

		inline void clear() {
			__text = NULL;
			__size = 0;
		}

		InlineString() {
			clear();
		}

		InlineString(const char *txt, size_t s) {
			__text = txt;
			__size = s;
		}

		bool operator ==(const char* right) const {
			if (strlen(right) != __size)
				return false;

			return strncmp(__text, right, __size) == 0;
		}

		bool operator !=(const char* right) const {
			if (strlen(right) != __size)
				return true;

			return strncmp(__text, right, __size) != 0;
		}

		bool operator ==(const InlineString& right) const {
			return __size == right.__size && strncmp(__text, right.__text, __size) == 0;
		}

		bool operator ==(const std::string& right) const {
			return __size == right.size() && strncmp(__text, right.c_str(), __size) == 0;
		}

		operator std::string() const {
			return string();
		}

		std::string string() const {
			return std::string(__text, __size);
		}

		operator bool() const {
			return __text != NULL;
		}
	};
}