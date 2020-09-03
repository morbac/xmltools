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
			i < expectedLexemes.size();
			lexeme = lexer.get(),
			i++) {

			auto& expected = expectedLexemes[i];

			ASSERT_TRUE(lexeme == expected);
		}

		ASSERT_EQ(i, expectedLexemes.size());
		ASSERT_TRUE(lexer.Done());
	}

	void TestSingleLex(Lexeme& l) {
		auto lex = std::vector<Lexeme>{ l };
		TestLex(lex);
	}


	// InputEnd
	TEST(Lexer, InputEnd) {
		TestSingleLex(Lexeme(Token::InputEnd, ""));
	}

	//Whitespace
	TEST(Lexer, Whitespace_tag_space) {
		auto lex = std::vector{
			Lexeme(Token::TagStart, "<") ,
			Lexeme(Token::Whitespace, " ")
		};

		TestLex(lex);
	}

	TEST(Lexer, Whitespace_tag_doublespace) {
		auto lex = std::vector{
			Lexeme(Token::TagStart, "<") ,
			Lexeme(Token::Whitespace, " \t")
		};

		TestLex(lex);
	}

	TEST(Lexer, Whitespace_linebreak) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::Whitespace, "  \r\n  ")
		};

		TestLex(lex, false);
	}

	TEST(Lexer, Whitespace_tag_linebreak) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart, "<"),
			Lexeme(Token::Whitespace, "  \r\n")
		};

		TestLex(lex, false);
	}

	TEST(Lexer, Linebreak) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::Linebreak, "\r\n")
		};

		TestLex(lex);
	}

	TEST(Lexer, Linebreak_spacebreak) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::Whitespace, "  "),
			Lexeme(Token::Linebreak, "\r\n"),
			Lexeme(Token::Whitespace, "  ")
		};

		TestLex(lex);
	}

	TEST(Lexer, Linebreak_tag_spacebreak) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart, "<"),
			Lexeme(Token::Whitespace, "  "),
			Lexeme(Token::Linebreak, "\r\n"),
			Lexeme(Token::Whitespace, "  "),
		};

		TestLex(lex);
	}


	// ProcessingInstruction

	TEST(Lexer, ProcessingInstructionStart) {
		TestSingleLex(Lexeme(Token::ProcessingInstructionStart, "<?"));
	}

	TEST(Lexer, ProcessingInstructionEnd) {
		auto lex = std::vector{
			Lexeme(Token::ProcessingInstructionStart, "<?") ,
			Lexeme(Token::ProcessingInstructionEnd, "?>")
		};

		TestLex(lex);
	}

	TEST(Lexer, CDSpec) {
		TestSingleLex(Lexeme(Token::CDSect, "<![CDATA[ some < > text ]]>"));
	}

	TEST(Lexer, TagStart_Name) {
		auto lex = std::vector{
			Lexeme(Token::TagStart, "<") ,
			Lexeme(Token::Name, "TEXT")
		};

		TestLex(lex);
	}

	TEST(Lexer, TagStart_Ntoken) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart, "<"),
			Lexeme(Token::Nmtoken, "0TEXT")
		};

		TestLex(lex);
	}

	// Text

	TEST(Lexer, Text) {
		TestSingleLex(Lexeme(Token::Text, "This is the end"));
	}

	TEST(Lexer, Text_WhitespaceAtStart) {
		TestSingleLex(Lexeme(Token::Text, "    This is the end"));
	}

	TEST(Lexer, Text_WhitespaceAtEnd) {
		TestSingleLex(Lexeme(Token::Text, "This is the end    "));
	}

	// TagStart

	TEST(Lexer, TagStart) {
		TestSingleLex(Lexeme(Token::TagStart, "<"));
	}

	// Comment

	TEST(Lexer, Comment) {
		TestSingleLex(Lexeme(Token::Comment, "<!-- Comment <> -->"));
	}

	// Eq

	TEST(Lexer, Eq) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart,"<"),
			Lexeme(Token::Eq, "="),
		};

		TestLex(lex);
	}

	// TagEnd

	TEST(Lexer, TagEnd) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart,"<"),
			Lexeme(Token::TagEnd, ">"),
		};

		TestLex(lex);
	}

	// ClosingTagEnd

	TEST(Lexer, ClosingTagEnd) {
		TestSingleLex(Lexeme(Token::ClosingTag, "</"));
	}

	// SelfClosingTagEnd

	TEST(Lexer, SelfClosingTagEnd) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart,"<"),
			Lexeme(Token::Name,"tagname"),
			Lexeme(Token::SelfClosingTagEnd, "/>"),
		};

		TestLex(lex);
	}

	// SystemLiteral

	TEST(Lexer, SystemLiteral_SingleQuote) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart,"<"),
			Lexeme(Token::SystemLiteral,"'some'"),
		};

		TestLex(lex);
	}

	TEST(Lexer, SystemLiteral_DoubleQuote) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart,"<"),
			Lexeme(Token::SystemLiteral,"\"some\""),
			Lexeme(Token::TagEnd,">"),
		};

		TestLex(lex);
	}

	TEST(Lexer, SystemLiteral_UnrecommendedChars) {
		auto lex = std::vector<Lexeme>{
			Lexeme(Token::TagStart,"<"),
			Lexeme(Token::SystemLiteral,"\"<>&\""),
			Lexeme(Token::TagEnd,">"),
		};

		TestLex(lex);
	}


	// InTag

	TEST(Lexer, InTag_AfterTagStart) {
		auto lex = Lexer("<");
		lex.get();
		
		ASSERT_TRUE(lex.InTag());
	}

	TEST(Lexer, InTag_AfterATTR) {
		auto lex = Lexer("<!ATTR");
		lex.get();

		ASSERT_TRUE(lex.InTag());
	}

	TEST(Lexer, InTag_AfterPI) {
		auto lex = Lexer("<?");
		lex.get();

		ASSERT_TRUE(lex.InTag());
	}

	TEST(Lexer, InTag_AfterTagCloseStart) {
		auto lex = Lexer("</ATTR");
		lex.get();

		ASSERT_TRUE(lex.InTag());
	}

	TEST(Lexer, InTag_false_AfterTagEnd) {
		auto lex = Lexer("<>");
		lex.get();
		lex.get();

		ASSERT_FALSE(lex.InTag());
	}

	TEST(Lexer, InTag_false_AfterSelfClosingTagEnd) {
		auto lex = Lexer("<ATTR/>");
		lex.get();
		lex.get();
		lex.get();

		ASSERT_FALSE(lex.InTag());
	}

	// ReadUntil

	TEST(Lexer, readUntil) {
		const char* txt = "abcd";
		auto lex = Lexer(txt);

		ASSERT_TRUE(lex.readUntil("d"));
		ASSERT_TRUE(0 == strncmp(lex.TokenText(), txt, strlen(txt)));
	}

	TEST(Lexer, readUntil_EndOfFile) {
		const char* txt = "<ATTR/>";
		auto lex = Lexer(txt);
		
		ASSERT_FALSE(lex.readUntil("g"));
		ASSERT_TRUE(0 == strncmp(lex.TokenText(),txt,strlen(txt)) );
	}
}