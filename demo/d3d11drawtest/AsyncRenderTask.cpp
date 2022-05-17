/*
Author: CSlime
Github: https://github.com/cs1ime
*/

#include "AsyncRenderTask.h"

namespace TaskRecv {
	void InitNewTaskBuffer(TaskBuffer &Buffer) {
		Buffer.ele_count = 0;
		Buffer.have_render_data = false;
		Buffer.lock = 0;
		Buffer.Share.PlaneHeight = 0;
		Buffer.Share.PlaneWidth = 0;
		Buffer.Share.WindowOffsetX = 0;
		Buffer.Share.WindowOffsetY = 0;
		Buffer.Share.WindowHeight = 0;
		Buffer.Share.WindowWidth = 0;
		Buffer.SizeOfBuffer = sizeof(Buffer);
		Buffer.Magic = 'LUXE';
	}
	void UpdateShareData(TaskBuffer& Buffer, TaskShareData& Share) {
		Buffer.Share = Share;
	}

	void RenderTask(TaskBuffer& Task, TaskDispatchRoutine Dispatch) {
		PUCHAR pData = &Task.Data;
		PULONG pSec_ele_count = (PULONG)(pData + DRAW_BUFFER_NEED_SIZE);
		PUCHAR pSecData = pData + DRAW_BUFFER_NEED_SIZE + sizeof(ULONG);
		for (int i = 0; i < *pSec_ele_count; i++) {
			TaskElement* ele = (TaskElement*)pSecData;
			Dispatch(ele);
			pSecData += ele->SizeOfBlock;
		}
	}
	bool ProcessTask(TaskBuffer& Task, TaskDispatchRoutine Dispatch) {
		if (Task.have_render_data == false) {
			if (Task.have_sec_render_data) {
				RenderTask(Task, Dispatch);
				return true;
			}
			else {
				return false;
			}
		}
			
		if (InterlockedCompareExchange(&Task.lock, 1, 0) == 1)
			return false;
		PUCHAR pData = &Task.Data;

		PUCHAR pSecData = pData + DRAW_BUFFER_NEED_SIZE + sizeof(ULONG);
		PULONG pSec_ele_count = (PULONG)(pData + DRAW_BUFFER_NEED_SIZE);
		*pSec_ele_count = Task.ele_count;
		memcpy(pSecData, pData, Task.SizeOfBuffer);
		
		Task.have_render_data = false;
		Task.have_sec_render_data = true;
		Task.lock = 0;

		RenderTask(Task, Dispatch);

		

		
	}

}
namespace TaskSender {
	void Begin(TaskSenderCache& Cache) {
		Cache.SizeOfBuffer = 0;
		Cache.ele_count = 0;
	}
	bool PresentToTask(TaskBuffer* Task, TaskSenderCache& Cache) {
		if (Task->have_render_data)
			return false;
		if (InterlockedCompareExchange(&Task->lock, 1, 0) == 1)
			return false;

		Task->SizeOfBuffer = Cache.SizeOfBuffer;
		Task->ele_count = Cache.ele_count;
		memcpy(&Task->Data, &Cache.Data, Task->SizeOfBuffer);
		Task->have_render_data = true;

		Task->lock = 0;

		return true;
	}

	bool CheckTask(TaskSenderCache& ctx) {
		if (DRAW_BUFFER_NEED_SIZE - ctx.SizeOfBuffer < ELEMENT_BLOCK_MAX_SIZE) {
			return false;
		}
		return true;
	}
	void AddLine(TaskSenderCache& ctx, float x1, float y1, float x2, float y2, DWORD color) {
		if (!CheckTask(ctx))
			return;
		TaskElement* pEle = (TaskElement*)((PUCHAR)&ctx.Data + ctx.SizeOfBuffer);

		pEle->Type = ELEMENT_LINE;
		ElementLine* ele = (ElementLine*)&pEle->Data;
		ele->x1 = x1;
		ele->x2 = x2;
		ele->y1 = y1;
		ele->y2 = y2;
		ele->col = color;

		pEle->SizeOfBlock = TaskElementHeaderSize + sizeof(ElementLine);
		ctx.ele_count++;
		ctx.SizeOfBuffer += pEle->SizeOfBlock;
	}
	void AddBox(TaskSenderCache& ctx, float x, float y, float w, float h, DWORD col) {
		AddLine(ctx, x, y, x + w, y, col);//top
		AddLine(ctx, x, y, x, y + h, col);//left
		AddLine(ctx, x + w, y, x + w, y + h, col);//right
		AddLine(ctx, x, y + h, x + w, y + h, col);//bottom
	}
	void AddCornBox(TaskSenderCache& ctx, float x, float y, float w, float h, DWORD col) {
		float xm = w / 3;
		float ym = h / 3;

		AddLine(ctx, x, y, x + xm, y, col); AddLine(ctx, x, y, x, y + ym, col);
		AddLine(ctx, x, y + h, x + xm, y + h, col); AddLine(ctx, x, y + h, x, y + h - ym, col);
		AddLine(ctx, x + w, y, x + w - xm, y, col); AddLine(ctx, x + w, y, x + w, y + ym, col);
		AddLine(ctx, x + w, y + h, x + w - xm, y + h, col); AddLine(ctx, x + w, y + h, x + w, y + h - ym, col);
	}
	void AddCircle(TaskSenderCache& ctx, float centerx, float centery, float rad, long pointCount, DWORD col) {
		if (!CheckTask(ctx))
			return;
		TaskElement* pEle = (TaskElement*)((PUCHAR)&ctx.Data + ctx.SizeOfBuffer);

		pEle->Type = ELEMENT_CIRCLE;
		ElementCircle* ele = (ElementCircle*)&pEle->Data;
		ele->centerx = centerx;
		ele->centery = centery;
		ele->rad = rad;
		ele->pointCount = pointCount;
		ele->col = col;

		pEle->SizeOfBlock = TaskElementHeaderSize + sizeof(ElementCircle);
		ctx.ele_count++;
		ctx.SizeOfBuffer += pEle->SizeOfBlock;
	}
	void AddSoildCircle(TaskSenderCache& ctx, float centerx, float centery, float rad, long pointCount, DWORD col) {
		if (!CheckTask(ctx))
			return;
		TaskElement* pEle = (TaskElement*)((PUCHAR)&ctx.Data + ctx.SizeOfBuffer);

		pEle->Type = ELEMENT_SOILDCIRCLE;
		ElementSoildCircle* ele = (ElementSoildCircle*)&pEle->Data;
		ele->centerx = centerx;
		ele->centery = centery;
		ele->rad = rad;
		ele->pointCount = pointCount;
		ele->col = col;

		pEle->SizeOfBlock = TaskElementHeaderSize + sizeof(ElementSoildCircle);
		ctx.ele_count++;
		ctx.SizeOfBuffer += pEle->SizeOfBlock;
	}
	void FillRect(TaskSenderCache& ctx, float x, float y, float w, float h, DWORD col) {
		if (!CheckTask(ctx))
			return;
		TaskElement* pEle = (TaskElement*)((PUCHAR)&ctx.Data + ctx.SizeOfBuffer);

		pEle->Type = ELEMENT_SOILDRECT;
		ElementSoildRect* ele = (ElementSoildRect*)&pEle->Data;
		ele->x = x;
		ele->y = y;
		ele->w = w;
		ele->h = h;
		ele->col = col;

		pEle->SizeOfBlock = TaskElementHeaderSize + sizeof(ElementSoildRect);
		ctx.ele_count++;
		ctx.SizeOfBuffer += pEle->SizeOfBlock;
	}
	void AddSoildTriangle(TaskSenderCache& ctx, float x1, float y1, float x2, float y2, float x3, float y3, DWORD col) {
		if (!CheckTask(ctx))
			return;
		TaskElement* pEle = (TaskElement*)((PUCHAR)&ctx.Data + ctx.SizeOfBuffer);

		pEle->Type = ELEMENT_SOILDTRIANGLE;
		ElementSoildTriangle* ele = (ElementSoildTriangle*)&pEle->Data;
		ele->x1 = x1;
		ele->y1 = y1;
		ele->x2 = x2;
		ele->y2 = y2;
		ele->x3 = x3;
		ele->y3 = y3;
		ele->col = col;

		pEle->SizeOfBlock = TaskElementHeaderSize + sizeof(ElementSoildTriangle);
		ctx.ele_count++;
		ctx.SizeOfBuffer += pEle->SizeOfBlock;
	}
	void AddString(TaskSenderCache& ctx, float x, float y, LPCSTR Str, ULONG Size, DWORD col) {
		if (!CheckTask(ctx))
			return;
		TaskElement* pEle = (TaskElement*)((PUCHAR)&ctx.Data + ctx.SizeOfBuffer);

		pEle->Type = ELEMENT_STRING;
		ElementString* ele = (ElementString*)&pEle->Data;
		int len = strlen(Str);
		if (len > ELEMENT_STRING_MAX_LENGTH) {
			len = ELEMENT_STRING_MAX_LENGTH;
		}
		PCHAR pstr = (PCHAR)&ele->str;
		memcpy(pstr, Str, len);
		pstr[len] = 0;
		ele->col = col;
		ele->x = x;
		ele->y = y;
		ele->size = Size;

		pEle->SizeOfBlock = TaskElementHeaderSize + sizeof(ElementString) + len;
		ctx.ele_count++;
		ctx.SizeOfBuffer += pEle->SizeOfBlock;

	}
	void AddStringStroke(TaskSenderCache& ctx, float x, float y, LPCSTR Str, ULONG Size, DWORD col, DWORD StrokeColor) {
		if (!CheckTask(ctx))
			return;
		TaskElement* pEle = (TaskElement*)((PUCHAR)&ctx.Data + ctx.SizeOfBuffer);

		pEle->Type = ELEMENT_STROKESTRING;
		ElementStrokeString* ele = (ElementStrokeString*)&pEle->Data;
		int len = strlen(Str);
		if (len > ELEMENT_STRING_MAX_LENGTH) {
			len = ELEMENT_STRING_MAX_LENGTH;
		}
		PCHAR pstr = (PCHAR)&ele->str;
		memcpy(pstr, Str, len);
		pstr[len] = 0;
		ele->col = col;
		ele->StrokeCol = StrokeColor;
		ele->x = x;
		ele->y = y;
		ele->size = Size;

		pEle->SizeOfBlock = TaskElementHeaderSize + sizeof(ElementStrokeString) + len;
		ctx.ele_count++;
		ctx.SizeOfBuffer += pEle->SizeOfBlock;
	}

}
