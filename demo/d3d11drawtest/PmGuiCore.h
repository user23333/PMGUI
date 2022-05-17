/*
Author: CSlime
Github: https://github.com/cs1ime
*/

#pragma once

#ifndef _PMGUI_CORE_H_
#define _PMGUI_CORE_H_

#include "PmVec.h"
#include "PmUtil.h"

typedef void (*PM_FUNC_AddLine)(float x1, float y1, float x2, float y2, int col);
typedef void (*PM_FUNC_FillRect)(float x1, float y1, float w, float h, int col);
typedef void (*PM_FUNC_FillTriangle)(float x1, float y1, float x2, float y2, float x3, float y3, int col);
typedef void (*PM_FUNC_AddText)(float x, float y, float size, int col, const char* text);
typedef float (*PM_FUNC_GetTextWidth)(const char* text, float size);

enum PmUserEventType {
	PMEVT_NONE = 0,
	PMEVT_LM_DOWN,
	PMEVT_LM_UP,
	PMEVT_RM_DOWN,
	PMEVT_RM_UP,
	PMEVT_M_MOVE,

	PMEVT_KEY_DOWN,
	PMEVT_KEY_UP
};

struct PmUserEvent {
	PmUserEventType EvtType;
	struct {
		struct {
			PmVec2 cur_pos;
		}M;
	}EvtData;
	//struct {

	//};
};
struct PmKeyMap {
	bool map[256];
};
struct PmKeyEvent {
	PmKeyMap ResultMap;
};
namespace Gui {
	void SetFunc_AddLine(PM_FUNC_AddLine pFunc);
	void SetFunc_FillRect(PM_FUNC_FillRect pFunc);
	void SetFunc_FillTriangle(PM_FUNC_FillTriangle pFunc);
	void SetFunc_AddText(PM_FUNC_AddText pFunc);
	void SetFunc_GetTextWidth(PM_FUNC_GetTextWidth pFunc);
	void Init(float font_height, PmRect MainFrame);

	void PostUserEvent(PmUserEvent Event);
	bool Begin(int tab_cnt, const char* title);
	void End();
	void Text(PmCol col, const char* text_start);
	void CheckBox(const char* text_start, bool* is_checked);
	bool ComboBox(const char* title,int* cur_idx, const char** entry_text, int entry_cnt);
	void Slider(const char* title, float* value, float min, float max);
	void Slider(const char* title, float* value, float min, float max, float block);
	bool Tab(const char* title);

}

namespace SGUI {
	void SetFunc_AddLine(PM_FUNC_AddLine pFunc);
	void SetFunc_FillRect(PM_FUNC_FillRect pFunc);
	void SetFunc_AddText(PM_FUNC_AddText pFunc);
	void SetFunc_GetTextWidth(PM_FUNC_GetTextWidth pFunc);

	void Begin();
	void PostKeyEvent(PmKeyEvent KeyEvent);
	void Init(float font_height, PmRect MainFrame);
	void MiddleText(const char* title, float font_height = 1.2f, PmCol col = PM_WHITE);
	void Text(const char* title, float font_height = 1.f, PmCol col = PM_WHITE);
	void Check(const char* title, bool* gptr, PmCol col = PM_WHITE, float scale = 1.f);
	void CheckAndBind(const char* title, bool* gptr, int vkey, PmCol col = PM_WHITE);
	void Slider(const char* title, int* gptr, int single, int min, int max, PmCol col = PM_WHITE);
	void Slider(const char* title, float* gptr, float single, float min, float max, PmCol col = PM_WHITE);
	void Combo(const char* title, int* gptr, const char** combo_list, int combo_count, PmCol col = PM_WHITE);

	void End();
}

#endif // !_PMGUI_CORE_INCLUDED_H_

