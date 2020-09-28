#include "XmlParser.h"

XmlParser::XmlParser(const char* data, size_t length) {
	this->srcText = data;
	this->srcLength = length;

	this->reset();
}

XmlParser::~XmlParser() {
	this->token.chars = NULL;
	this->srcText = NULL;
}

void XmlParser::reset() {
	this->inTag = false;
	this->hasAttrName = false;
	this->curpos = 0;
}

XmlToken XmlParser::parseNext() {
	this->token = this->nextToken();

	return this->token;
}

XmlToken XmlParser::nextToken() {
	char currentchar;

	const char* cursor = this->srcText + this->curpos;
	const char* startpos = this->srcText + this->curpos;

	if (this->curpos >= this->srcLength) {
		return { XmlTokenType::EndOfFile, this->srcText + this->srcLength, 0 };
	}

	while (this->curpos < this->srcLength) {
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
				         this->readUntilFirstOf("  />\t\r\n") };
			}
			break;
		}
		else if (currentchar == '/') {
			if (cursor[1] == '>') {
				this->inTag = false;
				return { XmlTokenType::TagSelfClosingEnd,
					     startpos,
					     this->readChars(2) };
			}
			else {
				return { XmlTokenType::Unknown,
					     startpos,
					     this->readChars(1) };
			}
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
		else if (currentchar == '=') {
			return { XmlTokenType::CharEQ,
				     startpos,
				     this->readChars(1) };
		}
		else if (currentchar == '\r' || currentchar == '\n') {
			return { XmlTokenType::LineBreak,
					 startpos,
					 this->readUntilFirstNotOf("\r\n") };
		}
		else if (currentchar == ' ' || currentchar == '\t' || currentchar == ' ') {
			// parsing whitespace
			return { XmlTokenType::Whitespace,
					 startpos,
					 this->readUntilFirstNotOf(" \t ") };
		} else if (this->inTag) {
			// let's parse attributes
			if (this->hasAttrName) {
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
							 this->readUntilFirstOf("=  /\t\r\n") };
				}
			} else {
				this->hasAttrName = true;
				return { XmlTokenType::AttrName,
					     startpos,
					     this->readUntilFirstOf("=  /\t\r\n") };
			}
		} else {
			// parsing text
			return { XmlTokenType::Text,
				     startpos,
				     this->readUntilFirstOf("< \t ") };
		}
	}

	return { XmlTokenType::Unknown, startpos, 1 };
}

size_t XmlParser::isIncoming(const char* characters) {
	const char* cursor = this->srcText + this->curpos;
	const char* tmp = strstr(cursor, characters);
	if (!tmp) return -1;
	return tmp - cursor;
}

size_t XmlParser::readChars(size_t nchars) {
	if (this->curpos + nchars > this->srcLength) {
		nchars = this->srcLength - this->curpos;
	}
	this->curpos += nchars;
	return nchars;
}

size_t XmlParser::readNextWord() {
	return this->readUntilFirstOf(DELIMITER_CHARS);
}

size_t XmlParser::readUntilFirstOf(const char* characters, size_t offset, bool goAfter) {
	size_t res = 0;
	if (offset > 0) offset = this->readChars(offset);
	const char* cursor = this->srcText + this->curpos;
	const char* tmp = strpbrk(cursor, characters);
	if (!tmp) tmp = this->srcText + this->srcLength;
	res = tmp - cursor;
	if (goAfter) {
		++res;
		if (this->curpos + res > this->srcLength) {
			res = this->srcLength - this->curpos;
		}
	}
	this->curpos += res;
	return res + offset;
}

size_t XmlParser::readUntilFirstNotOf(const char* characters, size_t offset) {
	// @todo optimize this
	size_t res = 0;
	char cursor;
	size_t i;
	if (offset > 0) offset = this->readChars(offset);
	size_t len = strlen(characters);
	while (this->curpos < this->srcLength) {
		cursor = (this->srcText + this->curpos)[0];
		for (i = 0; i < len; ++i) {
			if (cursor == characters[i]) {
				break;
			}
		}
		if (i >= len) {
			return res;
		}
		++res;
		++this->curpos;
	}
	return res + offset;
}

size_t XmlParser::readUntil(const char* delimiter, size_t offset, bool goAfter) {
	size_t res = 0;
	if (offset > 0) offset = this->readChars(offset);
	const char* cursor = this->srcText + this->curpos;
	const char* tmp = strstr(cursor, delimiter);
	if (!tmp) tmp = this->srcText + this->srcLength;
	res = tmp - cursor;
	if (goAfter) {
		size_t len = strlen(delimiter);
		res += len;
		
	}
	this->curpos += res;
	return res + offset;
}

std::string XmlParser::getTokenName() {
	switch (this->token.type) {
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
		case XmlTokenType::CharEQ: return "EQUAL";
		case XmlTokenType::EndOfFile: return "EOF";
		case XmlTokenType::Unknown: return "UNKNOWN";
		case XmlTokenType::None: return "NONE";
		default: return "UNDEFINED";
	}
	return "UNEXPECTED";
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