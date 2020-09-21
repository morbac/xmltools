#ifndef CHUNKEDSTREAM_HEADER_FILE_H
#define CHUNKEDSTREAM_HEADER_FILE_H

#include <list>
#include <stack>
#include <vector>
#include <functional>


namespace SimpleXml {
	
	class ChunkedStream {
	public:
		struct buf {
			const char* data = 0;
			size_t size = 0;
		};
	private:
		size_t bufferSize=1024;

		const char* __curpos;
		const char* __endpos;

		buf __curBuf = { 0,0 };
		char safetyChar = 0;

		std::stack<buf> bufavailable;
		std::vector<buf> bufUsed;

		size_t source_offset = 0;
		size_t read_offset = 0;
		bool sourceEmpty = false;

		void setCurBuff(buf b) {
			__curBuf = b;
			__curpos = b.data;
			__endpos = b.data + b.size;
		}

		bool readNextChunk();

		buf staticSource;

		std::function<size_t(size_t, char*, size_t)> chunkProviderCopy = NULL;

		char peekCharInternal(size_t idx = 0);
		size_t readInternal(char* target, size_t len);

		void moveActiveToAvailable() {
			if (__curBuf.data != staticSource.data) {
				bufavailable.push(__curBuf);
			}
		}

	public:

		ChunkedStream(size_t chunksize, std::function<size_t(size_t, char*, size_t)>&readChunk) { bufferSize = chunksize+1; chunkProviderCopy = readChunk; reset(); }
		ChunkedStream(const char* data) : ChunkedStream(data, strlen(data)) {}
		ChunkedStream(const char* data, size_t len) {
			staticSource.data = data;
			staticSource.size = len;
			reset();
		}
		ChunkedStream() {
			reset();
		}

		~ChunkedStream();

		void reset();
		bool eod() {
			if (__curpos < __endpos || // still have some __firstbuf left
				!bufUsed.empty() || 
				!sourceEmpty
				)
				return false;
			return true;
		}
		// 0 based peeking. 0 = active buffer
		buf peekBuf(size_t idx);
		bool peekMatch(const char* txt) { return peekMatch(txt, strlen(txt)); }
		bool peekMatch(const char* txt, size_t len);

		inline char peekChar() {
			if (__curpos < __endpos)
				return *__curpos;

			return peekCharInternal(0);
		}

		inline char peekChar(size_t idx) {
			if (__curpos+idx < __endpos)
				return __curpos[idx];

			return peekCharInternal(idx);
		}
		
		inline const char* begin() { return __curpos; }
		inline const char* end() { return __endpos; }
		inline size_t available() { return __endpos - __curpos; }

		inline size_t read(char* target, size_t len) {
			if (len < available()) {
				if (target != NULL)
					memcpy(target,__curpos, len);
				__curpos += len;
				read_offset += len;
				return len;
			}
			return readInternal(target, len);
		}
		size_t skip(size_t size);

		size_t offset() { return read_offset; }
	};
}

#endif