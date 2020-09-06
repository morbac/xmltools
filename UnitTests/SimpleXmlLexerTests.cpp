#include "StdAfx.h"

#include "../SimpleXmlLexer.h"
#include <vector>
#include <string>

using namespace SimpleXml;

namespace {

	inline Lexeme CLexeme(Token token, const char* text) { return Lexeme(token, text, text + strlen(text)); }

	void TestLex(const std::vector<Lexeme>& expectedLexemes, bool registerLinebreaks = true) {
		std::string txt;

		for (const auto &l : expectedLexemes) {
			txt.append(l.value.text(),l.value.size());
		}

		Lexer lexer = Lexer(txt.c_str());
		lexer.registerLinebreaks = registerLinebreaks;
		Lexeme lexeme;
		int i = 0;
		for (lexeme = lexer.get();
			i < expectedLexemes.size();
			lexeme = lexer.get(),
			i++) {

			auto& expected = expectedLexemes[i];

			ASSERT_TRUE(lexeme == expected);
		}

		ASSERT_EQ(i, expectedLexemes.size());
		ASSERT_TRUE(lexer.Done());
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
		auto lex = std::vector{
			CLexeme(Token::TagStart, "<") ,
			CLexeme(Token::Whitespace, " ")
		};

		TestLex(lex);
	}

	TEST(Lexer, Whitespace_tag_doublespace) {
		auto lex = std::vector{
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

	TEST(Lexer,Whitespace_eat) {
		auto lex = Lexer("   \r\n");
		auto l = lex.tryReadWhitespace();

		ASSERT_EQ(l.token, Token::Whitespace);
		ASSERT_TRUE(l.value == "   ");

		lex.eatToken();
		ASSERT_EQ(lex.peekChar(), '\r'); // make sure it is really eaten
	}

	TEST(Lexer, Whitespace_eat_linebreak) {
		char* txt = "   \r\n";
		auto lex = Lexer(txt);
		lex.registerLinebreaks = false;
		auto l = lex.tryReadWhitespace();

		ASSERT_EQ(l.token, Token::Whitespace);
		ASSERT_EQ(l.value, txt);

		lex.eatToken();
		ASSERT_TRUE(lex.Done()); // make sure it is really eaten
	}

	TEST(Lexer, Whitespace_eat_fail) {
		auto lex = Lexer("A");
		auto l = lex.tryReadWhitespace();

		ASSERT_EQ(l.token, Token::Unknown);
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
		auto lex = std::vector{
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
		auto lex = std::vector{
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
		auto lex = Lexer("<");
		lex.get();

		ASSERT_TRUE(lex.inTag());
	}

	TEST(Lexer, InTag_AfterATTR) {
		auto lex = Lexer("<!ATTR");
		lex.get();

		ASSERT_TRUE(lex.inTag());
	}

	TEST(Lexer, InTag_AfterPI) {
		auto lex = Lexer("<?");
		lex.get();

		ASSERT_TRUE(lex.inTag());
	}

	TEST(Lexer, InTag_AfterTagCloseStart) {
		auto lex = Lexer("</ATTR");
		lex.get();

		ASSERT_TRUE(lex.inTag());
	}

	TEST(Lexer, InTag_false_AfterTagEnd) {
		auto lex = Lexer("<>");
		lex.get();
		lex.get();

		ASSERT_FALSE(lex.inTag());
	}

	TEST(Lexer, InTag_false_AfterSelfClosingTagEnd) {
		auto lex = Lexer("<ATTR/>");
		lex.get();
		lex.get();
		lex.get();

		ASSERT_FALSE(lex.inTag());
	}

	// ReadUntil

	TEST(Lexer, readUntil) {
		const char* txt = "abcd";
		auto lex = Lexer(txt);

		auto l = lex.readUntil("d", true);
		ASSERT_TRUE(l == Token::Unknown);
		ASSERT_TRUE(l == txt);
	}

	TEST(Lexer, readUntilExcludeEnd) {
		const char* txt = "abcd";
		auto lex = Lexer(txt);

		auto l = lex.readUntil("d", false);
		ASSERT_TRUE(l == Token::Unknown);
		ASSERT_TRUE(l == "abc");
	}

	TEST(Lexer, readUntil_EndOfFile) {
		const char* txt = "<ATTR/>";
		auto lex = Lexer(txt);

		auto l = lex.readUntil("g", true);
		ASSERT_TRUE(l == Token::Unknown);
		ASSERT_TRUE(l == txt);
	}

	TEST(Lexer, readChar) {
		const char* txt = "<ATTR/>";
		auto lex = Lexer(txt);

		auto c = lex.readChar();
		ASSERT_EQ(c, '<');
		
		c = lex.readChar();
		ASSERT_EQ(c, 'A');
	}

	TEST(Lexer, peekChar) {
		const char* txt = "<ATTR/>";
		auto lex = Lexer(txt);

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
}