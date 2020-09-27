#include "XmlFormater.h"

XmlFormater::XmlFormater(XmlParser* parser) {
	this->parser = parser;

	this->reset();
}

void XmlFormater::reset() {
	prettyPrintParams = {
		"  ",	// indentation chars
		"\r\n",	// end-of-line
		255,	// max indentation level
		false,  // allow whitespace trim
		true    // allow-close tags
	};

	this->indentLevel = 0;
	this->out.clear();
}

void XmlFormater::destroy() {
	this->parser = NULL;
}

std::string XmlFormater::prettyPrint() {
	this->reset();
	this->parser->reset();

	XmlToken token;

	while ((token = this->parser->parseNext()).type != XmlTokenType::EndOfFile) {
		/*
		std::string tmp = this->parser->getTokenName();
		this->out.write(tmp.c_str(), tmp.length());
		*/

		switch (token.type) {
			case XmlTokenType::TagOpening: {	// <ns:sample
				this->writeEOL();
				this->writeIndentation();

				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::TagClosing: {	// </ns:sample
				this->updateIndentLevel(-1);
				this->writeEOL();
				this->writeIndentation();
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
			case XmlTokenType::AttrValue: {
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::Text: {
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::Whitespace: {
				//this->out.write(" ", 1);
				break;
			}
			case XmlTokenType::Instruction: {
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::Comment: {
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::CDATA: {
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::LineBreak: {
				this->writeEOL();
				break;
			}
			case XmlTokenType::CharEQ: {
				this->out.write("=", 1);
				break;
			}
			case XmlTokenType::EndOfFile: {
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::Unknown: {
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::None: {
				this->out.write(token.chars, token.size);
				break;
			}
			default: {
				this->out.write(token.chars, token.size);
				break;
			}
		}
	}

	return this->out.str();
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