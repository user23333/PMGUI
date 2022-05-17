/*
Author: CSlime
Github: https://github.com/cs1ime
*/
#include "DxImmediateVar.h"

CFontGlyph g_GlyphList[MAX_GLYPH_COUNT];
int g_GlyphListSize = 0;
CSizeMap g_GlyphMap[0x10000];
CFontInfo g_FontList[0x10000];
bool g_FontVaildList[0x10000];
CDrawList g_DrawList[MAX_DRAWLIST_COUNT];
int g_DrawListSize = 0;
UINT g_VBufferSize = 5000;
UINT g_IBufferSize = 10000;
CVertex g_vtx_buffer[MAX_VTX_BUFFER_COUNT];
int g_vtx_buffer_size = 0;
DWORD g_idx_buffer[MAX_IDX_BUFFER_COUNT];
int g_idx_buffer_size = 0;

void DxUTF8ToUnicode(LPCSTR utf8, LPWSTR uni, SIZE_T MaxCount) {
	unsigned char v;
	int unidx = 0;
	for (int i = 0; v = utf8[i]; i++) {
		if ((v & 0b10000000) == 0) {
			uni[unidx] = v;
		}
		else if ((v & 0b11100000) == 0b11000000) {
			unsigned short c = (unsigned short)((v & 0b00011111)) << 6;
			v = utf8[++i];
			if ((v & 0b11000000) == 0b10000000) {
				c |= (v & 0b00111111);
				uni[unidx] = c;
			}
			else
				continue;
		}
		else if ((v & 0b11110000) == 0b11100000) {
			unsigned short c = (unsigned short)((v & 0b00001111)) << 12;
			v = utf8[++i];
			if ((v & 0b11000000) == 0b10000000) {
				c |= (unsigned short)((v & 0b00111111)) << 6;
				v = utf8[++i];
				if ((v & 0b11000000) == 0b10000000) {
					c |= (unsigned short)((v & 0b00111111));
					uni[unidx] = c;
				}
				else
					continue;
			}
			else
				continue;
		}
		else
			continue;
		unidx++;
		if (unidx >= MaxCount)
			break;
	}
	uni[unidx] = 0;
	return;
}

VOID PushVtx(CVertex* vtx) {
	if (g_vtx_buffer_size >= MAX_VTX_BUFFER_COUNT)
		return;
	g_vtx_buffer[g_vtx_buffer_size++] = *vtx;
}
VOID PushIdx(DWORD idx) {
	if (g_idx_buffer_size >= MAX_IDX_BUFFER_COUNT)
		return;
	g_idx_buffer[g_idx_buffer_size++] = idx;
}
VOID PushDrawList(CDrawList* draw) {
	if (g_DrawListSize >= MAX_DRAWLIST_COUNT)
		return;
	g_DrawList[g_DrawListSize++] = *draw;
}
