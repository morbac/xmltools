#include "ChunkedStream.h"

namespace SimpleXml {

	ChunkedStream::~ChunkedStream() {
		for (auto b : bufUsed) {
			delete[] b.data;
		}

		moveActiveToAvailable();

		while(!bufavailable.empty()) {
			auto b = bufavailable.top();
			bufavailable.pop();
			delete[] b.data;
		}
	}

	void ChunkedStream::reset() {
		for (auto b : bufUsed) {
			bufavailable.push(b);
		}

		if (__curBuf.data != NULL) {
			if (__curBuf.data != staticSource.data) {
				bufavailable.push(__curBuf);
			}
			__curBuf.data = NULL;
			__curBuf.size = 0;
		}

		__curpos = &safetyChar;
		__endpos = &safetyChar;

		source_offset = 0;
		read_offset = 0;
		sourceEmpty = false;

		if (staticSource.data != NULL) {
			__curBuf = staticSource;
			__curpos = __curBuf.data;
			__endpos = __curBuf.data + __curBuf.size;
		}
	}

	bool ChunkedStream::readNextChunk() {
		buf b;
		
		if (chunkProviderCopy) {
			if (bufavailable.empty()) {
				b.data = new char[bufferSize];
			}
			else {
				b = bufavailable.top();
				bufavailable.pop();
			}
			b.size = chunkProviderCopy(source_offset, (char*)b.data, bufferSize - 1);
			if (b.size > 0) {
				source_offset += b.size;
				((char*)b.data)[b.size] = 0;
			}
		}
		else {
			sourceEmpty = true;
			return false;
		}

		if (b.size == 0) {
			delete[] b.data; // EOF
			sourceEmpty = true;
			return false;
		}

		bufUsed.push_back(b);
		return true;
	}

	ChunkedStream::buf ChunkedStream::peekBuf(size_t idx) {
		if (bufUsed.size() > idx) {
			return bufUsed[idx];
		}
		if (bufUsed.size() == idx) {
			if (readNextChunk())
				return bufUsed[idx];
		}
		return buf{ 0,0 };
	}

	bool ChunkedStream::peekMatch(const char* match,size_t len) {
		while (len > 0) {
			auto copysize = available();
			if (!copysize) {
				if (bufUsed.empty()) {
					break;
				}
				auto b = bufUsed.front();
				bufUsed.erase(bufUsed.begin());
				setCurBuff(b);
				copysize = available();
			}
			if (len < copysize)
				copysize = len;

			for (size_t i = 0; i < copysize; i ++) {
				if (__curpos [i]!= match[i])
					return false;
			}
				
			len-=copysize;
			match+=copysize;
		}
		return len == 0;
	}

	char ChunkedStream::peekCharInternal(size_t idx) {
		const char* pos = __curpos + idx;
		if (pos < __endpos)
			return *pos;

		idx -= available();
		
		int bidx =0;
		auto buf = peekBuf(0);
		while (idx >= buf.size && buf.data != NULL) {
			idx -= buf.size;
			bidx++;
			buf = peekBuf(bidx);
		}
		if (buf.data != NULL)
		{
			if (idx < buf.size)
				return buf.data[idx];
		}

		return 0;
	}

	size_t ChunkedStream::readInternal(char* target, size_t len) {
		size_t copied = 0;
		while (len > 0) {
			auto copysize = available();
			if(!copysize) {
				if (bufUsed.empty()) {
					break;
				}
				auto b = bufUsed.front();
				bufUsed.erase(bufUsed.begin());
				moveActiveToAvailable();
				setCurBuff(b);
				copysize = available();
			}
			if (len < copysize)
				copysize = len;

			if (target != NULL) {
				memcpy(target, __curpos, copysize);
				target += copysize;
			}
			__curpos += copysize;
			len -= copysize;
		}
		read_offset += copied;
		return copied;
	}

	size_t ChunkedStream::skip(size_t len) {
		return read(NULL, len);
	}
}