#include "XmlParser.h"

static inline bool contains(const char* characters, char element) {
	size_t len = strlen(characters);
	for (size_t i = 0; i < len; ++i) {
		if (characters[i] == element) {
			return true;
		}
	}
	return false;
}

XmlParser::XmlParser(const char* data, size_t length) {
	this->srcText = data;
	this->srcLength = length;

	this->reset();
}

XmlParser::~XmlParser() {
	this->prevtoken.chars = NULL;
	this->currtoken.chars = NULL;
	this->nexttoken.chars = NULL;
	this->srcText = NULL;
}

void XmlParser::reset() {
	this->currcontext = { false, false };
	this->hasAttrName = false;
	this->currpos = 0;

	this->prevtoken = { XmlTokenType::Undefined, NULL, 0 };
	this->currtoken = { XmlTokenType::Undefined, NULL, 0 };
	this->nexttoken = { XmlTokenType::Undefined, NULL, 0 };
}

XmlToken XmlParser::getNextStructureToken() {
	// @fixme Should we consider whitespace and linebreaks has structure token in opening tag ?
	if (this->nexttoken.type != XmlTokenType::Whitespace &&
		this->nexttoken.type != XmlTokenType::LineBreak &&
		this->nexttoken.type != XmlTokenType::Text) {
		return this->nexttoken;
	}
	else {
		// let's search in the buffered list
		for (std::list<XmlToken>::iterator it = this->buffer.begin(); it != this->buffer.end(); ++it) {
			if ((*it).type != XmlTokenType::Whitespace &&
				(*it).type != XmlTokenType::LineBreak &&
				(*it).type != XmlTokenType::Text) {
				return (*it);
			}
		}

		// can't find a structure token in the buffered list, let's fetch next tokens
		XmlToken res;
		do {
			res = this->fetchToken();
			this->buffer.push_back(res);

			if (res.type != XmlTokenType::Whitespace &&
				res.type != XmlTokenType::LineBreak &&
				res.type != XmlTokenType::Text) {
				return res;
			}
		} while (res.type != XmlTokenType::EndOfFile);

		return { XmlTokenType::Undefined, NULL, 0 };
	}
}

XmlToken XmlParser::parseNext() {
	if (this->buffer.empty()) {
		do {
			this->prevtoken = this->currtoken;
			this->currtoken = this->nexttoken;
			this->nexttoken = this->fetchToken();
		} while (this->currtoken.type == XmlTokenType::Undefined);
	}
	else {
		this->prevtoken = this->currtoken;
		this->currtoken = this->nexttoken;
		this->nexttoken = this->buffer.front();
		this->buffer.pop_front();
	}

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
			// @todo : prevent reading outside chars buffer
			if (cursor[1] == '?') {
				// <?xml ...?>
				// let's leave it untouched
				this->currcontext.inOpeningTag = false;
				this->currcontext.inClosingTag = false;
				return { XmlTokenType::Instruction,
						 startpos,
				         this->readUntil("?>", 0, true) };
			}
			else if (cursor[1] == '%') {
				// not really xml, but for jsp compatibility
				// let's leave it untouched
				this->currcontext.inOpeningTag = false;
				this->currcontext.inClosingTag = false;
				return { XmlTokenType::Instruction,
						 startpos,
						 this->readUntil("%>", 0, true) };
			}
			else if (cursor[1] == '!' && cursor[2] == '-' && cursor[3] == '-') {
				// <!--
				// let's leave it untouched
				this->currcontext.inOpeningTag = false;
				this->currcontext.inClosingTag = false;
				return { XmlTokenType::Comment,
						 startpos,
						 this->readUntil("-->", 0, true) };
			}
			else if (cursor[1] == '!' && cursor[2] == '[' && cursor[3] == 'C' && cursor[4] == 'D' &&
			         cursor[5] == 'A' && cursor[6] == 'T' && cursor[7] == 'A' && cursor[8] == '[') {
				// <![CDATA[
				// let's leave it untouched
				this->currcontext.inOpeningTag = false;
				this->currcontext.inClosingTag = false;
				return { XmlTokenType::CDATA,
				         startpos,
				         this->readUntil("]]>", 0, true) };
			}
			else if (cursor[1] == '/') {
				// </ns:sample
				this->currcontext.inOpeningTag = false;
				this->currcontext.inClosingTag = true;
				return { XmlTokenType::TagClosing,
						 startpos,
						 this->readUntilFirstOf("> \r\n") };
			}
			else {
				// parsing tag name like "<sample" or "<ns:sample"
				this->currcontext.inOpeningTag = true;
				this->currcontext.inClosingTag = false;
				return { XmlTokenType::TagOpening,
					     startpos,
				         this->readUntilFirstOf(" />\t\r\n") };
			}
			break;
		}
		else if (this->currcontext.inClosingTag) {
			// parsing closing tag content
			if (currentchar == '>') {
				this->currcontext.inClosingTag = false;
				return { XmlTokenType::TagClosingEnd,
						 startpos,
						 this->readChars(1) };
			}
			else if (currentchar == ' ' || currentchar == '\t') {
				return { XmlTokenType::Whitespace,
						 startpos,
						 this->readUntilFirstNotOf(" \t") };
			}
			else if (currentchar == '\r' || currentchar == '\n') {
				return { XmlTokenType::LineBreak,
							startpos,
							this->readUntilFirstNotOf("\r\n") };
			}
			else {
				return { XmlTokenType::Undefined,
							startpos,
							this->readChars(1) };
			}
		}
		else if (this->currcontext.inOpeningTag) {
			// parse opening tag content
			if (currentchar == '>') {
				this->currcontext.inOpeningTag = false;
				return { XmlTokenType::TagOpeningEnd,
						 startpos,
						 this->readChars(1) };
			}
			else if (currentchar == ' ' || currentchar == '\t') {
				return { XmlTokenType::Whitespace,
						 startpos,
						 this->readUntilFirstNotOf(" \t") };
			}
			else if (currentchar == '\r' || currentchar == '\n') {
				return { XmlTokenType::LineBreak,
							startpos,
							this->readUntilFirstNotOf("\r\n") };
			}
			else if (currentchar == '/') {
				if (cursor[1] == '>') {
					this->currcontext.inOpeningTag = false;
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

	return { XmlTokenType::Undefined, startpos, this->readChars(1) };
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
	return this->readUntilFirstOf(" \t\r\n=\"'<>");
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
	if (offset > 0) offset = this->readChars(offset);
	size_t len = strlen(characters);
	while (this->currpos < this->srcLength) {
		cursor = (this->srcText + this->currpos)[0];
		if (!contains(characters, cursor)) {
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
		res += strlen(delimiter);
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
		case XmlTokenType::Undefined:
		default: return "UNDEFINED";
	}
}