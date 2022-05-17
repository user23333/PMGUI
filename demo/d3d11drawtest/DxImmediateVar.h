/*
Author: CSlime
Github: https://github.com/cs1ime
*/
#pragma once

#ifndef _DX_IMMEDIATE_H_
#define _DX_IMMEDIATE_H_

#include "windows.h"

typedef struct _CVector2 {
	float x, y;
	_CVector2() { this->x = this->y = 0.0f; }
	_CVector2(float _x, float _y) { this->x = _x; this->y = _y; }
}CVector2, * PCVector2;
typedef struct _CVertex {
	float x, y;
	float u, v;
	DWORD col;
	_CVertex() { this->col = 0; this->x = 0.f; this->y = 0.f; this->u = 0.f; this->v = 0.f; }
	_CVertex(float _x, float _y, float _u, float _v, DWORD col) {
		this->col = col;
		this->x = _x;
		this->y = _y;
		this->u = _u;
		this->v = _v;
	}
	_CVertex(float _x, float _y, DWORD col) {
		this->col = col;
		this->x = _x;
		this->y = _y;
		this->u = 0.f;
		this->v = 0.f;

	}
}CVertex, * PCVertex;


#define MAX_FONT_SIZE (30)

struct CFontStroke {
	WCHAR Char;
	UINT Size;
	float ox;
	float oy;
	void* TexId;
	void* Tex2D;
};
struct CFontGlyph {
	UINT Char;
	UINT Size;
	float Left;
	float Top;
	float Width;
	float Height;
	void* TexId;
	void* Tex2D;
};
struct CSizeMap {
	USHORT SizeIndexMap[MAX_FONT_SIZE];
};
struct CFontInfo {
	UINT Char;
	int Size;
	int Left;
	int Top;
	int Width;
	int Height;
	unsigned char* bitmap;
};
struct CLineInfo {
	float sx;
	float sy;
	float ex;
	float ey;
	DWORD col;
};
struct CDrawList {
	UINT vtx_size;
	UINT idx_size;
	void* tex_id;
	ULONG primitive_type;
};

#define MAX_GLYPH_COUNT (10000)
extern CFontGlyph g_GlyphList[MAX_GLYPH_COUNT];
extern int g_GlyphListSize;
extern CSizeMap g_GlyphMap[0x10000];
extern CFontInfo g_FontList[0x10000];
extern bool g_FontVaildList[0x10000];
#define MAX_DRAWLIST_COUNT (30000)
extern CDrawList g_DrawList[MAX_DRAWLIST_COUNT];
extern int g_DrawListSize;
extern UINT g_VBufferSize;
extern UINT g_IBufferSize;
#define MAX_VTX_BUFFER_COUNT (100000)
#define MAX_IDX_BUFFER_COUNT (100000)
extern CVertex g_vtx_buffer[MAX_VTX_BUFFER_COUNT];
extern int g_vtx_buffer_size;
extern DWORD g_idx_buffer[MAX_IDX_BUFFER_COUNT];
extern int g_idx_buffer_size;

void DxUTF8ToUnicode(LPCSTR utf8, LPWSTR uni, SIZE_T MaxCount);

VOID PushVtx(CVertex* vtx);
VOID PushIdx(DWORD idx);
VOID PushDrawList(CDrawList* draw);

#endif // !_DX_IMMEDIATE_H_

