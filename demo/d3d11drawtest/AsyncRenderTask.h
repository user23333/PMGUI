/*
Author: CSlime
Github: https://github.com/cs1ime
*/

#pragma once

#ifndef _ASYNC_RENDER_TASK_
#define _ASYNC_RENDER_TASK_

#include "windows.h"

#define DRAW_BUFFER_NEED_SIZE (0x100000)
#define ELEMENT_STRING_MAX_LENGTH (260)
#define ELEMENT_BLOCK_MAX_SIZE (300)

struct TaskShareData {
	ULONG PlaneWidth;
	ULONG PlaneHeight;
	ULONG WindowOffsetX;
	ULONG WindowOffsetY;
	ULONG WindowWidth;
	ULONG WindowHeight;
};
struct TaskBuffer {
	ULONG Magic;
	TaskShareData Share;
	bool have_render_data;
	bool have_sec_render_data;
	ULONG lock;
	ULONG ele_count;
	ULONG SizeOfBuffer;
	UCHAR Data;
};
struct ReleasedTaskBuffer {
	TaskBuffer Task;
	UCHAR DrawBuffer[DRAW_BUFFER_NEED_SIZE];
	ULONG Second_ele_count;
	UCHAR SecondDrawBuffer[DRAW_BUFFER_NEED_SIZE];
};
struct TaskSenderCache {
	ULONG ele_count;
	ULONG SizeOfBuffer;
	UCHAR Data;
};
struct ReleasedTaskSenderCache {
	TaskSenderCache Cache;
	UCHAR DrawBuffer[DRAW_BUFFER_NEED_SIZE];
};
#define TaskElementHeaderSize ((long)&((TaskElement *)0)->Data)
struct TaskElement {
	ULONG SizeOfBlock;
	ULONG Type;
	UCHAR Data;
};
enum ElementType {
	ELEMENT_LINE = 1,
	ELEMENT_CIRCLE,
	ELEMENT_STRING,
	ELEMENT_STROKESTRING,
	ELEMENT_SOILDRECT,
	ELEMENT_SOILDTRIANGLE, 
	ELEMENT_SOILDCIRCLE,

};
struct ElementLine {
	float x1;
	float y1;
	float x2;
	float y2;
	DWORD col;
};
struct ElementCircle {
	float centerx;
	float centery;
	float rad;
	long pointCount;
	DWORD col;
};
struct ElementString {
	float x;
	float y;
	ULONG size;
	DWORD col;
	ULONG Isu8;
	CHAR str;
};
struct ElementStrokeString {
	float x;
	float y;
	ULONG size;
	DWORD col;
	DWORD StrokeCol;
	ULONG Isu8;
	CHAR str;
};
struct ElementSoildRect {
	float x;
	float y;
	float w;
	float h;
	DWORD col;
};
struct ElementSoildTriangle {
	float x1;
	float y1;
	float x2;
	float y2;
	float x3;
	float y3;
	DWORD col;
};
struct ElementSoildCircle {
	float centerx;
	float centery;
	float rad;
	long pointCount;
	DWORD col;
};

typedef void(*TaskDispatchRoutine)(TaskElement*);

namespace TaskRecv {
	void InitNewTaskBuffer(TaskBuffer &Buffer);
	void UpdateShareData(TaskBuffer& Buffer, TaskShareData& Share);
	bool ProcessTask(TaskBuffer& Task, TaskDispatchRoutine Dispatch);
}
namespace TaskSender {
	void Begin(TaskSenderCache& Cache);
	bool PresentToTask(TaskBuffer* Task, TaskSenderCache& Cache);

	void AddLine(TaskSenderCache& ctx, float x1, float y1, float x2, float y2, DWORD color);
	void AddBox(TaskSenderCache& ctx, float x, float y, float w, float h, DWORD col);
	void AddCornBox(TaskSenderCache& ctx, float x, float y, float w, float h, DWORD col);
	void AddCircle(TaskSenderCache& ctx, float centerx, float centery, float rad, long pointCount, DWORD col);
	void AddSoildCircle(TaskSenderCache& ctx, float centerx, float centery, float rad, long pointCount, DWORD col);
	void FillRect(TaskSenderCache& ctx, float x, float y, float w, float h, DWORD col);
	void AddSoildTriangle(TaskSenderCache& ctx, float x1, float y1, float x2, float y2, float x3, float y3, DWORD col);
	void AddString(TaskSenderCache& ctx, float x, float y, LPCSTR Str, ULONG Size, DWORD col);
	void AddStringStroke(TaskSenderCache& ctx, float x, float y, LPCSTR Str, ULONG Size, DWORD col, DWORD StrokeColor);


}

#endif // !_ASYNC_RENDER_TASK_RECV_

