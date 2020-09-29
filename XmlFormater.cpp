#include "XmlFormater.h"

XmlFormater::XmlFormater(const char* data, size_t length) {
	this->parser = new XmlParser(data, length);

	this->prettyPrintParams.indentChars = "  ";
	this->prettyPrintParams.eolChars = "\r\n";
	this->prettyPrintParams.maxIndentLevel = 255;
	this->prettyPrintParams.trimWhitespaceAroundText = false;
	this->prettyPrintParams.autoCloseTags = false;
	this->prettyPrintParams.indentAttributes = false;
	this->prettyPrintParams.indentOnly = false;

	this->reset();
}

XmlFormater::XmlFormater(const char* data, size_t length, PrettyPrintParamsType params) {
	this->parser = new XmlParser(data, length);

	this->prettyPrintParams = params;

	this->reset();
}

void XmlFormater::reset() {
	this->indentLevel = 0;
	this->out.clear();
}

XmlFormater::~XmlFormater() {
	this->reset();

	if (this->parser != NULL) {
		delete this->parser;
	}
}

std::string XmlFormater::debugTokens() {
	this->reset();
	this->parser->reset();

	XmlToken token;
	std::stringstream out;

	while ((token = this->parser->parseNext()).type != XmlTokenType::EndOfFile) {
		std::string tmp = this->parser->getTokenName();

		out.write("/", 1);
		out.write(tmp.c_str(), tmp.length());
	}

	return out.str().erase(0, 1);
}

std::stringstream* XmlFormater::prettyPrint() {
	this->reset();
	this->parser->reset();

	XmlToken token = { XmlTokenType::Undefined }, prevtoken, nexttoken;
	bool applyAutoclose = false;

	while ((token = this->parser->parseNext()).type != XmlTokenType::EndOfFile) {
		prevtoken = this->parser->getPrevToken();

		switch (token.type) {
			case XmlTokenType::TagOpening: {	// <ns:sample
				if (prevtoken.type != XmlTokenType::Text &&
					prevtoken.type != XmlTokenType::Undefined) {
					this->writeEOL();
					this->writeIndentation();
				}

				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::TagOpeningEnd: {
				nexttoken = this->parser->getNextToken();
				if (this->prettyPrintParams.autoCloseTags &&
					nexttoken.type == XmlTokenType::TagClosing) {
					this->out.write("/>", 2);
					applyAutoclose = true;
				}
				else {
					this->out.write(">", 1);
					this->updateIndentLevel(1);
					applyAutoclose = false;
				}
				break;
			}
			case XmlTokenType::TagClosing: {	// </ns:sample
				if (!applyAutoclose) {
					if (prevtoken.type != XmlTokenType::Text &&
						prevtoken.type != XmlTokenType::Undefined &&
						prevtoken.type != XmlTokenType::TagOpeningEnd) {
						this->writeEOL();
						this->writeIndentation();
					}
					this->updateIndentLevel(-1);
					this->out.write(token.chars, token.size);
				}
				break;
			}
			case XmlTokenType::TagClosingEnd: {
				if (!applyAutoclose) {
					this->out.write(">", 1);
				}
				applyAutoclose = false;
				break;
			}
			case XmlTokenType::TagSelfClosingEnd: {
				this->out.write("/>", 2);
				applyAutoclose = false;
				break;
			}
			case XmlTokenType::AttrName: {
				this->out.write(" ", 1);
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::Text: {
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::LineBreak: {
				if (this->prettyPrintParams.indentOnly) {
					this->out.write(token.chars, token.size);
				}
				break;
			}
			case XmlTokenType::Comment: {
				if (prevtoken.type != XmlTokenType::Text &&
					prevtoken.type != XmlTokenType::Undefined) {
					this->writeEOL();
					this->writeIndentation();
				}
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::CDATA: {
				if (prevtoken.type != XmlTokenType::Text &&
					prevtoken.type != XmlTokenType::Undefined) {
					this->writeEOL();
					this->writeIndentation();
				}
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::Whitespace: {
				// ignore all whitespace
				// note: whitespaces present in text are not
				//       tokenized as whitespaces, but as text
				break;
			}
			case XmlTokenType::AttrValue:
			case XmlTokenType::Instruction:
			case XmlTokenType::Equal: 
			case XmlTokenType::EndOfFile:
			case XmlTokenType::Undefined:
			default: {
				this->out.write(token.chars, token.size);
				break;
			}
		}
		
		prevtoken = token;
	}

	return &(this->out);
}


void XmlFormater::writeEOL() {
	this->out.write(this->prettyPrintParams.eolChars.c_str(),
		            this->prettyPrintParams.eolChars.length());
}

void XmlFormater::writeIndentation() {
	for (size_t i = 0; i < this->indentLevel; ++i) {
		this->out.write(this->prettyPrintParams.indentChars.c_str(),
			            this->prettyPrintParams.indentChars.length());
	}
}

void XmlFormater::updateIndentLevel(int change) {
	if (change > 0) {
		if (++this->indentLevel > this->prettyPrintParams.maxIndentLevel) {
			this->indentLevel = this->prettyPrintParams.maxIndentLevel;
		}
	}
	else if (change < 0) {
		if (this->indentLevel > 0) {
			--this->indentLevel;
		}
		else {
			this->indentLevel = 0;
		}
	}
}