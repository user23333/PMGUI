/*
Author: CSlime
Github: https://github.com/cs1ime
*/

#pragma once

#ifndef _PM_SPRINTF_H_
#define _PM_SPRINTF_H_

#include "stdio.h"

#define STB_SPRINTF_DECORATE
#define STB_SPRINTF_IMPLEMENTATION

#ifndef STBSP__ASAN
#define STBSP__ASAN
#endif

#ifdef STB_SPRINTF_STATIC
#define STBSP__PUBLICDEC static
#define STBSP__PUBLICDEF static STBSP__ASAN
#else
#ifdef __cplusplus
#define STBSP__PUBLICDEC extern "C"
#define STBSP__PUBLICDEF extern "C" STBSP__ASAN
#else
#define STBSP__PUBLICDEC extern
#define STBSP__PUBLICDEF STBSP__ASAN
#endif
#endif

#if defined(__has_attribute)
#if __has_attribute(format)
#define STBSP__ATTRIBUTE_FORMAT(fmt,va) __attribute__((format(printf,fmt,va)))
#endif
#endif

#ifndef STBSP__ATTRIBUTE_FORMAT
#define STBSP__ATTRIBUTE_FORMAT(fmt,va)
#endif

#ifdef _MSC_VER
#define STBSP__NOTUSED(v)  (void)(v)
#else
#define STBSP__NOTUSED(v)  (void)sizeof(v)
#endif

#include <stdarg.h> // for va_arg(), va_list()
#include <stddef.h> // size_t, ptrdiff_t

#ifndef STB_SPRINTF_MIN
#define STB_SPRINTF_MIN 512 // how many characters per callback
#endif
typedef char* STBSP_SPRINTFCB(const char* buf, void* user, int len);

#ifndef STB_SPRINTF_DECORATE
#define STB_SPRINTF_DECORATE(name) stbsp_##name // define this before including if you want to change the names
#endif

STBSP__PUBLICDEC int STB_SPRINTF_DECORATE(myvsprintf)(char* buf, char const* fmt, va_list va);
STBSP__PUBLICDEC int STB_SPRINTF_DECORATE(myvsnprintf)(char* buf, int count, char const* fmt, va_list va);
STBSP__PUBLICDEC int STB_SPRINTF_DECORATE(mysprintf)(char* buf, char const* fmt, ...) STBSP__ATTRIBUTE_FORMAT(2, 3);
STBSP__PUBLICDEC int STB_SPRINTF_DECORATE(mysnprintf)(char* buf, int count, char const* fmt, ...) STBSP__ATTRIBUTE_FORMAT(3, 4);

STBSP__PUBLICDEC int STB_SPRINTF_DECORATE(myvsprintfcb)(STBSP_SPRINTFCB* callback, void* user, char* buf, char const* fmt, va_list va);
STBSP__PUBLICDEC void STB_SPRINTF_DECORATE(myset_separators)(char comma, char period);


#endif // !_PM_SPRINTF_H_

