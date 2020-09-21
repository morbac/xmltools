
#include "gtest/gtest.h"

#include "ChunkedStream.h"

using namespace SimpleXml;

TEST(ChunkedStreamTests, peekChar) {
	auto txt = "a";
	ChunkedStream s(txt);

	ASSERT_EQ(s.peekChar(), 'a');
}

TEST(ChunkedStreamTests, peekChar_idx1) {
	auto txt = "ab";
	ChunkedStream s(txt);

	ASSERT_EQ(s.peekChar(), 'a');
	ASSERT_EQ(s.peekChar(1), 'b');
}

TEST(ChunkedStreamTests, peekChar_eod) {
	auto txt = "";
	ChunkedStream s(txt);

	ASSERT_EQ(s.peekChar(), 0);
	ASSERT_TRUE(s.eod());
}

std::size_t peekChar_buffers_txt_function(size_t offset, char* target, size_t len, char *src) {
	if (offset > strlen(src))
		return 0;
	target[0] = src[offset];
	return 1;
};

TEST(ChunkedStreamTests, peekChar_buffers) {
	char* peekChar_buffers_txt = "abcd";
	std::function chunker = [peekChar_buffers_txt](size_t a, char* b, size_t c) { return peekChar_buffers_txt_function(a, b, c, peekChar_buffers_txt); };
	ChunkedStream s(1024, chunker);

	for (int i = 0; i < 5; i++) {
		ASSERT_EQ(s.peekChar(i), peekChar_buffers_txt[i]) << "at index " + std::to_string(i);
	}
}