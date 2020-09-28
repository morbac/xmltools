#include "XmlFormater.h"

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

std::stringstream* XmlFormater::prettyPrint() {
	this->reset();
	this->parser->reset();

	XmlToken token = { XmlTokenType::Undefined };
	XmlToken prevtoken = token;

	while ((token = this->parser->parseNext()).type != XmlTokenType::EndOfFile) {
		switch (token.type) {
			case XmlTokenType::TagOpening: {	// <ns:sample
				if (prevtoken.type != XmlTokenType::Text) {
					this->writeEOL();
					this->writeIndentation();
				}

				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::TagClosing: {	// </ns:sample
				this->updateIndentLevel(-1);
				if (prevtoken.type != XmlTokenType::Text) {
					this->writeEOL();
					this->writeIndentation();
				}
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::TagOpeningEnd: {
				this->out.write(">", 1);
				this->updateIndentLevel(1);
				break;
			}
			case XmlTokenType::TagClosingEnd: {
				this->out.write(">", 1);
				break;
			}
			case XmlTokenType::TagSelfClosingEnd: {
				this->out.write("/>", 2);
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
				if (prevtoken.type != XmlTokenType::Text) {
					this->writeEOL();
					this->writeIndentation();
				}
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::CDATA: {
				if (prevtoken.type != XmlTokenType::Text) {
					this->writeEOL();
					this->writeIndentation();
				}
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::Whitespace: {
				// ignore all whitespace
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