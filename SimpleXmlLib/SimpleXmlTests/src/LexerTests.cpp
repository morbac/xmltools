
#include "gtest/gtest.h"

#include <vector>
#include <string>

#include "Lexer.h"


using namespace SimpleXml;

struct Lexeme {
	Token token;
	std::string_view value;

	Lexeme(Token type, const char* text, const char* end) :value(text, end - text) {
		this->token = type;
	}

	Lexeme() {
		token = Token::None;
	}

	bool operator ==(const char* right) const {
		return this->value == right;
	}

	bool operator ==(const Lexeme& right) const {

		if (token != right.token)
			return false;

		return value == right.value;
	}

	bool operator ==(const Token right) const {

		return token == right;
	}

	bool operator !=(const Token right) const {

		return token != right;
	}
};

namespace {

	inline Lexeme CLexeme(Token token, const char* text) { return Lexeme(token, text, text + strlen(text)); }

	void TestLex(const std::vector<Lexeme>& expectedLexemes, bool registerLinebreaks = true) {
		std::string txt;

		for (const auto &l : expectedLexemes) {
			txt.append(l.value.data(),l.value.size());
		}

		ChunkedStream s = ChunkedStream(txt.c_str(), txt.size());
		Lexer lexer = Lexer(s);
		lexer.parms.registerLinebreaks = registerLinebreaks;
		Token token;
		size_t i = 0;
		for (token = lexer.peekToken();
			i < expectedLexemes.size();
			token = lexer.peekToken(),
			i++) {

			auto& expected = expectedLexemes[i];

			ASSERT_EQ(expected.value.size(), lexer.tokenSize()) << "index: "+std::to_string(i)+ ", token: " + std::string(expected.value);
			std::string s;
			lexer.readTokenData(s);
			ASSERT_EQ(s, expected.value);

			//lexer.eatToken(); // not required because of readTokenData
		}

		ASSERT_EQ(i, expectedLexemes.size());
		ASSERT_TRUE(lexer.Done());
	}

	std::string tokenText(Lexer& lex) {
		std::string ret;
		ret.resize(lex.tokenSize());
		lex.readTokenData(ret);
		return ret;
	}

	void TestSingleLex(const Lexeme& l) {
		auto lex = std::vector<Lexeme>{ l };
		TestLex(lex);
	}


	// InputEnd
	TEST(Lexer, InputEnd) {
		TestSingleLex(CLexeme(Token::InputEnd, ""));
	}

	//Whitespace
	TEST(Lexer, Whitespace_tag_space) {
		auto lex = std::vector<Lexeme>{
			CLexeme(Token::TagStart, "<") ,
			CLexeme(Token::Whitespace, " ")
		};

		TestLex(lex);
	}

	TEST(Lexer, Whitespace_tag_doublespace) {
		auto lex = std::vector<Lexeme>{
			CLexeme(Token::TagStart, "<") ,
			CLexeme(Token::Whitespace, " \t")
		};

		TestLex(lex);
	}

	TEST(Lexer, Whitespace_linebreak) {
		auto lex = std::vector<Lexeme>{
			CLexeme(Token::Whitespace, "  \r\n  ")
		};

		TestLex(lex, false);
	}

	TEST(Lexer, Whitespace_tag_linebreak) {
		auto lex = std::vector<Lexeme>{
			CLexeme(Token::TagStart, "<"),
			CLexeme(Token::Whitespace, "  \r\n")
		};

		TestLex(lex, false);
	}
	/*
	TEST(Lexer,Whitespace_eat) {
		const char* xml = "   \r\n";
		ChunkedStream s = ChunkedStream(xml, strlen(xml));
		auto lex = Lexer(s);
		auto l = lex.tryReadWhitespace();

		ASSERT_TRUE(l);
		ASSERT_TRUE(l.value == "   ");

		lex.eatToken();
		ASSERT_EQ(lex.peekChar(), '\r'); // make sure it is really eaten
	}

	TEST(Lexer, Whitespace_eat_linebreak) {
		char* xml = "   \r\n";
		ChunkedStream s = ChunkedStream(xml, strlen(xml));
		auto lex = Lexer(s);
		lex.registerLinebreaks = false;
		auto l = lex.tryReadWhitespace();

		ASSERT_TRUE(l);
		ASSERT_EQ(0, lex.inputStream->begin().value, xml);

		lex.eatToken();
		ASSERT_TRUE(lex.Done()); // make sure it is really eaten
	}
	*/
	TEST(Lexer, Whitespace_eat_fail) {
		char* xml = "A";
		ChunkedStream s = ChunkedStream(xml, strlen(xml));
		auto lex = Lexer(s);
		auto l = lex.tryReadWhitespace();

		ASSERT_FALSE(l);
	}

	TEST(Lexer, Linebreak) {
		auto lex = std::vector<Lexeme>{
			CLexeme(Token::Linebreak, "\r\n")
		};

		TestLex(lex);
	}

	TEST(Lexer, Linebreak_spacebreak) {
		auto lex = std::vector<Lexeme>{
			CLexeme(Token::Whitespace, "  "),
			CLexeme(Token::Linebreak, "\r\n"),
			CLexeme(Token::Whitespace, "  ")
		};

		TestLex(lex);
	}

	TEST(Lexer, Linebreak_tag_spacebreak) {
		auto lex = std::vector<Lexeme>{
			CLexeme(Token::TagStart, "<"),
			CLexeme(Token::Whitespace, "  "),
			CLexeme(Token::Linebreak, "\r\n"),
			CLexeme(Token::Whitespace, "  "),
		};

		TestLex(lex);
	}


	// ProcessingInstruction

	TEST(Lexer, ProcessingInstructionStart) {
		TestSingleLex(CLexeme(Token::ProcessingInstructionStart, "<?"));
	}

	TEST(Lexer, ProcessingInstructionEnd) {
		auto lex = std::vector<Lexeme>{
			CLexeme(Token::ProcessingInstructionStart, "<?") ,
			CLexeme(Token::ProcessingInstructionEnd, "?>")
		};

		TestLex(lex);
	}

	TEST(Lexer, CDSpec) {
		TestSingleLex(CLexeme(Token::CDSect, "<![CDATA[ some < > text ]]>"));
	}

	TEST(Lexer, DeclStart) {
		TestSingleLex(CLexeme(Token::DeclStart, "<!"));
	}

	TEST(Lexer, TagStart_Name) {
		auto lex = std::vector<Lexeme>{
			CLexeme(Token::TagStart, "<") ,
			CLexeme(Token::Name, "TEXT")
		};

		TestLex(lex);
	}

	TEST(Lexer, TagStart_Ntoken) {
		auto lex = std::vector<Lexeme>{
			CLexeme(Token::TagStart, "<"),
			CLexeme(Token::Nmtoken, "0TEXT")
		};

		TestLex(lex);
	}

	// Text

	TEST(Lexer, Text) {
		TestSingleLex(CLexeme(Token::Text, "This is the end"));
	}

	TEST(Lexer, Text_WhitespaceAtStart) {
		TestSingleLex(CLexeme(Token::Text, "    This is the end"));
	}

	TEST(Lexer, Text_WhitespaceAtEnd) {
		TestSingleLex(CLexeme(Token::Text, "This is the end    "));
	}

	// TagStart

	TEST(Lexer, TagStart) {
		TestSingleLex(CLexeme(Token::TagStart, "<"));
	}

	// Comment

	TEST(Lexer, Comment) {
		TestSingleLex(CLexeme(Token::Comment, "<!-- Comment <> -->"));
	}

	// Eq

	TEST(Lexer, Eq) {
		auto lex = std::vector<Lexeme>{
			CLexeme(Token::TagStart,"<"),
			CLexeme(Token::Eq, "="),
		};

		TestLex(lex);
	}

	// TagEnd

	TEST(Lexer, TagEnd) {
		auto lex = std::vector<Lexeme>{
			CLexeme(Token::TagStart,"<"),
			CLexeme(Token::TagEnd, ">"),
		};

		TestLex(lex);
	}

	// ClosingTagEnd

	TEST(Lexer, ClosingTagEnd) {
		TestSingleLex(CLexeme(Token::ClosingTag, "</"));
	}

	// SelfClosingTagEnd

	TEST(Lexer, SelfClosingTagEnd) {
		auto lex = std::vector<Lexeme>{
			CLexeme(Token::TagStart,"<"),
			CLexeme(Token::Name,"tagname"),
			CLexeme(Token::SelfClosingTagEnd, "/>"),
		};

TestLex(lex);
	}

	// SystemLiteral

	TEST(Lexer, SystemLiteral_SingleQuote) {
		auto lex = std::vector<Lexeme>{
			CLexeme(Token::TagStart,"<"),
			CLexeme(Token::SystemLiteral,"'some'"),
		};

		TestLex(lex);
	}

	TEST(Lexer, SystemLiteral_DoubleQuote) {
		auto lex = std::vector<Lexeme>{
			CLexeme(Token::TagStart,"<"),
			CLexeme(Token::SystemLiteral,"\"some\""),
			CLexeme(Token::TagEnd,">"),
		};

		TestLex(lex);
	}

	TEST(Lexer, SystemLiteral_UnrecommendedChars) {
		auto lex = std::vector<Lexeme>{
			CLexeme(Token::TagStart,"<"),
			CLexeme(Token::SystemLiteral,"\"<>&\""),
			CLexeme(Token::TagEnd,">"),
		};

		TestLex(lex);
	}


	// inTag

	TEST(Lexer, InTag_AfterTagStart) {
		ChunkedStream s("<");
		auto lex = Lexer(s);
		lex.peekToken();
		lex.eatToken();

		ASSERT_TRUE(lex.inTag());
	}

	TEST(Lexer, InTag_AfterATTR) {
		ChunkedStream s("<!ATTR");
		auto lex = Lexer(s);
		lex.peekToken();
		lex.eatToken();

		ASSERT_TRUE(lex.inTag());
	}

	TEST(Lexer, InTag_AfterPI) {
		ChunkedStream s("<?");
		auto lex = Lexer(s);
		lex.peekToken();
		lex.eatToken();

		ASSERT_TRUE(lex.inTag());
	}

	TEST(Lexer, InTag_AfterTagCloseStart) {
		ChunkedStream s("</ATTR");
		auto lex = Lexer(s);
		lex.peekToken();
		lex.eatToken();

		ASSERT_TRUE(lex.inTag());
	}

	TEST(Lexer, InTag_false_AfterTagEnd) {
		ChunkedStream s("<>");
		auto lex = Lexer(s);
		lex.peekToken();
		lex.eatToken();
		lex.peekToken();
		lex.eatToken();

		ASSERT_FALSE(lex.inTag());
	}

	TEST(Lexer, InTag_false_AfterSelfClosingTagEnd) {
		ChunkedStream s("<ATTR/>");
		auto lex = Lexer(s);
		lex.peekToken();
		lex.eatToken();
		lex.peekToken();
		lex.eatToken();
		lex.peekToken();
		lex.eatToken();

		ASSERT_FALSE(lex.inTag());
	}

	// ReadUntil

	TEST(Lexer, readUntil) {
		const char* txt = "abcd";
		ChunkedStream s(txt);
		auto lex = Lexer(s);

		auto l = lex.readUntil("d", true);
		ASSERT_TRUE(l);
		ASSERT_EQ(tokenText(lex), txt);
	}

	TEST(Lexer, readUntilExcludeEnd) {
		const char* txt = "abcd";
		ChunkedStream s(txt);
		auto lex = Lexer(s);

		auto l = lex.readUntil("d", false);
		ASSERT_TRUE(l);
		ASSERT_EQ(tokenText(lex), "abc");
	}

	TEST(Lexer, readUntil_EndOfFile) {
		const char* txt = "<ATTR/>";
		ChunkedStream s(txt);
		auto lex = Lexer(s);

		auto l = lex.readUntil("g", true);
		ASSERT_FALSE(l);
		ASSERT_EQ(tokenText(lex), txt);
	}

	TEST(Lexer, readChar) {
		const char* txt = "<ATTR/>";
		ChunkedStream s(txt);
		auto lex = Lexer(s);

		auto c = lex.readChar();
		ASSERT_EQ(c, '<');
		
		c = lex.readChar();
		ASSERT_EQ(c, 'A');
	}

	TEST(Lexer, peekChar) {
		const char* txt = "<ATTR/>";
		ChunkedStream s(txt);
		auto lex = Lexer(s);

		auto c = lex.peekChar();
		ASSERT_EQ(c, '<');

		c = lex.peekChar();
		ASSERT_EQ(c, '<');
	}

	TEST(Lexer, Element_Text) {
		auto lex = std::vector<Lexeme>{
				CLexeme(Token::TagStart,"<"),
				CLexeme(Token::Name,"test"),
				CLexeme(Token::TagEnd,">"),
				CLexeme(Token::Text,"."),
				CLexeme(Token::ClosingTag,"</"),
				CLexeme(Token::Name,"test"),
				CLexeme(Token::TagEnd,">")
		};

		TestLex(lex);
	}

	TEST(Lexer, tryReadName_none) {
		const char* txt = " name";
		ChunkedStream s(txt);
		auto lex = Lexer(s);

		ASSERT_FALSE(lex.tryReadName());
	}

	TEST(Lexer, tryReadName_1char) {
		const char* txt = "w ";
		ChunkedStream s(txt);
		auto lex = Lexer(s);

		ASSERT_TRUE(lex.tryReadName());
		std::string name;
		lex.readTokenData(name);
		ASSERT_EQ(name, "w");
	}

	TEST(Lexer, tryReadName_2char) {
		const char* txt = "w5>";
		ChunkedStream s(txt);
		auto lex = Lexer(s);

		ASSERT_TRUE(lex.tryReadName());
		std::string name;
		lex.readTokenData(name);
		ASSERT_EQ(name, "w5");
	}

	TEST(Lexer, tryReadName_invalidstartchar) {
		const char* txt = "5x";
		ChunkedStream s(txt);
		auto lex = Lexer(s);

		ASSERT_FALSE(lex.tryReadName());
	}

	TEST(Lexer, tryReadNmtoken_none) {
		const char* txt = " name";
		ChunkedStream s(txt);
		auto lex = Lexer(s);

		ASSERT_FALSE(lex.tryReadNmtoken());
	}

	TEST(Lexer, tryReadNmtoken_1char) {
		const char* txt = "w ";
		ChunkedStream s(txt);
		auto lex = Lexer(s);

		ASSERT_TRUE(lex.tryReadNmtoken());
		std::string name;
		lex.readTokenData(name);
		ASSERT_EQ(name, "w");
	}

	TEST(Lexer, tryReadNmtoken_2char) {
		const char* txt = "w5>";
		ChunkedStream s(txt);
		auto lex = Lexer(s);

		ASSERT_TRUE(lex.tryReadNmtoken());
		std::string name;
		lex.readTokenData(name);
		ASSERT_EQ(name, "w5");
	}

	TEST(Lexer, tryReadNmtoken_invalidstartchar_name) {
		const char* txt = "5x";
		ChunkedStream s(txt);
		auto lex = Lexer(s);

		ASSERT_TRUE(lex.tryReadNmtoken());
		std::string name;
		lex.readTokenData(name);
		ASSERT_EQ(name, "5x");
	}
}