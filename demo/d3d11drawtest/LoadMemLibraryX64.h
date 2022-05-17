/*
Author: CSlime
Github: https://github.com/cs1ime
*/
#pragma once
#ifndef __LOADMEMLIBRARYX64__
#define __LOADMEMLIBRARYX64__

#include "windows.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	ULONG64 LoadMemLibraryX64(PVOID DllData, ULONG64 lpresverd);
	ULONG64 GetMemProcAddressX64(ULONG64 ModuleBase, LPCSTR ProcName);
	ULONG64 GetMemModuleHandleX64(LPCSTR ModuleName);

#ifdef __cplusplus
}
#endif // __cplusplus



#endif // !__LOADMEMLIBRARYX64__
