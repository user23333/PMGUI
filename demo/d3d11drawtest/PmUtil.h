/*
Author: CSlime
Github: https://github.com/cs1ime
*/

#pragma once

#ifndef _PMCOL_H_
#define _PMCOL_H_

#include "PmSprintf.h"
#include "PmCol.h"


#define PM_VK_F1             0x70
#define PM_VK_F2             0x71
#define PM_VK_F3             0x72
#define PM_VK_F4             0x73
#define PM_VK_F5             0x74
#define PM_VK_F6             0x75
#define PM_VK_F7             0x76
#define PM_VK_F8             0x77
#define PM_VK_F9             0x78
#define PM_VK_F10            0x79
#define PM_VK_F11            0x7A
#define PM_VK_F12            0x7B
#define PM_VK_F13            0x7C
#define PM_VK_F14            0x7D
#define PM_VK_F15            0x7E
#define PM_VK_F16            0x7F
#define PM_VK_F17            0x80
#define PM_VK_F18            0x81
#define PM_VK_F19            0x82
#define PM_VK_END            0x23
#define PM_VK_HOME           0x24
#define PM_VK_LEFT           0x25
#define PM_VK_UP             0x26
#define PM_VK_RIGHT          0x27
#define PM_VK_DOWN           0x28
#define PM_VK_LSHIFT         0xA0
#define PM_VK_RSHIFT         0xA1
#define PM_VK_LCONTROL       0xA2
#define PM_VK_RCONTROL       0xA3
#define PM_VK_LMENU          0xA4
#define PM_VK_RMENU          0xA5

void pm_zeromem(void* addr, int sz);
void pm_memcpy(void* dst, const void* src, int sz);
void pm_strcpy(char* dst, const char* src);

void pm_xor_data(void* data, int size, unsigned int xor_key);
#endif // !_PMCOL_H_

