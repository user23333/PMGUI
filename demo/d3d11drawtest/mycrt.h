/*
Author: CSlime
Github: https://github.com/cs1ime
*/

#pragma once

#ifndef __MYCRT__INCLUDED__
#define  __MYCRT__INCLUDED__
#include "nthelperx64.h"


#define myDbgPrint(s,a,b,c)\
//{\
CHAR str[300]; \
sprintf(str,(LPCSTR)s, a, b, c); \
OutputDebugStringA(str);\
}
#define myDbgPrint1(s,a)\
//{\
CHAR str[300]; \
sprintf(str,(LPCSTR)s, a); \
OutputDebugStringA(str);\
}
typedef int(_cdecl *_CRT_SPRINTF)(LPSTR buf, LPCSTR fstr, ...);
namespace crt {
	

	bool init(bool manualmap);
	void *malloc(size_t size);
	void free(void *address);
	int sprintf(char* buf, const char* fmt, ...);
	void exit();

}

template <typename T>
class myvector {
private:
	T *p_buffer;
	size_t buffersize;
	size_t currentindex;
public:

	void init() {
		this->p_buffer = 0;
		this->buffersize = 0;
		this->currentindex = 0;
	}
	void reserve(size_t size) {
		this->buffersize = size / sizeof(T);
		if (this->p_buffer) {
			crt::free(this->p_buffer);
		}
		this->p_buffer = (T *)crt::malloc(size);
		this->currentindex = 0;
	}
	void push_back(T obj) {
		//currentindex++;
		if (this->currentindex >= this->buffersize || this->p_buffer == 0) {
			void *oldbuffer = this->p_buffer;
			size_t oldsize = this->buffersize;
			if (this->buffersize < 16)
				this->buffersize = 16;
			this->buffersize *= 2;
			this->p_buffer = (T *)crt::malloc(this->buffersize * sizeof(T));
			if (oldbuffer) {
				memcpy(this->p_buffer, oldbuffer, oldsize * sizeof(T));
				crt::free(oldbuffer);
			}
		}
		this->p_buffer[this->currentindex] = obj;
		this->currentindex++;
	}
	void clear() {
		this->currentindex = 0;
	}
	T* buffer() {
		return this->p_buffer;
	}
	size_t size() {
		return this->currentindex;
	}
	void free() {
		crt::free(this->p_buffer);
		this->p_buffer = 0;
		this->buffersize = 0;
		this->currentindex = 0;
	}

	myvector() {
		this->init();
	}
	~myvector() {
		this->free();
	}
};

class myheap {
private:
	void *p_buffer;
	size_t buffersize;
public:
	void init() {
		this->p_buffer = 0;
		this->buffersize = 0;
	}
	void resize(size_t size) {
		if (size == 0)return;
		if (this->buffersize < size || this->p_buffer == 0) {
			void *oldbuffer = this->p_buffer;
			size_t oldsize = this->buffersize;
			this->buffersize = size * 2;
			this->p_buffer = crt::malloc(this->buffersize);
			if (oldbuffer) {
				memcpy(this->p_buffer, oldbuffer, oldsize);
				crt::free(oldbuffer);
			}
		}

	}
	void free() {
		crt::free(this->p_buffer);
		this->p_buffer = 0;
		this->buffersize = 0;
	}
	void* buffer() {
		return this->p_buffer;
	}
	myheap() {
		this->init();
	}
	~myheap() {
		this->free();
	}
};

#endif // !__MYCRT__INCLUDED__

