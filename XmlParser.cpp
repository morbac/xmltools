#include "XmlParser.h"

XmlParser::XmlParser(const char* data, size_t length) {
	this->srcText = data;
	this->srcLength = length;

	this->reset();
}

XmlParser::~XmlParser() {
	this->currtoken.chars = NULL;
	this->srcText = NULL;
}

void XmlParser::reset() {
	this->inTag = false;
	this->hasAttrName = false;
	this->currpos = 0;

	this->currtoken = { XmlTokenType::Undefined, NULL, 0 };
}

XmlToken XmlParser::parseNext() {
	this->currtoken = this->fetchToken();
	return this->currtoken;
}

XmlToken XmlParser::fetchToken() {
	char currentchar;

	const char* cursor = this->srcText + this->currpos;
	const char* startpos = this->srcText + this->currpos;

	if (this->currpos >= this->srcLength) {
		return { XmlTokenType::EndOfFile, this->srcText + this->srcLength, 0 };
	}

	while (this->currpos < this->srcLength) {
		currentchar = cursor[0];
		if (currentchar == '<') {
			// @todo : prevent buffer overflow
			if (cursor[1] == '?') {
				// <?xml ...?>
				// let's leave it untouched
				return { XmlTokenType::Instruction,
						 startpos,
				         this->readUntil("?>", 0, true) };
			}
			else if (cursor[1] == '%') {
				// not really xml, but for jsp compatibility
				// let's leave it untouched
				return { XmlTokenType::Instruction,
						 startpos,
						 this->readUntil("%>", 0, true) };
			}
			else if (cursor[1] == '!' && cursor[2] == '-' && cursor[3] == '-') {
				// <!--
				// let's leave it untouched
				return { XmlTokenType::Comment,
						 startpos,
						 this->readUntil("-->", 0, true) };
			}
			else if (cursor[1] == '!' && cursor[2] == '[' && cursor[3] == 'C' && cursor[4] == 'D' &&
				cursor[5] == 'A' && cursor[6] == 'T' && cursor[7] == 'A' && cursor[8] == '[') {
				// <![CDATA[
				// let's leave it untouched
				return { XmlTokenType::CDATA,
						 startpos,
						 this->readUntil("]]>", 0, true) };
			}
			else if (cursor[1] == '/') {
				// </ns:sample
				this->inTag = false;
				return { XmlTokenType::TagClosing,
						 startpos,
						 this->readUntilFirstOf(" >") };
			}
			else {
				// parsing tag name like "<sample" or "<ns:sample"
				this->inTag = true;
				return { XmlTokenType::TagOpening,
					     startpos,
				         this->readUntilFirstOf(" />\t\r\n") };
			}
			break;
		}
		else if (currentchar == '>') {
			if (this->inTag) {
				this->inTag = false;
				return { XmlTokenType::TagOpeningEnd,
						 startpos,
						 this->readChars(1) };
			}
			else {
				return { XmlTokenType::TagClosingEnd,
						 startpos,
						 this->readChars(1) };
			}
		}
		else if (this->inTag) {
			// parse tag content

			if (currentchar == '\r' || currentchar == '\n') {
				return { XmlTokenType::LineBreak,
							startpos,
							this->readUntilFirstNotOf("\r\n") };
			}
			else if (currentchar == '/') {
				if (cursor[1] == '>') {
					this->inTag = false;
					return { XmlTokenType::TagSelfClosingEnd,
							 startpos,
							 this->readChars(2) };
				}
				else {
					return { XmlTokenType::Undefined,
							 startpos,
							 this->readChars(1) };
				}
			}
			else if (currentchar == '=') {
				return { XmlTokenType::Equal,
						 startpos,
						 this->readChars(1) };
			}
			else if (currentchar == ' ' || currentchar == '\t') {
				// parsing whitespace
				return { XmlTokenType::Whitespace,
						 startpos,
						 this->readUntilFirstNotOf(" \t") };
			}
			else if (this->hasAttrName) {
				this->hasAttrName = false;
				char valDelimiter[2] = { currentchar, '\0' };
				if (currentchar == '\"' || currentchar == '\'') {
					// standard value, delimited with " or '
					return { XmlTokenType::AttrValue,
							 startpos,
							 this->readUntilFirstOf(valDelimiter, 1, true) };	// skip actual delimiter + parse content
				}
				else {
					// attribute with no value
					this->hasAttrName = true;
					return { XmlTokenType::AttrName,
							 startpos,
							 this->readUntilFirstOf("= /\t\r\n") };
				}
			} else {
				this->hasAttrName = true;
				return { XmlTokenType::AttrName,
					     startpos,
					     this->readUntilFirstOf("= /\t\r\n") };
			}
		}
		else {
			// parsing text
			return { XmlTokenType::Text,
				     startpos,
				     this->readUntilFirstOf("<") };
		}
	}

	return { XmlTokenType::Undefined, startpos, 1 };
}

size_t XmlParser::isIncoming(const char* characters) {
	const char* cursor = this->srcText + this->currpos;
	const char* tmp = strstr(cursor, characters);
	if (!tmp) return -1;
	return tmp - cursor;
}

size_t XmlParser::readChars(size_t nchars) {
	if (this->currpos + nchars > this->srcLength) {
		nchars = this->srcLength - this->currpos;
	}
	this->currpos += nchars;
	return nchars;
}

size_t XmlParser::readNextWord() {
	return this->readUntilFirstOf(DELIMITER_CHARS);
}

size_t XmlParser::readUntilFirstOf(const char* characters, size_t offset, bool goAfter) {
	size_t res = 0;
	if (offset > 0) offset = this->readChars(offset);
	const char* cursor = this->srcText + this->currpos;
	const char* tmp = strpbrk(cursor, characters);
	if (!tmp) tmp = this->srcText + this->srcLength;
	res = tmp - cursor;
	if (goAfter) {
		++res;
		if (this->currpos + res > this->srcLength) {
			res = this->srcLength - this->currpos;
		}
	}
	this->currpos += res;
	return res + offset;
}

size_t XmlParser::readUntilFirstNotOf(const char* characters, size_t offset) {
	// @todo optimize this
	size_t res = 0;
	char cursor;
	size_t i;
	if (offset > 0) offset = this->readChars(offset);
	size_t len = strlen(characters);
	while (this->currpos < this->srcLength) {
		cursor = (this->srcText + this->currpos)[0];
		for (i = 0; i < len; ++i) {
			if (cursor == characters[i]) {
				break;
			}
		}
		if (i >= len) {
			return res;
		}
		++res;
		++this->currpos;
	}
	return res + offset;
}

size_t XmlParser::readUntil(const char* delimiter, size_t offset, bool goAfter) {
	size_t res = 0;
	if (offset > 0) offset = this->readChars(offset);
	const char* cursor = this->srcText + this->currpos;
	const char* tmp = strstr(cursor, delimiter);
	if (!tmp) tmp = this->srcText + this->srcLength;
	res = tmp - cursor;
	if (goAfter) {
		size_t len = strlen(delimiter);
		res += len;
		
	}
	this->currpos += res;
	return res + offset;
}

std::string XmlParser::getTokenName() {
	switch (this->currtoken.type) {
		case XmlTokenType::TagOpening: return "TAG_OPENING";
		case XmlTokenType::TagClosing: return "TAG_CLOSING";
		case XmlTokenType::TagOpeningEnd: return "TAG_OPENING_END";
		case XmlTokenType::TagClosingEnd: return "TAG_CLOSING_END";
		case XmlTokenType::TagSelfClosingEnd: return "TAG_SELFCLOSING_END";
		case XmlTokenType::AttrName: return "ATTR_NAME";
		case XmlTokenType::AttrValue: return "ATTR_VALUE";
		case XmlTokenType::Text: return "TEXT";
		case XmlTokenType::Whitespace: return "WHITESPACE";
		case XmlTokenType::Instruction: return "INSTRUCTION";
		case XmlTokenType::Comment: return "COMMENT";
		case XmlTokenType::CDATA: return "CDATA";
		case XmlTokenType::LineBreak: return "LINEBREAK";
		case XmlTokenType::Equal: return "EQUAL";
		case XmlTokenType::EndOfFile: return "EOF";
		default: return "UNDEFINED";
	}
}

std::string XmlParser::dumpTokens() {
	this->reset();

	XmlToken token;
	std::stringstream out;

	while ((token = this->parseNext()).type != XmlTokenType::EndOfFile) {
		std::string tmp = this->getTokenName();

		out.write("/", 1);
		out.write(tmp.c_str(), tmp.length());
	}

	return out.str().erase(0,1);
}