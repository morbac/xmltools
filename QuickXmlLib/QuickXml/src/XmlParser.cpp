#include "XmlParser.h"

namespace QuickXml {
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
		this->currcontext = { false, false, 0 };
		this->hasAttrName = false;
		this->currpos = 0;

		this->prevtoken = { XmlTokenType::Undefined, NULL, 0, 0 };
		this->currtoken = { XmlTokenType::Undefined, NULL, 0, 0 };
		this->nexttoken = { XmlTokenType::Undefined, NULL, 0, 0 };
	}

	XmlToken XmlParser::getNextStructureToken() {
		// @fixme Should we consider whitespace and linebreaks has structure token in opening tag ?
		if (!(this->nexttoken.type & (XmlTokenType::Whitespace | XmlTokenType::LineBreak | XmlTokenType::Text))) {
			return this->nexttoken;
		}
		else {
			// let's search in the buffered list
			for (std::list<XmlToken>::iterator it = this->buffer.begin(); it != this->buffer.end(); ++it) {
				if (!((*it).type & (XmlTokenType::Whitespace |XmlTokenType::LineBreak | XmlTokenType::Text))) {
					return (*it);
				}
			}

			// can't find a structure token in the buffered list, let's fetch next tokens
			XmlToken res;
			do {
				res = this->fetchToken();
				this->buffer.push_back(res);

				if (!(res.type & (XmlTokenType::Whitespace | XmlTokenType::LineBreak | XmlTokenType::Text))) {
					return res;
				}
			} while (res.type != XmlTokenType::EndOfFile);

			return { XmlTokenType::Undefined, NULL, 0, this->currpos };
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

	XmlToken XmlParser::parseUntil(XmlTokensType type) {
		do {
			this->parseNext();
		} while (!(this->currtoken.type & type));

		return this->currtoken;
	}

	XmlToken XmlParser::fetchToken() {
		char currentchar;

		const char* cursor = this->srcText + this->currpos;
		const char* startpos = this->srcText + this->currpos;
		size_t currpos_bak;

		if (this->currpos >= this->srcLength) {
			return { XmlTokenType::EndOfFile,
					 this->srcLength,
				     this->srcText + this->srcLength,
				     0 };
		}

		while (this->currpos < this->srcLength) {
			currentchar = cursor[0];
			currpos_bak = this->currpos;
			if (currentchar == '<') {
				// @todo : prevent reading outside chars buffer
				if (cursor[1] == '?') {
					// <?xml ...?>
					// let's leave it untouched
					this->currcontext.inOpeningTag = false;
					this->currcontext.inClosingTag = false;
					return { XmlTokenType::Instruction,
							 this->currpos,
							 startpos,
						     this->readUntil("?>", 0, true) };
				}
				else if (cursor[1] == '%') {
					// not really xml, but for jsp compatibility
					// let's leave it untouched
					this->currcontext.inOpeningTag = false;
					this->currcontext.inClosingTag = false;
					return { XmlTokenType::Instruction,
							 this->currpos,
							 startpos,
							 this->readUntil("%>", 0, true) };
				}
				else if (cursor[1] == '!' && cursor[2] == '-' && cursor[3] == '-') {
					// <!--
					// let's leave it untouched
					this->currcontext.inOpeningTag = false;
					this->currcontext.inClosingTag = false;
					return { XmlTokenType::Comment,
							 this->currpos,
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
							 this->currpos,
							 startpos,
							 this->readUntil("]]>", 0, true) };
				}
				else if (cursor[1] == '!') {
					// <!  for instance "<![INCLUDE or <!DOCTYPE
					// some other declaration
					this->currcontext.inOpeningTag = false;
					this->currcontext.inClosingTag = false;
					this->currcontext.declarationObjects++;
					XmlToken token = { XmlTokenType::Declaration,
									   this->currpos,
					                   startpos,
									   this->readUntilFirstOf("[>", (cursor[2] == '[' ? 3 : 2), false) };
					cursor = this->srcText + this->currpos;
					if (cursor[0] == '[') {
						this->readChars(1);
						token.size++;
					}
					return token;
				}
				else if (cursor[1] == '/') {
					// </ns:sample
					this->currcontext.inOpeningTag = false;
					this->currcontext.inClosingTag = true;
					return { XmlTokenType::TagClosing,
							 this->currpos,
							 startpos,
							 this->readUntilFirstOf("> \r\n") };
				}
				else {
					// parsing tag name like "<sample" or "<ns:sample"
					this->currcontext.inOpeningTag = true;
					this->currcontext.inClosingTag = false;
					return { XmlTokenType::TagOpening,
							 this->currpos,
							 startpos,
							 this->readUntilFirstOf(" />\t\r\n") };
				}
				break;
			}
			else if (this->currcontext.declarationObjects > 0) {
				if (currentchar == ']' && cursor[1] == '>') {
					if (this->currcontext.declarationObjects > 0) {
						this->currcontext.declarationObjects--;
					}
					return { XmlTokenType::DeclarationEnd,
							 this->currpos,
							 startpos,
							 this->readChars(2) };
				}
				else if (currentchar == '>') {
					if (this->currcontext.declarationObjects > 0) {
						this->currcontext.declarationObjects--;
					}
					return { XmlTokenType::DeclarationEnd,
							 this->currpos,
							 startpos,
							 this->readChars(1) };
				}
				else if (currentchar == ' ' || currentchar == '\t') {
					return { XmlTokenType::Whitespace,
							 this->currpos,
							 startpos,
							 this->readUntilFirstNotOf(" \t") };
				}
				else if (currentchar == '\r' || currentchar == '\n') {
					return { XmlTokenType::LineBreak,
							 this->currpos,
							 startpos,
							 this->readUntilFirstNotOf("\r\n") };
				}
				else {
					return { XmlTokenType::Undefined,
							 this->currpos,
							 startpos,
							 this->readChars(1) };
				}
			}
			else if (this->currcontext.inClosingTag) {
				this->hasAttrName = false;
				// parsing closing tag content
				if (currentchar == '>') {
					this->currcontext.inClosingTag = false;
					return { XmlTokenType::TagClosingEnd,
							 this->currpos,
							 startpos,
							 this->readChars(1) };
				}
				else if (currentchar == ' ' || currentchar == '\t') {
					return { XmlTokenType::Whitespace,
							 this->currpos,
							 startpos,
							 this->readUntilFirstNotOf(" \t") };
				}
				else if (currentchar == '\r' || currentchar == '\n') {
					return { XmlTokenType::LineBreak,
							 this->currpos,
							 startpos,
							 this->readUntilFirstNotOf("\r\n") };
				}
				else {
					return { XmlTokenType::Undefined,
							 this->currpos,
							 startpos,
							 this->readChars(1) };
				}
			}
			else if (this->currcontext.inOpeningTag) {
				// parse opening tag content
				if (currentchar == '>') {
					this->hasAttrName = false;
					this->currcontext.inOpeningTag = false;
					return { XmlTokenType::TagOpeningEnd,
							 this->currpos,
							 startpos,
							 this->readChars(1) };
				}
				else if (currentchar == ' ' || currentchar == '\t') {
					return { XmlTokenType::Whitespace,
							 this->currpos,
							 startpos,
							 this->readUntilFirstNotOf(" \t") };
				}
				else if (currentchar == '\r' || currentchar == '\n') {
					return { XmlTokenType::LineBreak,
							 this->currpos,
							 startpos,
							 this->readUntilFirstNotOf("\r\n") };
				}
				else if (currentchar == '/') {
					if (cursor[1] == '>') {
						this->hasAttrName = false;
						this->currcontext.inOpeningTag = false;
						return { XmlTokenType::TagSelfClosingEnd,
								 this->currpos,
								 startpos,
								 this->readChars(2) };
					}
					else {
						return { XmlTokenType::Undefined,
								 this->currpos,
								 startpos,
								 this->readChars(1) };
					}
				}
				else if (currentchar == '=') {
					this->expectAttrValue = true;
					return { XmlTokenType::Equal,
							 this->currpos,
							 startpos,
							 this->readChars(1) };
				}
				else if (this->hasAttrName) {
					this->hasAttrName = false;
					if (this->expectAttrValue || currentchar == '"' || currentchar == '\'') {
						// standard value, delimited with " or '
						this->expectAttrValue = false;
						if (currentchar == '"' || currentchar == '\'') {
							// normal case, let's skip the quoted/apostrophed attribute value
							char valDelimiter[2] = { currentchar, '\0' };
							return { XmlTokenType::AttrValue,
									 this->currpos,
								     startpos,
									 this->readUntilFirstOf(valDelimiter, 1, true) }; // skip actual delimiter + parse content
						}
						else {
							// we have some unexpected chars between the = and the attribute value
							// let's read next word of string
							return { XmlTokenType::AttrValue,
									 this->currpos,
									 startpos,
									 this->readNextWord(true) };
						}
					}
					else {
						// attribute with no value
						this->hasAttrName = true;
						return { XmlTokenType::AttrName,
								 this->currpos,
								 startpos,
								 this->readUntilFirstOf("= /\t\r\n") };
					}
				}
				else {
					this->hasAttrName = true;
					return { XmlTokenType::AttrName,
							 this->currpos,
							 startpos,
							 this->readUntilFirstOf("= /\t\r\n") };
				}
			}
			else {
				// parsing text
				return { XmlTokenType::Text,
						 this->currpos,
						 startpos,
						 this->readUntilFirstOf("<") };
			}
		}

		return { XmlTokenType::Undefined,
			     this->currpos,
			     startpos,
			     this->readChars(1) };
	}

	size_t XmlParser::readChars(size_t nchars) {
		if (this->currpos + nchars > this->srcLength) {
			nchars = this->srcLength - this->currpos;
		}
		this->currpos += nchars;
		return nchars;
	}

	size_t XmlParser::readNextWord(bool skipQuotedStrings) {
		if (skipQuotedStrings) {
			const char* cursor = this->srcText + this->currpos;
			size_t num = 0;
			size_t n = this->readChars(1);
			while (n > 0) {
				num += n;
				if (cursor[num] == ' ' || cursor[num] == '\t' || cursor[num] == '\r' || cursor[num] == '\n') {
					break;
				}
				else if (cursor[num] == '"') {
					num += this->readUntil("\"", 1, true);
					break;
				}
				else if (cursor[num] == '\'') {
					num += this->readUntil("'", 1, true);
					break;
				}
				n = this->readChars(1);
			}

			return num;
		}
		else {
			return this->readUntilFirstOf(" \t\r\n=\"'<>");
		}
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

	size_t XmlParser::readUntil(const char* delimiter, size_t offset, bool goAfter, std::string skipDelimiter) {
		size_t res = 0;
		if (offset > 0) offset = this->readChars(offset);
		const char* cursor = this->srcText + this->currpos;
		if (skipDelimiter.length() > 0) {
			size_t lvl = 0;
			const char* beg;
			const char* end;
			do {
				end = strstr(cursor, delimiter);
				beg = strstr(cursor, skipDelimiter.c_str());
				if (beg != NULL && beg < end) {
					++lvl;
					cursor = beg + 1;
				}
				else if (end != NULL && lvl > 0) {
					--lvl;
					cursor = end + 1;
				}
			} while (lvl > 0);
			if (!end) end = this->srcText + this->srcLength;
			res = end - (this->srcText + this->currpos);
			if (goAfter) {
				res += strlen(delimiter);
			}
			this->currpos += res;
		}
		else {
			const char* end = strstr(cursor, delimiter);
			if (!end) end = this->srcText + this->srcLength;
			res = end - cursor;
			if (goAfter) {
				res += strlen(delimiter);
			}
			this->currpos += res;
		}
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
			case XmlTokenType::Declaration: return "DECLARATION";
			case XmlTokenType::DeclarationEnd: return "DECLARATION_END";
			case XmlTokenType::Comment: return "COMMENT";
			case XmlTokenType::CDATA: return "CDATA";
			case XmlTokenType::LineBreak: return "LINEBREAK";
			case XmlTokenType::Equal: return "EQUAL";
			case XmlTokenType::EndOfFile: return "EOF";
			case XmlTokenType::Undefined:
			default: return "UNDEFINED";
		}
	}
}