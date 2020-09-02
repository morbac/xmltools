#include "StdAfx.h"

#include "../SimpleXmlLexer.h"
#include <vector>

using namespace SimpleXml;

namespace {

	void TestLex(std::vector<Lexeme>& expectedLexemes, bool registerLinebreaks = true) {
		std::string txt;

		for (auto l : expectedLexemes) {
			txt.append(l.text);
		}

		Lexer lexer = Lexer(txt.c_str());
		lexer.RegisterLinebreaks = registerLinebreaks;
		Lexeme lexeme;
		int i = 0;
		for (lexeme = lexer.get();

			lexeme.token != Token::InputEnd &&
			i < expectedLexemes.size();

			lexeme = lexer.get(),
			i++) {

			auto& expected = expectedLexemes[i];


			ASSERT_EQ(lexeme.token, expected.token);
			ASSERT_EQ(lexeme.size(), expected.size());
			ASSERT_TRUE(0 == strncmp(lexeme.text, expected.text, lexeme.size()));
		}

		ASSERT_EQ(i, expectedLexemes.size());
		ASSERT_EQ(lexeme.token, Token::InputEnd);
	}

	void TestSingleLex(Lexeme& l) {
		auto lex = std::vector<Lexeme>{ l };
		TestLex(lex);
	}

	TEST(Lexer, CDSpec) {
		TestSingleLex(Lexeme(Token::CDSect, "<![CDATA[ some < > text ]]>"));
	}

	TEST(Lexer, TagStartName) {
		auto lex = std::vector{
			Lexeme(Token::TagStart, "<") ,
			Lexeme(Token::Name, "TEXT")
		};

		TestLex(lex);
	}

	TEST(Lexer, TagStartNtoken) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart, "<"),
			Lexeme(Token::Nmtoken, "0TEXT")
		};

		TestLex(lex);
	}

	TEST(Lexer, TEXTOnly) {
		TestSingleLex(Lexeme(Token::Text, "This is the end"));
	}

	TEST(Lexer, TEXTWhitespaceAtStart) {
		TestSingleLex(Lexeme(Token::Text, "    This is the end"));
	}

	TEST(Lexer, TEXTWhitespaceAtEnd) {
		TestSingleLex(Lexeme(Token::Text, "This is the end    "));
	}

	TEST(Lexer, TEXTRegisterLinebreaks) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::Whitespace, "  "),
			Lexeme(Token::Linebreak, "\r\n")
		};

		TestLex(lex);
	}

	TEST(Lexer, TEXTRegisterLinebreaksInTag) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart, "<"),
			Lexeme(Token::Whitespace, "  "),
			Lexeme(Token::Linebreak, "\r\n")
		};

		TestLex(lex);
	}

	TEST(Lexer, TEXTDontRegisterLinebreaksInTag) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart, "<"),
			Lexeme(Token::Whitespace, "  \r\n")
		};

		TestLex(lex,false);
	}

	TEST(Lexer, TEXTDontRegisterLinebreaks) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::Whitespace, "  \r\n  ")
		};

		TestLex(lex,false);
	}

	TEST(Lexer, TagStart) {
		TestSingleLex(Lexeme(Token::TagStart, "<"));
	}

	TEST(Lexer, Comment) {
		TestSingleLex(Lexeme(Token::Comment, "<!-- Comment <> -->"));
	}

	TEST(Lexer, Eq) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart,"<"),
			Lexeme(Token::Eq, "="),
		};

		TestLex(lex);
	}

	TEST(Lexer, TagEnd) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart,"<"),
			Lexeme(Token::TagEnd, ">"),
		};

		TestLex(lex);
	}

	TEST(Lexer, ClosingTagEnd) {
		TestSingleLex(Lexeme(Token::ClosingTag, "</"));
	}

	TEST(Lexer, SelfClosingTagEnd) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart,"<"),
			Lexeme(Token::Name,"tagname"),
			Lexeme(Token::SelfClosingTagEnd, "/>"),
		};

		TestLex(lex);
	}

	TEST(Lexer, SystemLiteralSingleQuote) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart,"<"),
			Lexeme(Token::SystemLiteral,"'some'"),
		};

		TestLex(lex);
	}

	TEST(Lexer, SystemLiteralDoubleQuote) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart,"<"),
			Lexeme(Token::SystemLiteral,"\"some\""),
			Lexeme(Token::TagEnd,">"),
		};

		TestLex(lex);
	}

	TEST(Lexer, SystemLiteralUnrecommendedChars) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart,"<"),
			Lexeme(Token::SystemLiteral,"\"<>&\""),
			Lexeme(Token::TagEnd,">"),
		};

		TestLex(lex);
	}


	TEST(Lexer, CheckInTagAfterTagStart) {
		auto lex = Lexer("<");
		lex.get();
		
		ASSERT_TRUE(lex.InTag());
	}

	TEST(Lexer, CheckInTagAfterATTR) {
		auto lex = Lexer("<!ATTR");
		lex.get();

		ASSERT_TRUE(lex.InTag());
	}

	TEST(Lexer, CheckInTagAfterPI) {
		auto lex = Lexer("<?");
		lex.get();

		ASSERT_TRUE(lex.InTag());
	}

	TEST(Lexer, CheckInTagAfterTagCloseStart) {
		auto lex = Lexer("</ATTR");
		lex.get();

		ASSERT_TRUE(lex.InTag());
	}

	TEST(Lexer, CheckNotInTagAfterTagEnd) {
		auto lex = Lexer("<>");
		lex.get();
		lex.get();

		ASSERT_FALSE(lex.InTag());
	}

	TEST(Lexer, CheckNotInTagAfterSelfClosingTagEnd) {
		auto lex = Lexer("<ATTR/>");
		lex.get();
		lex.get();
		lex.get();

		ASSERT_FALSE(lex.InTag());
	}
}