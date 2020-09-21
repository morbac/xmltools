#ifndef ERRORLIST_HEADER_FILE_H
#define ERRORLIST_HEADER_FILE_H


#include <string>
#include <list>

namespace SimpleXml {
	
	struct Error {
		enum class Type {
			Info,
			Warning,
			Error
		}type = Type::Error;

		std::string resourceLocation;
		size_t line = 0;
		size_t column = 0;

		std::string message;
	};

	class ErrorList {
		std::list<Error> _errors;
	public:
		void add(Error e) {
			_errors.push_back(e);
		}

		void clear() {
			_errors.clear();
		}

		bool empty() { return _errors.empty(); }
		auto begin() { return _errors.begin(); }
		auto end() { return _errors.end(); }
	};
}

#endif