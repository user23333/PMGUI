/*
Author: CSlime
Github: https://github.com/cs1ime
*/

#include "PmGuiCore.h"
#define PM_XORKEY (0xDAAE6666)

struct PmWindow {
	struct PmWindow* prev_window;

	//PmVec2 base_pos;
	PmRect window_sp;

	PmRect logo_sp;

	PmRect title_sp;

	PmRect bar_sp;

	PmRect frame_sp;

	PmRect user_sp;

	PmVec2 cur_emt_pos;

#define PM_MAX_SUBWINDOWS (5)
	struct PmWindow* sub_windows[PM_MAX_SUBWINDOWS];

};

typedef bool PmMouseKeyState;
#define PmMouseKeyDown (true)
#define PmMouseKeyUp (false)

struct PmMouseState {
	PmVec2 pos;
	PmMouseKeyState LM;
	PmMouseKeyState RM;
	
};
struct PmTabWindow {
	bool text_or_image;//0=text 1=image
	char title[100];
	void* tex_id;

};
PmKeyMap VaildKeyMap = { 0 };
void SetVaildKey(int key_idx) {
	VaildKeyMap.map[key_idx] = 1;
}
namespace Gui {

	PM_FUNC_AddLine AddLine = 0;
	PM_FUNC_FillRect FillRect = 0;
	PM_FUNC_FillTriangle FillTriangle = 0;
	PM_FUNC_AddText AddText = 0;
	PM_FUNC_GetTextWidth GetTextWidth = 0;

	void SetFunc_AddLine(PM_FUNC_AddLine pFunc) {
		AddLine = pFunc;
	}
	void SetFunc_FillRect(PM_FUNC_FillRect pFunc) {
		FillRect = pFunc;
	}
	void SetFunc_AddText(PM_FUNC_AddText pFunc) {
		AddText = pFunc;
	}
	void SetFunc_FillTriangle(PM_FUNC_FillTriangle pFunc) {
		FillTriangle = pFunc;
	}
	void SetFunc_GetTextWidth(PM_FUNC_GetTextWidth pFunc) {
		GetTextWidth = pFunc;
	}
#define PM_MAX_REVERSEDWINDOW (20)
	PmWindow g_ReversedWindow[PM_MAX_REVERSEDWINDOW];
	int g_re_idx = 0;
	PmWindow g_MainWindow = { 0 };
	float g_tab_off = 0.f;
	int g_tab_selected_index = 0;
	int g_tab_cur_index = 0;
	bool g_tab_changed = false;
	bool g_tab_added = false;
	bool g_MainWindows_Enabled = true;

	float g_fontHeight = 0.f;
	PmMouseState g_mouse = { {0.f,0.f}, PmMouseKeyUp,PmMouseKeyUp };
	PmVec2 g_last_tab_pos_rel = { -1.f,-1.f };


	PmWindow* GetNewReversedWindow() {
		PM_ASSERT(g_re_idx < PM_MAX_REVERSEDWINDOW);
		return &g_ReversedWindow[g_re_idx++];
	}
	void NewFrame() {
		g_re_idx = 0;

	}
	float g_logo_w = 30.f;
	void Init(float font_height, PmRect MainFrame) {
		g_fontHeight = font_height;
		g_logo_w = g_fontHeight + g_fontHeight / 2;
		g_MainWindow.window_sp = MainFrame;
		g_last_tab_pos_rel = PmVec2(g_fontHeight / 2, g_fontHeight / 2);
	}
	bool IsInRect(PmVec2 pos, PmRect rect) {
		return pos.x >= rect.x && pos.y >= rect.y && pos.x <= (rect.x + rect.w) && pos.y <= (rect.y + rect.h);
	}
	PmVec2 g_last_down_pos = { -1.f,-1.f };
	PmVec2 g_last_up_pos = { -1.f,-1.f };
	bool g_click_event = false;
	bool g_move_window_event = false;
	bool g_move_logo_event = false;
	bool g_window_moving = false;
	bool g_set_moving_false = false;
	PmVec2 g_last_start_mov_window_pos = { -1.f,-1.f };
	PmVec2 g_last_end_mov_window_pos = { -1.f,-1.f };

	bool g_moving_logo = false;

	PmRect g_combo_list_rect = { -1.f,-1.f,-1.f,-1.f };
	PmRect g_combo_list_rect2 = { -1.f,-1.f,-1.f,-1.f };
	bool g_combo_list_active = false;
	bool g_combo_block_active = false;
	bool g_combo_set_block_active_false = false;

	PmVec2 g_last_cbox_pos = { -1.f,-1.f };

	PmVec2 g_last_slist_pos = { -1.f,-1.f };
	bool g_moving_slist = false;

	bool g_moving = false;
	PmVec2 g_last_moving_off = { -1.f,-1.f };
#include <stdio.h>
	int g_tab_cnt = 0;

	void PostUserEvent(PmUserEvent Event) {
		switch (Event.EvtType)
		{
		case PMEVT_M_MOVE: 
		{
			PmVec2 prev_pos = g_mouse.pos;
			g_mouse.pos = Event.EvtData.M.cur_pos;
			if (g_mouse.LM == PmMouseKeyDown) {
				if (g_move_window_event) {
					//printf("move_event\n");
					PmVec2 off = Event.EvtData.M.cur_pos - prev_pos;
					g_moving_logo = false;
					if (off.x > 0.f || off.y > 0.f || off.x < 0.f || off.y < 0.f) {
						g_window_moving = true;
					}
						//printf("")
					g_MainWindow.window_sp.pos = g_MainWindow.window_sp.pos + off;
					/*if (IsInRect(g_last_down_pos, g_MainWindow.logo_sp) && g_moving_logo == false) {
						g_logo_mov_cache = g_logo_mov_cache + off;
						if (fabs(g_logo_mov_cache.x) >= 5.f) {
							float overflow = g_logo_mov_cache.x - 5.f;
							g_logo_mov_cache.x = 5.f;
							off.x = overflow;
							g_moving_logo = true;
						}
						if (fabs(g_logo_mov_cache.y) >= 5.f) {
							//if()
							float overflow = g_logo_mov_cache.y - 5.f;
							g_logo_mov_cache.y = 5.f;
							off.y = overflow;
							g_moving_logo = true;
						}
					}
					
					if (g_moving_logo) {
						printf("g_moving_logo :%d\n",g_moving_logo);
						if (off.x > 0.f || off.y > 0.f) {
							g_moving = true;
						}
						//printf("")
						g_MainWindow.window_sp.pos = g_MainWindow.window_sp.pos + off;
					}*/

				}
				if (g_move_logo_event) {
					PmVec2 off = Event.EvtData.M.cur_pos - prev_pos;
					PmVec2 poff = Event.EvtData.M.cur_pos - g_last_down_pos;
					//printf("%f %f\n", poff.x, poff.y);
					PmVec2 g_logo_mov_cache = poff;
					if (!g_moving_logo) {
						if (g_logo_mov_cache.x > 5.f) {
							float overflow = g_logo_mov_cache.x - 5.f;
							g_logo_mov_cache.x = 5.f;
							off.x = overflow;
							g_moving_logo = true;
						}
						if (g_logo_mov_cache.y > 5.f) {
							//if()
							float overflow = g_logo_mov_cache.y - 5.f;
							g_logo_mov_cache.y = 5.f;
							off.y = overflow;
							g_moving_logo = true;
						}
						if (g_logo_mov_cache.x < -5.f) {
							float overflow = g_logo_mov_cache.x + 5.f;
							g_logo_mov_cache.x = -5.f;
							off.x = overflow;
							g_moving_logo = true;
						}
						if (g_logo_mov_cache.y < -5.f) {
							//if()
							float overflow = g_logo_mov_cache.y + 5.f;
							g_logo_mov_cache.y = -5.f;
							off.y = overflow;
							g_moving_logo = true;
						}
					}

					if (g_moving_logo) {
						//printf("g_moving_logo :%d\n", g_moving_logo);
						if (off.x > 0.f || off.y > 0.f || off.x < 0.f || off.y < 0.f) {
							g_window_moving = true;
						}
						//printf("")
						g_MainWindow.window_sp.pos = g_MainWindow.window_sp.pos + off;
					}
				}
				if (!g_move_window_event && !g_move_logo_event) {
					PmVec2 off = Event.EvtData.M.cur_pos - prev_pos;
					if (off.x > 0.f || off.y > 0.f || off.x < 0.f || off.y < 0.f) {
						g_last_moving_off = off;
						g_moving = true;
					}
					else {
						g_moving = false;
					}
				}
			}

			break;
		}
		case PMEVT_LM_DOWN:
		{
			g_mouse.LM = PmMouseKeyDown;
			g_last_down_pos = Event.EvtData.M.cur_pos;
			g_mouse.pos = Event.EvtData.M.cur_pos;
			if (IsInRect(g_last_down_pos, g_MainWindow.bar_sp)) {
				g_move_window_event = true;
				g_move_logo_event = false;
			}
			if (IsInRect(g_last_down_pos, g_MainWindow.logo_sp)) {
				g_move_logo_event = true;
				g_move_window_event = false;
			}
			if (g_combo_list_active) {
				if (!IsInRect(g_mouse.pos, g_combo_list_rect)/* && !IsInRect(g_mouse.pos, g_combo_list_rect2)*/) {
					g_combo_list_active = false;
				}
				//g_combo_block_active
				if (IsInRect(g_mouse.pos, g_combo_list_rect2)) {
					g_combo_list_active = false;
					g_combo_block_active = true;
					//printf("set block\n");
				}
			}

			break;
		}
		case PMEVT_LM_UP:
		{
			//printf("up\n");
			g_mouse.LM = PmMouseKeyUp;
			g_last_up_pos = Event.EvtData.M.cur_pos;
			g_mouse.pos = Event.EvtData.M.cur_pos;

			g_set_moving_false = true;

			g_click_event = true;
			g_move_window_event = false;
			g_move_logo_event = false;
			g_moving_logo = false;
			g_combo_set_block_active_false = true;
			g_moving_slist = false;

			break;
		}
		default:
			break;
		}
	}
	void AddBox(float x, float y, float w, float h, PmCol col) {
		AddLine(x, y, x + w, y, col);
		AddLine(x, y, x, y + h, col);
		AddLine(x + w, y, x + w, y + h, col);
		AddLine(x, y + h, x + w, y + h, col);
	}

	const int alpha = 220;
	PmCol g_bg_col = PM_ARGB(alpha,0, 0, 20);

	bool Begin(int tab_cnt, const char* title) {
		float x = g_MainWindow.window_sp.x;
		float y = g_MainWindow.window_sp.y;
		float w = g_MainWindow.window_sp.w;
		float h = g_MainWindow.window_sp.h;
		//PmVec2 pos2 = pos + wh;

		float logo_w = g_logo_w;

		g_tab_cnt = tab_cnt;
		g_MainWindow.logo_sp = PmRect(x + 1.f, y + 1.f, logo_w, logo_w);
		PmRect logo_rect = g_MainWindow.logo_sp;
		if (g_click_event) {
			if (IsInRect(g_last_down_pos, logo_rect) && IsInRect(g_last_up_pos, logo_rect) && g_window_moving == false) {
				g_MainWindows_Enabled = !g_MainWindows_Enabled;
				//printf("%d\n", g_MainWindows_Enabled);
			}
		}

		PmCol logo_col;
		//g_mouse.pos.x = 60.f;
		//g_mouse.pos.y = 60.f;

		if (!IsInRect(g_mouse.pos, logo_rect) || g_moving_logo)
			logo_col = PM_XRGB(251, 87, 4);
		else
			logo_col = PM_XRGB(0, 87, 4);


		FillRect(x + 1.f, y + 1.f, logo_w, logo_w, logo_col);

		PmCol bg_col = g_bg_col;
		//236,239,244
		FillRect(x + 1.f + logo_w, y + 1.f, w - logo_w, logo_w, bg_col);

		//描边
		AddLine(x, y, x + w, y, PM_BLACK);
		AddLine(x, y + logo_w, x + w, y + logo_w, PM_BLACK);

		AddLine(x + logo_w, y, x + logo_w, y + logo_w, PM_BLACK);

		AddText(x + 1.f + logo_w + logo_w / 3, y + (logo_w - g_fontHeight) / 2, g_fontHeight, PM_XRGB(222, 221, 255), title);

		PmVec2 FrameStart = PmVec2(x + 1.f, y + 1.f + logo_w);
		
		PmVec2 UserPos = PmVec2(FrameStart.x + 5.f, FrameStart.y + 5.f);
		PmVec2 UserWh = PmVec2(w - 5.f, h - 5.f);

		if (g_MainWindows_Enabled) {
			FillRect(FrameStart.x, FrameStart.y, w, h, bg_col);

			//AddLine(FrameStart.x)

			AddLine(x, y, x, y + logo_w + h, PM_BLACK);
			AddLine(x, FrameStart.y + h, x + w, FrameStart.y + h, PM_BLACK);
			AddLine(x + w, y, x + w, y + logo_w + h, PM_BLACK);
			//AddLine(pos.x, pos.y, pos.x + wh.w, pos.y + logo_w, PM_BLACK);
			//AddLine(pos.x, pos.y, pos.x + wh.w, pos.y, PM_BLACK);
			//AddLine(pos.x, pos.y, pos.x + wh.w, pos.y, PM_BLACK);
			PmVec2 tran1 = PmVec2(x + logo_w / 4, y + logo_w / 4);
			PmVec2 tran2 = PmVec2(x + logo_w * 0.75, y + logo_w / 4);
			PmVec2 tran3 = PmVec2(x + logo_w / 2, y + logo_w * 0.75);

			FillTriangle(tran1.x, tran1.y, tran2.x, tran2.y, tran3.x, tran3.y, PM_RED);
			AddLine(tran1.x, tran1.y, tran2.x, tran2.y, PM_BLACK);
			AddLine(tran2.x, tran2.y, tran3.x, tran3.y, PM_BLACK);
			AddLine(tran3.x, tran3.y, tran1.x, tran1.y, PM_BLACK);

		}
		else {
			AddLine(x, y, x, y + logo_w, PM_BLACK);
			AddLine(x + w, y, x + w, y + logo_w, PM_BLACK);

			PmVec2 tran1 = PmVec2(x + logo_w / 4, y + logo_w / 4);
			PmVec2 tran2 = PmVec2(x + logo_w / 4, y + logo_w * 0.75);
			PmVec2 tran3 = PmVec2(x + logo_w * 0.75, y + logo_w / 2);

			FillTriangle(tran1.x, tran1.y, tran2.x, tran2.y, tran3.x, tran3.y, PM_RED);
			AddLine(tran1.x, tran1.y, tran2.x, tran2.y, PM_BLACK);
			AddLine(tran2.x, tran2.y, tran3.x, tran3.y, PM_BLACK);
			AddLine(tran3.x, tran3.y, tran1.x, tran1.y, PM_BLACK);

		}

		

		g_MainWindow.user_sp = PmRect(UserPos.x, UserPos.y, UserWh.w, UserWh.h);
		g_MainWindow.frame_sp = PmRect(FrameStart.x, FrameStart.y, w, h);
		g_MainWindow.title_sp = PmRect(x, y, w + 2.f, logo_w + 2.f);
		g_MainWindow.bar_sp = PmRect(x + 1.f + logo_w, y, w + 2.f - logo_w, logo_w + 2.f);
		g_MainWindow.cur_emt_pos = g_MainWindow.user_sp.pos;

		return g_MainWindows_Enabled;
	}

	const char** g_render_entry_text = 0;
	int g_render_entry_cnt = 0;
	PmCol g_render_entry_col = 0;
	int* g_prender_cur_idx = 0;

	void End() {
		if (g_combo_list_active) {

			FillRect(g_combo_list_rect.x, g_combo_list_rect.y, g_combo_list_rect.w, g_combo_list_rect.h + g_fontHeight * 0.2, g_bg_col);
			AddBox(g_combo_list_rect.x, g_combo_list_rect.y, g_combo_list_rect.w, g_combo_list_rect.h + g_fontHeight * 0.2, PM_BLACK);

			PmVec2 m_off_rect = g_mouse.pos - g_combo_list_rect.pos;
			int g_render_selected_idx = 0;
			g_render_selected_idx = -1;
			if (
				m_off_rect.x >= 0.f && m_off_rect.y >= 0.f && 
				m_off_rect.x <= g_combo_list_rect.w && m_off_rect.y <= g_combo_list_rect.h) {
				float off_y = 0.f;
				for (int i = 0; i < g_render_entry_cnt; i++) {
					float block_size = g_fontHeight + g_fontHeight * 0.4;
					if (m_off_rect.y >= off_y && m_off_rect.y < off_y + block_size) {
						g_render_selected_idx = i;
						if (g_click_event) {
							*g_prender_cur_idx = i;
							g_combo_list_active = false;
						}
						break;
					}
					off_y += block_size;
				}
			}
			
			
			PmVec2 text_pos = PmVec2(g_combo_list_rect.x + g_fontHeight * 0.2, g_combo_list_rect.y + g_fontHeight * 0.125 * 2);
			if (g_render_entry_text) {
				for (int i = 0; i < g_render_entry_cnt; i++) {
					if (i == g_render_selected_idx) {
						FillRect(
							g_combo_list_rect.x + g_fontHeight * 0.2,
							g_combo_list_rect.y + (g_fontHeight + g_fontHeight * 0.4) * i,
							g_combo_list_rect.w - g_fontHeight * 0.2 * 2,
							g_fontHeight + g_fontHeight * 0.2 * 2, PM_BLUE);
						AddBox(g_combo_list_rect.x + g_fontHeight * 0.2,
							g_combo_list_rect.y + (g_fontHeight + g_fontHeight * 0.4) * i ,
							g_combo_list_rect.w - g_fontHeight * 0.2 * 2,
							g_fontHeight + g_fontHeight * 0.2 * 2, PM_BLACK);
					}
					AddText(text_pos.x, text_pos.y + g_fontHeight * 0.2, g_fontHeight, PM_XRGB(115, 186, 253), g_render_entry_text[i]);
					text_pos.y += g_fontHeight + g_fontHeight * 0.4;
				}
			}
		}
		g_click_event = false;
		if (g_set_moving_false) {
			g_window_moving = false;
			g_set_moving_false = false;
		}
		if (g_combo_set_block_active_false) {
			g_combo_block_active = false;
			g_combo_set_block_active_false = false;
		}
		g_moving = false;
		g_tab_added = false;
		g_prender_cur_idx = 0;
		g_tab_off = 0;
		g_tab_cnt = 0;
		g_tab_cur_index = 0;
		g_tab_changed = false;
	}
	void Text(PmCol col, const char* text_start) {
		AddText(g_MainWindow.cur_emt_pos.x, g_MainWindow.cur_emt_pos.y, g_fontHeight, col, text_start);
		g_MainWindow.cur_emt_pos.y += g_fontHeight + g_fontHeight / 5.f;
	}
	void CheckBox(const char* text_start, bool* is_checked) {
		
		float box_wid = g_fontHeight * 0.875;
		float box_off = g_fontHeight * 0.125;
		float text_off = g_fontHeight;

		PmRect box_rect = PmRect(g_MainWindow.cur_emt_pos.x, g_MainWindow.cur_emt_pos.y, box_wid, box_wid);
		PmRect pd_rect = PmRect(g_MainWindow.cur_emt_pos.x, g_MainWindow.cur_emt_pos.y, g_MainWindow.window_sp.w, g_fontHeight);

		
		AddText(g_MainWindow.cur_emt_pos.x + text_off, g_MainWindow.cur_emt_pos.y, g_fontHeight, PM_XRGB(228, 228, 228), text_start);

		if (IsInRect(g_mouse.pos, pd_rect) && !g_combo_list_active) {
			FillRect(box_rect.x + 1.f, box_rect.y + 1.f, box_rect.w - 1.f, box_rect.h - 2.f, PM_XRGB(196, 199, 204));
		}
		AddBox(g_MainWindow.cur_emt_pos.x, g_MainWindow.cur_emt_pos.y, box_wid, box_wid, PM_XRGB(62, 142, 246));
		if (g_click_event && IsInRect(g_last_down_pos, pd_rect) && IsInRect(g_last_up_pos, pd_rect) && !g_combo_list_active) {
			*is_checked = !*is_checked;
		}
		if (*is_checked) {
			float ico_wid = box_wid * 0.5;
			float ico_off = box_wid * 0.25;
			PmRect ico_rect = PmRect(box_rect.x + ico_off, box_rect.y + ico_off, ico_wid, ico_wid);
			FillRect(ico_rect.x, ico_rect.y, ico_rect.w, ico_rect.h, PM_YELLOW);
			AddBox(ico_rect.x, ico_rect.y, ico_rect.w, ico_rect.h, PM_BLACK);
		}
		g_MainWindow.cur_emt_pos.y += g_fontHeight + g_fontHeight / 2.f;

	}
	PmVec2 g_last_combo_pos = { -1.f,-1.f };
	bool ComboBox(const char* title,int *cur_idx, const char** entry_text, int entry_cnt) {

		PmRect cbbox_rect = PmRect(g_MainWindow.cur_emt_pos.x, g_MainWindow.cur_emt_pos.y, g_fontHeight * 6.2f, g_fontHeight * 1.2f);

		PmRect icon_rect = PmRect(g_MainWindow.cur_emt_pos.x + cbbox_rect.w - g_fontHeight * 1.2f, g_MainWindow.cur_emt_pos.y, g_fontHeight * 1.2f, g_fontHeight * 1.2f);

		PmRect user_rect = cbbox_rect;

		bool self_selected = g_combo_list_active && IsInRect(g_last_combo_pos, user_rect);

		bool is_in_icon_rect = g_combo_list_active ? (self_selected ? IsInRect(g_mouse.pos, user_rect) : false) : IsInRect(g_mouse.pos, user_rect);
		PmCol icon_col = is_in_icon_rect ? PM_XRGB(11, 85, 255) : PM_XRGB(115, 186, 253);
		FillRect(icon_rect.x, icon_rect.y, icon_rect.w, icon_rect.h, icon_col);
		AddLine(icon_rect.x, icon_rect.y, icon_rect.x, icon_rect.y + icon_rect.h, PM_BLACK);
		AddBox(cbbox_rect.x, cbbox_rect.y, cbbox_rect.w, cbbox_rect.h, PM_XRGB(115, 186, 253));
		if (*cur_idx >= 0 && *cur_idx < entry_cnt) {
			AddText(cbbox_rect.x + g_fontHeight * 0.125, cbbox_rect.y + g_fontHeight * 0.2, g_fontHeight, PM_XRGB(115, 186, 253), entry_text[*cur_idx]);
		}

		PmVec2 tran1 = PmVec2(icon_rect.x + icon_rect.w * 0.25f, icon_rect.y + icon_rect.w * 0.25f);
		PmVec2 tran2 = PmVec2(icon_rect.x + icon_rect.w * 0.75f, icon_rect.y + icon_rect.w * 0.25f);
		PmVec2 tran3 = PmVec2(icon_rect.x + icon_rect.w * 0.5f, icon_rect.y + icon_rect.w * 0.75f);
		FillTriangle(tran1.x, tran1.y,
			tran2.x, tran2.y, 
			tran3.x, tran3.y, PM_GREEN);
		AddLine(tran1.x, tran1.y, tran2.x, tran2.y, PM_BLACK);
		AddLine(tran2.x, tran2.y, tran3.x, tran3.y, PM_BLACK);
		AddLine(tran3.x, tran3.y, tran1.x, tran1.y, PM_BLACK);

		PmVec2 text_pos = PmVec2(g_MainWindow.cur_emt_pos.x + cbbox_rect.w + g_fontHeight * 0.05f, g_MainWindow.cur_emt_pos.y + g_fontHeight * 0.125f);
		AddText(text_pos.x, text_pos.y, g_fontHeight, PM_XRGB(228, 228, 228), title);
		//if (g_click_event) {
			//if(IsInRect(g_mouse.pos,))
		//}
		
		if (g_click_event && (g_combo_list_active ? IsInRect(g_last_combo_pos, user_rect) : true)) {
			if (IsInRect(g_mouse.pos, user_rect) && IsInRect(g_last_down_pos, user_rect)) {
				//处理combo单击事件
				if (g_combo_list_active && IsInRect(g_last_combo_pos, user_rect)) {
					//第二次单击combo
					g_combo_list_active = false;
				}
				else {
					if (!g_combo_block_active) {
						//printf("combo\n");
						//第一次单击combo
						g_last_combo_pos = g_mouse.pos;
						g_combo_list_rect = cbbox_rect;
						g_combo_list_rect.y += cbbox_rect.h;
						g_combo_list_rect.w = cbbox_rect.w;
						if (entry_cnt)
							g_combo_list_rect.h = (g_fontHeight + g_fontHeight * 0.4) * entry_cnt;
						else
							g_combo_list_rect.h = g_fontHeight * 0.4;
						g_combo_list_rect2 = user_rect;

						g_combo_list_active = true;
					}

				}
			}
		}


		g_MainWindow.cur_emt_pos.y += g_fontHeight + g_fontHeight / 1.5f;

		if (g_combo_list_active && IsInRect(g_last_combo_pos, cbbox_rect)) {
			g_render_entry_text = entry_text;
			g_render_entry_cnt = entry_cnt;
			g_prender_cur_idx = cur_idx;
			return true;
		}
			
		return false;
	}

	void Slider(const char* title, float* value, float min, float max) {

		float slider_h = g_fontHeight * 1.25;
		float slider_x = g_MainWindow.cur_emt_pos.x;
		float slider_y = g_MainWindow.cur_emt_pos.y;
		float slider_w = g_fontHeight * 5;

		PmRect slider_rect = PmRect(slider_x, slider_y, slider_w, slider_h);

		AddBox(slider_x, slider_y, slider_w, slider_h, PM_XRGB(115, 186, 253));

		float text_x = slider_x + slider_w + g_fontHeight * 0.125 * 2;
		float text_y = slider_y;
		float yoff = (slider_h - g_fontHeight) / 2;
		AddText(text_x, text_y + yoff, g_fontHeight, PM_XRGB(228, 228, 228), title);


		float bz = (*value - min) / (max - min);
		float block_x = slider_x + (slider_w - g_fontHeight) * bz + g_fontHeight * 0.25;
		float block_y = slider_y + g_fontHeight * 0.125;
		float block_w = g_fontHeight - g_fontHeight * 0.125 * 4;
		float block_h = slider_h - g_fontHeight * 0.125 * 2;

		PmRect block_rect = PmRect(block_x, block_y, block_w, block_h);
		if ((IsInRect(g_mouse.pos, block_rect) && !g_moving_slist) || (g_moving_slist && IsInRect(g_last_slist_pos, slider_rect))) {
			FillRect(block_x, block_y, block_w, block_h, PM_XRGB(11, 85, 255));
		}
		else {
			FillRect(block_x, block_y, block_w, block_h, PM_XRGB(115, 186, 253));
		}

		AddBox(block_x, block_y, block_w, block_h, PM_BLACK);

		float arrow_min_x = slider_x + g_fontHeight * 0.25 + block_w / 2;
		float arrow_max_x = slider_x + slider_w - g_fontHeight * 0.5;
		//float arrow_min_x = slider_x + g_fontHeight * 0.25;
		//float arrow_max_x = slider_x + slider_w - g_fontHeight * 0.125;
		//AddLine(arrow_min_x, block_y + 5.f, arrow_max_x, block_y + 5.f, PM_BLACK);

		float len = max - min;

		if (g_mouse.LM == PmMouseKeyDown && !g_combo_list_active) {
			if (IsInRect(g_mouse.pos, block_rect) && IsInRect(g_last_down_pos, slider_rect)) {
				g_last_slist_pos = g_mouse.pos;
				g_moving_slist = true;
			}
			else if (IsInRect(g_last_down_pos, slider_rect)) {
				float off_x = g_mouse.pos.x - arrow_min_x;
				//printf("%f\n", off_x);
				float slider_all_mov_length = g_fontHeight * 4;
				float max_min = max - min;
				float mov_scale = off_x / slider_all_mov_length;
				float dst = max_min * mov_scale + min;
				if (dst > max) {
					dst = max;
				}
				else if (dst < min) {
					dst = min;
				}
				*value = dst;
			}
		}
		/*if (g_click_event) {
			if (IsInRect(g_mouse.pos, slider_rect) && IsInRect(g_last_down_pos, slider_rect) || !) {

			}
		}*/
		if (g_moving_slist) {
			if (g_moving && IsInRect(g_last_slist_pos, slider_rect)) {

				float off_x = g_last_moving_off.x;
				bool go_next = true;
				if (g_mouse.pos.x < arrow_min_x || g_mouse.pos.x > arrow_max_x) {
					if (g_mouse.pos.x > arrow_max_x) {
						go_next = false;
						*value = max;
					}
					else if (g_mouse.pos.x < arrow_min_x) {
						go_next = false;
						*value = min;
					}
				}
				if (go_next) {
					//printf("moving_off:%1.1f\n", off_x);
					float slider_all_mov_length = g_fontHeight * 4;
					float max_min = max - min;
					float mov_scale = off_x / slider_all_mov_length;
					float dst = *value + max_min * mov_scale;
					if (dst > max) {
						dst = max;
					}
					else if (dst < min) {
						dst = min;
					}
					*value = dst;
				}
			}
		}
		g_MainWindow.cur_emt_pos.y += g_fontHeight + g_fontHeight / 2.f;
	}
	float g_moving_slider_value = 0.f;
	bool g_first_moving = false;
	void Slider(const char* title, float* value, float min, float max, float block) {

		float slider_h = g_fontHeight * 1.25;
		float slider_x = g_MainWindow.cur_emt_pos.x;
		float slider_y = g_MainWindow.cur_emt_pos.y;
		float slider_w = g_fontHeight * 5;

		PmRect slider_rect = PmRect(slider_x, slider_y, slider_w, slider_h);

		AddBox(slider_x, slider_y, slider_w, slider_h, PM_XRGB(115, 186, 253));

		float text_x = slider_x + slider_w + g_fontHeight * 0.125 * 2;
		float text_y = slider_y;
		float yoff = (slider_h - g_fontHeight) / 2;
		AddText(text_x, text_y + yoff, g_fontHeight, PM_XRGB(228, 228, 228), title);

		PmRect draw_block_rect;

		float bz = (*value - min) / (max - min);
		float block_x = slider_x + (slider_w - g_fontHeight) * bz + g_fontHeight * 0.25;
		float block_y = slider_y + g_fontHeight * 0.125;
		float block_w = g_fontHeight - g_fontHeight * 0.125 * 4;
		float block_h = slider_h - g_fontHeight * 0.125 * 2;

		draw_block_rect = PmRect(block_x, block_y, block_w, block_h);

		PmRect cmp_block_rect;
		if ((g_moving_slist && IsInRect(g_last_slist_pos, slider_rect))) {
			float bz = (g_moving_slider_value - min) / (max - min);
			float block_x = slider_x + (slider_w - g_fontHeight) * bz + g_fontHeight * 0.25;
			float block_y = slider_y + g_fontHeight * 0.125;
			float block_w = g_fontHeight - g_fontHeight * 0.125 * 4;
			float block_h = slider_h - g_fontHeight * 0.125 * 2;

			cmp_block_rect = PmRect(block_x, block_y, block_w, block_h);
		}
		else {
			cmp_block_rect = draw_block_rect;
		}

		int col = PM_XRGB(115, 186, 253);
		if ((IsInRect(g_last_slist_pos, slider_rect) && g_moving_slist) || (IsInRect(g_mouse.pos, cmp_block_rect) && !g_moving_slist)) {
			FillRect(block_x, block_y, block_w, block_h, PM_XRGB(11, 85, 255));
		}
		else {
			FillRect(block_x, block_y, block_w, block_h, PM_XRGB(115, 186, 253));
		}
		AddBox(block_x, block_y, block_w, block_h, PM_BLACK);

		float arrow_min_x = slider_x + g_fontHeight * 0.25 + cmp_block_rect.w / 2;
		float arrow_max_x = slider_x + slider_w - g_fontHeight * 0.5;
		//float arrow_min_x = slider_x + g_fontHeight * 0.25;
		//float arrow_max_x = slider_x + slider_w - g_fontHeight * 0.125;
		//AddLine(arrow_min_x, block_y + 5.f, arrow_max_x, block_y + 5.f, PM_BLACK);

		float len = max - min;

		if (g_mouse.LM == PmMouseKeyDown && !g_combo_list_active) {
			if (IsInRect(g_mouse.pos, cmp_block_rect) && IsInRect(g_last_down_pos, slider_rect)) {
				g_last_slist_pos = g_mouse.pos;
				g_moving_slist = true;
				if (g_first_moving == false) {
					g_moving_slider_value = *value;
					g_first_moving = true;
				}

			}
			else if (IsInRect(g_last_down_pos, slider_rect)) {
				float off_x = g_mouse.pos.x - arrow_min_x;
				//printf("%f\n", off_x);
				float slider_all_mov_length = g_fontHeight * 4;
				float mov_scale = off_x / slider_all_mov_length;
				float dst = len * mov_scale + min;
				if (dst > max) {
					dst = max;
				}
				else if (dst < min) {
					dst = min;
				}
				g_moving_slider_value = dst;
				*value = dst - fmod(dst, block);
			}
		}
		else {
			g_first_moving = false;
		}
		/*if (g_click_event) {
			if (IsInRect(g_mouse.pos, slider_rect) && IsInRect(g_last_down_pos, slider_rect) || !) {

			}
		}*/
		if (g_moving_slist) {
			if (g_moving && IsInRect(g_last_slist_pos, slider_rect)) {

				float off_x = g_last_moving_off.x;
				bool go_next = true;
				if (g_mouse.pos.x < arrow_min_x || g_mouse.pos.x > arrow_max_x) {
					if (g_mouse.pos.x > arrow_max_x) {
						go_next = false;
						g_moving_slider_value = max;
						*value = max;
					}
					else if (g_mouse.pos.x < arrow_min_x) {
						go_next = false;
						g_moving_slider_value = min;
						*value = min;
					}
				}
				if (go_next) {
					//printf("moving_off:%1.1f\n", off_x);
					float slider_all_mov_length = g_fontHeight * 4;
					float mov_scale = off_x / slider_all_mov_length;
					float dst = g_moving_slider_value + len * mov_scale;
					if (dst > max) {
						dst = max;
					}
					else if (dst < min) {
						dst = min;
					}
					g_moving_slider_value = dst;
					*value = dst - fmod(dst, block);;
				}
			}
		}
		g_MainWindow.cur_emt_pos.y += g_fontHeight + g_fontHeight / 2;
	}
	void PM_strcpy(char* dst, const char* src) {
		while (*src) {
			*dst++ = *src++;
		}
	}
	bool Tab(const char* title) {
		float tab_w = g_MainWindow.window_sp.w / (float)g_tab_cnt;
		float tab_h = g_fontHeight * 3;
		PmRect TabRect = PmRect(g_MainWindow.frame_sp.x - 1.f + g_tab_off, g_MainWindow.frame_sp.y - 1.f, tab_w, tab_h);

		bool IsSelected = g_tab_cur_index == g_tab_selected_index;

		PmCol tab_col = PM_ARGB(alpha, 115, 186, 253);
		if (IsSelected) {
			tab_col = PM_ARGB(alpha, 11, 85, 255);
		}
		if (IsInRect(g_mouse.pos, TabRect)) {
			tab_col = PM_ARGB(220, 255, 65, 0);
		}
		FillRect(TabRect.x, TabRect.y, TabRect.w, TabRect.h, tab_col);
		AddBox(TabRect.x, TabRect.y, TabRect.w, TabRect.h, PM_BLACK);
		
		PmVec2 text_pos = PmVec2(TabRect.x + (TabRect.w - GetTextWidth(title, g_fontHeight)) / 2, TabRect.y + g_fontHeight);
		AddText(text_pos.x, text_pos.y, g_fontHeight, PM_WHITE, title);

		bool retfalse = false;
		if (g_click_event && g_tab_changed == false) {
			for (int i = 0; i < g_tab_cnt; i++) {
				float tab_w = g_MainWindow.window_sp.w / (float)g_tab_cnt;
				float tab_h = g_fontHeight * 3;
				PmRect TabRect = PmRect(g_MainWindow.frame_sp.x - 1.f + i * tab_w, g_MainWindow.frame_sp.y - 1.f, tab_w, tab_h);
				if (IsInRect(g_mouse.pos, TabRect) && IsInRect(g_last_down_pos, TabRect)) {
					g_tab_selected_index = i;
					break;
				}
			}
			g_tab_changed = true;
		}
		g_tab_off += tab_w;
		if (g_tab_added == false) {
			g_MainWindow.cur_emt_pos.y += tab_h;
			g_tab_added = true;
		}

		return g_tab_cur_index++ == g_tab_selected_index;
	}

}
enum EntryType : int {
	ET_TEXT = 1,
	ET_MIDDLE_TEXT,
	ET_Check,
	ET_Slider,
	ET_Combo, 
};
enum ValueType : int {
	VT_INT = 1,
	VT_FLOAT, 
};
struct ComboListEntry {
	char str[80];
};
struct PmSelectEntry {
	int type;
	int value_type;
	char text[260];
	float font_height_scale;
	PmCol Col;

	const char** combo_list;
	int combo_count;

	bool *check;

	int* int_value;
	int int_single;
	int int_min;
	int int_max;

	float* float_value;
	float float_single;
	float float_min;
	float float_max;
};

namespace SGUI {
#define MAX_ENTRY_COUNT (100)
	PmSelectEntry *VaildSelectEntry[MAX_ENTRY_COUNT];
	int VaildSelectEntryCount = 0;
	PmSelectEntry AllEntry[MAX_ENTRY_COUNT];
	int AllEntryCount = 0;
	PmSelectEntry* BindHotKeyMap[256] = { 0 };
	int g_select_idx = 0;

	PM_FUNC_AddLine AddLine = 0;
	PM_FUNC_FillRect FillRect = 0;
	PM_FUNC_AddText AddText = 0;
	PM_FUNC_GetTextWidth GetTextWidth = 0;

	PmCol EnableColor = PM_WHITE;
	PmCol DisableColor = PM_RGBA(200, 200, 200, 200);
	PmCol SliderColor = PM_WHITE;
	PmCol ComboColor = PM_WHITE;
	PmCol ComboTextColor = PM_WHITE;

	void SetFunc_AddLine(PM_FUNC_AddLine pFunc) {
		AddLine = pFunc;
	}
	void SetFunc_FillRect(PM_FUNC_FillRect pFunc) {
		FillRect = pFunc;
	}
	void SetFunc_AddText(PM_FUNC_AddText pFunc) {
		AddText = pFunc;
	}
	void SetFunc_GetTextWidth(PM_FUNC_GetTextWidth pFunc) {
		GetTextWidth = pFunc;
	}

	void AddBox(float x, float y, float w, float h, PmCol col) {
		AddLine(x, y, x + w, y, col);
		AddLine(x, y, x, y + h, col);
		AddLine(x + w, y, x + w, y + h, col);
		AddLine(x, y + h, x + w, y + h, col);
	}
	
	bool have_keyevt = false;
	PmKeyEvent keyevt = { 0 };
	void ProcessKeyEvent(PmKeyEvent KeyEvent) {
		if (VaildSelectEntryCount) {
			if (KeyEvent.ResultMap.map[PM_VK_DOWN]) {
				if (g_select_idx + 1 < VaildSelectEntryCount) {
					g_select_idx++;
				}
				else {
					g_select_idx = 0;
				}
			}
			else if (KeyEvent.ResultMap.map[PM_VK_UP]) {
				if (g_select_idx == 0) {
					g_select_idx = VaildSelectEntryCount - 1;
				}
				else {
					g_select_idx--;
				}
			}
			PmSelectEntry* pEntry = VaildSelectEntry[g_select_idx];
			if (KeyEvent.ResultMap.map[PM_VK_LEFT]) {
				if (pEntry->type == ET_Slider) {
					switch (pEntry->value_type) {
					case VT_INT:
					{
						int v = *pEntry->int_value - pEntry->int_single;
						if (v < pEntry->int_min)
							v = pEntry->int_min;
						*pEntry->int_value = v;
						break;
					}
					case VT_FLOAT:
					{
						float v = *pEntry->float_value - pEntry->float_single;
						if (v < pEntry->float_min)
							v = pEntry->float_min;
						*pEntry->float_value = v;
						break;
					}
					default:
						break;
					}
				}
				if (pEntry->type == ET_Combo) {
					int v = *pEntry->int_value - 1;
					if (v < 0)
						v = 0;
					*pEntry->int_value = v;
				}
				if (pEntry->type == ET_Check) {
					*pEntry->check = !*pEntry->check;
				}

			}
			else if (KeyEvent.ResultMap.map[PM_VK_RIGHT]) {
				if (pEntry->type == ET_Slider) {
					switch (pEntry->value_type) {
					case VT_INT:
					{
						int v = *pEntry->int_value + pEntry->int_single;
						if (v > pEntry->int_max)
							v = pEntry->int_max;
						*pEntry->int_value = v;
						break;
					}
					case VT_FLOAT:
					{
						float v = *pEntry->float_value + pEntry->float_single;
						if (v > pEntry->float_max)
							v = pEntry->float_max;
						*pEntry->float_value = v;
						break;
					}
					default:
						break;
					}
				}
				if (pEntry->type == ET_Combo) {
					if (pEntry->combo_count) {
						int v = *pEntry->int_value + 1;
						if (v > pEntry->combo_count - 1)
							v = pEntry->combo_count - 1;
						*pEntry->int_value = v;
					}
				}
				if (pEntry->type == ET_Check) {
					*pEntry->check = !*pEntry->check;
				}

			}
		}
		for (int i = 0; i < 256; i++) {
			if (BindHotKeyMap[i] && KeyEvent.ResultMap.map[i]) {
				PmSelectEntry* Entry = BindHotKeyMap[i];

				if (Entry->type == ET_Check) {
					*Entry->check = !*Entry->check;
				}
			}
		}

	}
	void PostKeyEvent(PmKeyEvent KeyEvent) {
		have_keyevt = 1;
		for (int i = 0; i < 256; i++) {
			keyevt.ResultMap.map[i] |= KeyEvent.ResultMap.map[i];
		}
	}

	float g_font_height = 14.f;
	PmRect g_MainFrame = { 0,0,200.f,500.f };
	void Init(float font_height, PmRect MainFrame) {
		g_font_height = font_height;
		g_MainFrame = MainFrame;
		SetVaildKey(PM_VK_UP);
		SetVaildKey(PM_VK_DOWN);
		SetVaildKey(PM_VK_LEFT);
		SetVaildKey(PM_VK_RIGHT);
	}

	void Begin() {
		VaildSelectEntryCount = 0;
		AllEntryCount = 0;
	}
	void MiddleText(const char* title, float font_height_scale, PmCol col) {
		PmSelectEntry* pEntry = &AllEntry[AllEntryCount];
		pm_zeromem(pEntry, sizeof(PmSelectEntry));
		pEntry->font_height_scale = font_height_scale;
		pEntry->type = ET_MIDDLE_TEXT;
		pEntry->Col = col;
		pm_strcpy(pEntry->text, title);
		pm_xor_data(pEntry->text, sizeof(pEntry->text), PM_XORKEY);

		AllEntryCount++;
	}
	void Text(const char* title, float font_height_scale, PmCol col) {
		PmSelectEntry* pEntry = &AllEntry[AllEntryCount];
		pm_zeromem(pEntry, sizeof(PmSelectEntry));
		pEntry->font_height_scale = font_height_scale;
		pEntry->type = ET_TEXT;
		pEntry->Col = col;
		pm_strcpy(pEntry->text, title);
		pm_xor_data(pEntry->text, sizeof(pEntry->text), PM_XORKEY);

		AllEntryCount++;
	}
	void Check(const char* title, bool* gptr, PmCol col, float scale) {
		PmSelectEntry* pEntry = &AllEntry[AllEntryCount];
		pm_zeromem(pEntry, sizeof(PmSelectEntry));
		pEntry->font_height_scale = scale;
		pEntry->type = ET_Check;
		pm_strcpy(pEntry->text, title);
		pm_xor_data(pEntry->text, sizeof(pEntry->text), PM_XORKEY);
		pEntry->Col = col;
		

		pEntry->check = gptr;
		AllEntryCount++;

		VaildSelectEntry[VaildSelectEntryCount] = pEntry;
		VaildSelectEntryCount++;
	}
	void CheckAndBind(const char* title, bool* gptr, int vkey, PmCol col) {
		SetVaildKey(vkey);
		BindHotKeyMap[vkey] = &AllEntry[AllEntryCount];
		Check(title, gptr, col);
		
	}
	void Slider(const char* title, int* gptr, int single, int min, int max, PmCol col) {
		PmSelectEntry* pEntry = &AllEntry[AllEntryCount];
		pm_zeromem(pEntry, sizeof(PmSelectEntry));
		pEntry->font_height_scale = 1.f;
		pEntry->type = ET_Slider;
		pm_strcpy(pEntry->text, title);
		pm_xor_data(pEntry->text, sizeof(pEntry->text), PM_XORKEY);
		pEntry->Col = col;

		pEntry->value_type = VT_INT;
		pEntry->int_value = gptr;
		pEntry->int_single = single;
		pEntry->int_min = min;
		pEntry->int_max = max;
		AllEntryCount++;

		VaildSelectEntry[VaildSelectEntryCount] = pEntry;
		VaildSelectEntryCount++;
	}
	void Slider(const char* title, float* gptr, float single, float min, float max, PmCol col) {
		PmSelectEntry* pEntry = &AllEntry[AllEntryCount];
		pm_zeromem(pEntry, sizeof(PmSelectEntry));
		pEntry->font_height_scale = 1.f;
		pEntry->type = ET_Slider;
		pm_strcpy(pEntry->text, title);
		pm_xor_data(pEntry->text, sizeof(pEntry->text), PM_XORKEY);
		pEntry->Col = col;

		pEntry->value_type = VT_FLOAT;
		pEntry->float_value = gptr;
		pEntry->float_single = single;
		pEntry->float_min = min;
		pEntry->float_max = max;
		AllEntryCount++;

		VaildSelectEntry[VaildSelectEntryCount] = pEntry;
		VaildSelectEntryCount++;
	}
	void Combo(const char* title, int* gptr, const char** combo_list, int combo_count, PmCol col) {
		PmSelectEntry* pEntry = &AllEntry[AllEntryCount];
		pm_zeromem(pEntry, sizeof(PmSelectEntry));
		pEntry->font_height_scale = 1.f;
		pEntry->type = ET_Combo;
		pm_strcpy(pEntry->text, title);
		pm_xor_data(pEntry->text, sizeof(pEntry->text), PM_XORKEY);
		pEntry->Col = col;

		pEntry->int_value = gptr;
		pEntry->combo_list = combo_list;
		pEntry->combo_count = combo_count;
		AllEntryCount++;

		VaildSelectEntry[VaildSelectEntryCount] = pEntry;
		VaildSelectEntryCount++;
	}

	char tempstr[300] = { 0 };
	void RenderSimpleGui() {
		FillRect(g_MainFrame.x, g_MainFrame.y, g_MainFrame.w, g_MainFrame.h, PM_RGBA(0, 0, 0, 120));
		AddBox(g_MainFrame.x, g_MainFrame.y, g_MainFrame.w, g_MainFrame.h, PM_RGBA(87, 250, 255, 233));
		PmRect PaintRect = PmRect(g_MainFrame.x + 3.f, g_MainFrame.y + 3.f, g_MainFrame.w - 6.f, g_MainFrame.h - 6.f);
		float px = PaintRect.x;
		float py = PaintRect.y;

		py += 3.f;
		for (int i = 0; i < AllEntryCount; i++) {
			PmSelectEntry* pEntry = &AllEntry[i];
			float font_height = g_font_height * pEntry->font_height_scale;

			if (VaildSelectEntry[g_select_idx] == pEntry) {
				FillRect(px, py - 1.f - 2.f, PaintRect.w, font_height + 4.f , PM_RGBA(87, 250, 255, 120));
			}
			switch (pEntry->type)
			{
			case ET_MIDDLE_TEXT:
			{
				pm_xor_data(pEntry->text, sizeof(pEntry->text), PM_XORKEY);
				float textsz = GetTextWidth(pEntry->text, font_height);
				float startx = PaintRect.x + (PaintRect.w / 2 - textsz / 2);
				AddText(startx, py, font_height, pEntry->Col, pEntry->text);
				pm_xor_data(pEntry->text, sizeof(pEntry->text), PM_XORKEY);

				py += font_height + 6.f;
				break;
			}
			case ET_TEXT:
			{
				pm_xor_data(pEntry->text, sizeof(pEntry->text), PM_XORKEY);
				//float textsz = GetTextWidth(pEntry->text, font_height);
				//float startx = PaintRect.x + (PaintRect.w / 2 - textsz / 2);
				AddText(px, py, font_height, pEntry->Col, pEntry->text);
				pm_xor_data(pEntry->text, sizeof(pEntry->text), PM_XORKEY);

				py += font_height + 6.f;
				break;
			}
			case ET_Check:
			{
				pm_xor_data(pEntry->text, sizeof(pEntry->text), PM_XORKEY);

				float box_wid = font_height * 0.875 + 4.f;
				//float box_off = font_height * 0.125;
				float text_off = font_height;

				PmRect box_rect = PmRect(px + PaintRect.w - box_wid - 2.f, py - 2.f, box_wid , box_wid);
				AddBox(box_rect.x, box_rect.y, box_wid, box_wid, PM_XRGB(255, 140, 92));
				AddBox(box_rect.x + 1.f, box_rect.y + 1.f, box_wid - 2.f, box_wid - 2.f, PM_XRGB(255, 140, 92));
				if (*pEntry->check) {
					float ico_wid = box_wid * 0.5;
					float ico_off = box_wid * 0.25;
					PmRect ico_rect = PmRect(box_rect.x + ico_off, box_rect.y + ico_off, ico_wid, ico_wid);
					FillRect(ico_rect.x, ico_rect.y, ico_rect.w, ico_rect.h, PM_YELLOW);
					AddBox(ico_rect.x, ico_rect.y, ico_rect.w, ico_rect.h, PM_BLACK);
				}

				AddText(px, py, font_height, *pEntry->check ? EnableColor : DisableColor, pEntry->text);

				pm_xor_data(pEntry->text, sizeof(pEntry->text), PM_XORKEY);
				py += font_height + 6.f;
				break;
			}
			case ET_Slider:
			{
				pm_xor_data(pEntry->text, sizeof(pEntry->text), PM_XORKEY);
				
				AddText(px, py, font_height, pEntry->Col, pEntry->text);

				pm_xor_data(pEntry->text, sizeof(pEntry->text), PM_XORKEY);

				py += font_height + 6.f;
				break;
			}
			case ET_Combo:
			{
				pm_xor_data(pEntry->text, sizeof(pEntry->text), PM_XORKEY);
				AddText(px, py, font_height, pEntry->Col, pEntry->text);

				pm_xor_data(pEntry->text, sizeof(pEntry->text), PM_XORKEY);

				py += font_height + 6.f;
				break;
			}
			default:
				break;
			}
		}


	}

	void End() {
		if (have_keyevt) {
			ProcessKeyEvent(keyevt);
			have_keyevt = 0;
			pm_zeromem(&keyevt, sizeof(keyevt));
		}
		RenderSimpleGui();
	}

}
