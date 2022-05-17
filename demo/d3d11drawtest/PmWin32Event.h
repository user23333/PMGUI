/*
Author: CSlime
Github: https://github.com/cs1ime
*/

#pragma once

#ifndef __PMWIN32EVENT__H__
#define __PMWIN32EVENT__H__

#include "PmGuiCore.h"
#include "windows.h"
typedef PmVec2(*PM_FUNC_GetMousePos)();

void PM_SetFunc_GetMousePos(PM_FUNC_GetMousePos pFunc);
void PM_UpdateEvent();
void PM_UpdateKeyEvent();
void PM_UpdateBindKeyEvent();

#endif // !__PMWIN32EVENT__H__

