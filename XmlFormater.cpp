#include "XmlFormater.h"

static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n');
	}));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n');
	}).base(), s.end());
}

static inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

XmlFormater::XmlFormater(const char* data, size_t length) {
	this->parser = new XmlParser(data, length);

	XmlFormaterParamsType params;
	params.indentChars = "  ";
	params.eolChars = "\n";
	params.maxIndentLevel = 255;
	params.enforceConformity = true;
	params.autoCloseTags = false;
	params.indentAttributes = false;
	params.indentOnly = false;

	this->init(data, length, params);
}

XmlFormater::XmlFormater(const char* data, size_t length, XmlFormaterParamsType params) {
	this->init(data, length, params);
}

XmlFormater::~XmlFormater() {
	this->reset();
	if (this->parser != NULL) {
		delete this->parser;
	}
}

void XmlFormater::init(const char* data, size_t length, XmlFormaterParamsType params) {
	if (this->parser != NULL) {
		delete this->parser;
	}

	this->parser = new XmlParser(data, length);
	this->params = params;
	this->reset();
}

void XmlFormater::reset() {
	this->indentLevel = 0;
	this->out.clear();
	this->out.str(std::string());	// make the stringstream empty
}

std::string XmlFormater::debugTokens(std::string separator) {
	this->reset();
	this->parser->reset();

	XmlToken token;
	std::stringstream out;

	while ((token = this->parser->parseNext()).type != XmlTokenType::EndOfFile) {
		out << separator << this->parser->getTokenName();
	}

	// return result after having removed the firt '/'
	return out.str().erase(0, separator.length());
}

std::stringstream* XmlFormater::linearize() {
	this->reset();
	this->parser->reset();

	XmlToken token = { XmlTokenType::Undefined }, nexttoken;
	XmlTokenType lastAppliedTokenType = XmlTokenType::Undefined;
	bool applyAutoclose = false;

	while ((token = this->parser->parseNext()).type != XmlTokenType::EndOfFile) {
		switch (token.type) {
			case XmlTokenType::LineBreak: {
				break;
			}
			case XmlTokenType::Whitespace: {
				if (this->parser->getXmlContext().inOpeningTag) {
					lastAppliedTokenType = XmlTokenType::Whitespace;
					this->out << " ";
				}
				break;
			}
			case XmlTokenType::Text: {
				std::string tmp(token.chars, token.size);
				trim(tmp);
				if (this->params.enforceConformity) {
					nexttoken = this->parser->getNextToken();
					if (tmp.length() > 0 ||
						(nexttoken.type != XmlTokenType::TagOpening &&
							(nexttoken.type != XmlTokenType::TagClosing ||
								lastAppliedTokenType == XmlTokenType::TagOpeningEnd))) {
						lastAppliedTokenType = XmlTokenType::Text;
						this->out.write(token.chars, token.size);
					}
				}
				else {
					lastAppliedTokenType = XmlTokenType::Text;
					this->out << tmp;
				}
				break;
			}
			case XmlTokenType::TagOpeningEnd: {
				if (this->params.enforceConformity) {
					nexttoken = this->parser->getNextToken();
				}
				else {
					nexttoken = this->parser->getNextStructureToken();
				}
				if (this->params.autoCloseTags &&
					nexttoken.type == XmlTokenType::TagClosing) {
					lastAppliedTokenType = XmlTokenType::TagSelfClosingEnd;
					this->out << "/>";
					applyAutoclose = true;
				}
				else {
					lastAppliedTokenType = XmlTokenType::TagOpeningEnd;
					this->out << ">";
					applyAutoclose = false;
				}
				break;
			}
			case XmlTokenType::TagClosing: {	// </ns:sample
				if (!applyAutoclose) {
					lastAppliedTokenType = XmlTokenType::TagClosing;
					this->out.write(token.chars, token.size);
				}
				break;
			}
			case XmlTokenType::TagClosingEnd: {
				if (!applyAutoclose) {
					lastAppliedTokenType = XmlTokenType::TagClosingEnd;
					this->out << ">";
				}
				applyAutoclose = false;
				break;
			}
			case XmlTokenType::TagSelfClosingEnd: {
				lastAppliedTokenType = XmlTokenType::TagSelfClosingEnd;
				this->out << "/>";
				applyAutoclose = false;
				break;
			}
			case XmlTokenType::TagOpening:
			case XmlTokenType::AttrName:
			case XmlTokenType::Comment:
			case XmlTokenType::CDATA:
			case XmlTokenType::AttrValue:
			case XmlTokenType::Instruction:
			case XmlTokenType::Equal:
			case XmlTokenType::Undefined:
			default: {
				lastAppliedTokenType = token.type;
				this->out.write(token.chars, token.size);
				break;
			}
		}
	}

	return &(this->out);
}

std::stringstream* XmlFormater::prettyPrint() {
	this->reset();
	this->parser->reset();

	XmlToken token = { XmlTokenType::Undefined }, nexttoken;
	XmlTokenType lastAppliedTokenType = XmlTokenType::Undefined;
	bool applyAutoclose = false;

	while ((token = this->parser->parseNext()).type != XmlTokenType::EndOfFile) {
		switch (token.type) {
			case XmlTokenType::TagOpening: {	// <ns:sample
				if (lastAppliedTokenType != XmlTokenType::Text &&
					lastAppliedTokenType != XmlTokenType::CDATA &&
					lastAppliedTokenType != XmlTokenType::Undefined) {
					this->writeEOL();
					this->writeIndentation();
				}
				lastAppliedTokenType = XmlTokenType::TagOpening;
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::TagOpeningEnd: {
				nexttoken = this->parser->getNextToken();
				if (this->params.autoCloseTags &&
					nexttoken.type == XmlTokenType::TagClosing) {
					lastAppliedTokenType = XmlTokenType::TagSelfClosingEnd;
					this->out << "/>";
					applyAutoclose = true;
				}
				else {
					lastAppliedTokenType = XmlTokenType::TagOpeningEnd;
					this->out << ">";
					this->updateIndentLevel(1);
					applyAutoclose = false;
				}
				break;
			}
			case XmlTokenType::TagClosing: {	// </ns:sample
				if (!applyAutoclose) {
					this->updateIndentLevel(-1);
					if (lastAppliedTokenType != XmlTokenType::Text &&
						lastAppliedTokenType != XmlTokenType::CDATA && 
						lastAppliedTokenType != XmlTokenType::TagOpeningEnd &&
						lastAppliedTokenType != XmlTokenType::Undefined) {
						this->writeEOL();
						this->writeIndentation();
					}
					lastAppliedTokenType = XmlTokenType::TagClosing;
					this->out.write(token.chars, token.size);
				}
				break;
			}
			case XmlTokenType::TagClosingEnd: {
				if (!applyAutoclose) {
					lastAppliedTokenType = XmlTokenType::TagClosingEnd;
					this->out << ">";
				}
				applyAutoclose = false;
				break;
			}
			case XmlTokenType::TagSelfClosingEnd: {
				lastAppliedTokenType = XmlTokenType::TagSelfClosingEnd;
				this->out << "/>";
				applyAutoclose = false;
				break;
			}
			case XmlTokenType::AttrName: {
				this->out << " ";
				lastAppliedTokenType = XmlTokenType::AttrName;
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::Text: {
				// check if text could be ignored
				// @todo implement the !enforceConformity mode
				XmlToken nexttoken = this->parser->getNextToken();
				std::string tmp(token.chars, token.size);
				trim(tmp);
				if (tmp.length() > 0 ||
					( nexttoken.type != XmlTokenType::TagOpening &&
					  ( nexttoken.type != XmlTokenType::TagClosing ||
						lastAppliedTokenType == XmlTokenType::TagOpeningEnd ) )) {
					lastAppliedTokenType = XmlTokenType::Text;
					this->out.write(token.chars, token.size);
				}
				break;
			}
			case XmlTokenType::LineBreak: {
				if (this->params.indentOnly) {
					lastAppliedTokenType = XmlTokenType::LineBreak;
					this->out.write(token.chars, token.size);
				}
				break;
			}
			case XmlTokenType::Comment: {
				if (lastAppliedTokenType != XmlTokenType::Text &&
					lastAppliedTokenType != XmlTokenType::CDATA &&
					lastAppliedTokenType != XmlTokenType::Undefined) {
					this->writeEOL();
					this->writeIndentation();
				}
				lastAppliedTokenType = XmlTokenType::Comment;
				this->out.write(token.chars, token.size);
				break;
			}
			case XmlTokenType::CDATA: {
				lastAppliedTokenType = XmlTokenType::CDATA;
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
				lastAppliedTokenType = token.type;
				this->out.write(token.chars, token.size);
				break;
			}
		}
	}

	return &(this->out);
}


void XmlFormater::writeEOL() {
	this->out << this->params.eolChars;
}

void XmlFormater::writeIndentation() {
	for (size_t i = 0; i < this->indentLevel; ++i) {
		this->out << this->params.indentChars;
	}
}

void XmlFormater::updateIndentLevel(int change) {
	if (change > 0) {
		if (++this->indentLevel > this->params.maxIndentLevel) {
			this->indentLevel = this->params.maxIndentLevel;
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