
/*
Author: CSlime
Github: https://github.com/cs1ime
*/

#include "PmWin32Event.h"

namespace Key {
	bool LM_Down = false;
	bool RM_Down = false;

    bool LM_Up = false;
    bool RM_Up = false;
}
#define IsKeyDown(v)(GetAsyncKeyState(v))
#define IsKeyUp(v)(!GetAsyncKeyState(v))

bool KeyProcDown(int vkey, bool* bv) {
    if (IsKeyDown(vkey)) {
        if (!*bv) {
            *bv = true;
            return true;
        }
    }
    else {
        *bv = false;
    }
    return false;
}
bool KeyProcUp(int vkey, bool* bv) {
    if (IsKeyUp(vkey)) {
        if (!*bv) {
            *bv = true;
            return true;
        }
    }
    else {
        *bv = false;
    }
    return false;
}

PM_FUNC_GetMousePos g_GetMousePos = 0;

void PM_SetFunc_GetMousePos(PM_FUNC_GetMousePos pFunc) {
    g_GetMousePos = pFunc;
}

PmVec2 prev_m_pos = { -1.f,-1.f };
bool m_pos_inited = false;
void PM_UpdateEvent() {
    PmVec2 m_pos = g_GetMousePos();
    if (KeyProcDown(VK_LBUTTON, &Key::LM_Down)) {
        PmUserEvent evt;
        evt.EvtType = PMEVT_LM_DOWN;
        evt.EvtData.M.cur_pos = m_pos;
        Gui::PostUserEvent(evt);
    }
    else if (KeyProcUp(VK_LBUTTON, &Key::LM_Up)) {
        PmUserEvent evt;
        evt.EvtType = PMEVT_LM_UP;
        evt.EvtData.M.cur_pos = m_pos;
        Gui::PostUserEvent(evt);
    }
    if (m_pos_inited) {
        if (!(prev_m_pos == m_pos)) {
            PmUserEvent evt;
            evt.EvtType = PMEVT_M_MOVE;
            evt.EvtData.M.cur_pos = m_pos;
            Gui::PostUserEvent(evt);
        }
    }
    else {
        m_pos_inited = true;
    }
    prev_m_pos = m_pos;


}

extern PmKeyMap VaildKeyMap;
PmKeyMap ProcMap = { 0 };
void PM_UpdateKeyEvent() {
    PmKeyEvent key;
    memset(&key, 0, sizeof(key));
    for (int i = 0; i < 256; i++) {
        if (VaildKeyMap.map[i]) {
            key.ResultMap.map[i] = KeyProcDown(i, &ProcMap.map[i]);
        }
    }
    SGUI::PostKeyEvent(key);
}
void PM_UpdateBindKeyEvent() {
    PmKeyEvent key;
    memset(&key, 0, sizeof(key));
    for (int i = 0; i < 256; i++) {
        if (i == PM_VK_DOWN || i == PM_VK_UP || i == PM_VK_LEFT || i == PM_VK_RIGHT)
            continue;
        if (VaildKeyMap.map[i]) {
            key.ResultMap.map[i] = KeyProcDown(i, &ProcMap.map[i]);
        }
    }
    SGUI::PostKeyEvent(key);
}