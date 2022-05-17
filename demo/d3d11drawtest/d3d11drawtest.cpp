/*
Author: CSlime
Github: https://github.com/cs1ime
*/

// d3d11drawtest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "d3d11.h"
#include <d3dx11.h>
#include <xnamath.h>
#include "DX11ImmediateRender.h"
#include "PmGuiCore.h"
#include "PmWin32Event.h"
#include "AsyncRenderTask.h"


struct cbPerObject { XMMATRIX  WVP; };

cbPerObject cbPerObj;

//Vertex Structure and Vertex Layout (Input Layout) - Overloaded Vertex Structure
typedef struct _Vector2
{
	float     x, y;
	_Vector2() { x = y = 0.0f; }
	_Vector2(float _x, float _y) { x = _x; y = _y; }
	float  operator[] (size_t idx) const { return (&x)[idx]; }    // We very rarely use this [] operator, the assert overhead is fine.
	float& operator[] (size_t idx) { return (&x)[idx]; }    // We very rarely use this [] operator, the assert overhead is fine.
}Vector2, *PVector2;
struct Vertex {
	Vertex() {}
	Vertex(float x, float y, float z, float cr, float cg, float cb, float ca) : pos(x, y, z), color(cr, cg, cb, ca) {}
	XMFLOAT3 pos;
	XMFLOAT4 color;
};

D3D11_INPUT_ELEMENT_DESC g_layout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

// Data
static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;
static ID3D10Blob*				g_pVSBlob = 0;
static ID3D10Blob*				g_pPSBlob = 0;
static ID3D11VertexShader*		g_pVS = 0;
static ID3D11PixelShader*		g_pPS = 0;
static ID3D11InputLayout*		g_InputLayout = 0;
static ID3D11Buffer*			g_VtxBuffer = 0;
static ID3D11Buffer*			g_IdxBuffer = 0;
static ID3D11Buffer*			g_WVPBuffer = 0;
static ID3D11RasterizerState*	g_WireFrame = 0;
static ID3D11Texture2D*			g_depthStencilBuffer = 0;
static ID3D11DepthStencilView*	g_depthStencilView = 0;
static ID3D11DepthStencilState* g_pDepthStencilState = NULL;

FLOAT width = 1280.0f;
FLOAT height = 800.0f;

void CreateRenderTarget()
{
	ID3D11Texture2D* BackBuffer;
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
	g_pd3dDevice->CreateRenderTargetView(BackBuffer, NULL, &g_mainRenderTargetView);
	BackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}
bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			//CleanupRenderTarget();
			//g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			//CreateRenderTarget();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}
void InitD3D11() {

	D3DX11CompileFromFileW(L"Effects.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &g_pVSBlob, 0, 0);
	D3DX11CompileFromFileW(L"Effects.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &g_pPSBlob, 0, 0);

	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.DepthEnable = false;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	desc.StencilEnable = false;
	desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	desc.BackFace = desc.FrontFace;
	g_pd3dDevice->CreateDepthStencilState(&desc, &g_pDepthStencilState);
	g_pd3dDeviceContext->OMSetDepthStencilState(g_pDepthStencilState, 0);

	g_pd3dDevice->CreateVertexShader(g_pVSBlob->GetBufferPointer(), g_pVSBlob->GetBufferSize(), 0, &g_pVS);
	g_pd3dDevice->CreatePixelShader(g_pPSBlob->GetBufferPointer(), g_pPSBlob->GetBufferSize(), 0, &g_pPS);

	g_pd3dDevice->CreateInputLayout(g_layout, 2, g_pVSBlob->GetBufferPointer(), g_pVSBlob->GetBufferSize(), &g_InputLayout);
	g_pd3dDeviceContext->IASetInputLayout(g_InputLayout);
	
	g_pd3dDeviceContext->VSSetShader(g_pVS, 0, 0);
	g_pd3dDeviceContext->PSSetShader(g_pPS, 0, 0);

	Vertex v[] = {
			Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
			Vertex(-1.0f, +1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f),
			Vertex(+1.0f, +1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f),
			Vertex(+1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f),
			Vertex(-1.0f, -1.0f, +1.0f, 0.0f, 1.0f, 1.0f, 1.0f),
			Vertex(-1.0f, +1.0f, +1.0f, 1.0f, 1.0f, 1.0f, 1.0f),
			Vertex(+1.0f, +1.0f, +1.0f, 1.0f, 0.0f, 1.0f, 1.0f),
			Vertex(+1.0f, -1.0f, +1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
	};
	D3D11_SUBRESOURCE_DATA vtxinitdata;
	ZeroMemory(&vtxinitdata, sizeof(vtxinitdata));
	vtxinitdata.pSysMem = &v;

	D3D11_BUFFER_DESC vertex_desc;
	RtlZeroMemory(&vertex_desc, sizeof(vertex_desc));
	vertex_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_desc.ByteWidth = sizeof(Vertex) * 8;
	vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_desc.CPUAccessFlags = 0;
	vertex_desc.MiscFlags = 0;
	g_pd3dDevice->CreateBuffer(&vertex_desc, &vtxinitdata, &g_VtxBuffer);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pd3dDeviceContext->IASetVertexBuffers(0, 1, &g_VtxBuffer, &stride, &offset);

	DWORD indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};
	D3D11_SUBRESOURCE_DATA idxinitdata;
	ZeroMemory(&idxinitdata, sizeof(idxinitdata));
	idxinitdata.pSysMem = &indices;

	D3D11_BUFFER_DESC index_desc;
	RtlZeroMemory(&index_desc, sizeof(index_desc));
	index_desc.Usage = D3D11_USAGE_DEFAULT;
	index_desc.ByteWidth = sizeof(DWORD) * 3 * 12;
	index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	index_desc.CPUAccessFlags = 0;
	index_desc.MiscFlags = 0;
	g_pd3dDevice->CreateBuffer(&index_desc, &idxinitdata, &g_IdxBuffer);
	
	g_pd3dDeviceContext->IASetIndexBuffer(g_IdxBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//D3D11_PRIMITIVE_TOPOLOGY_LINELIST

	D3D11_VIEWPORT vp;
	ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = 1280.0f;
	vp.Height = 800.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	g_pd3dDeviceContext->RSSetViewports(1, &vp);

	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	float L = 0;
	float R = 0 + width;
	float T = 0;
	float B = 0 + height;
	float mvp[4][4] =
	{
		{ 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
		{ 0.0f,         2.0f / (T - B),     0.0f,       0.0f },
		{ 0.0f,         0.0f,           0.5f,       0.0f },
		{ (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
	};
	D3D11_SUBRESOURCE_DATA initdata;
	ZeroMemory(&initdata, sizeof(initdata));
	initdata.pSysMem = &mvp;
	g_pd3dDevice->CreateBuffer(&cbbd, &initdata, &g_WVPBuffer);
	g_pd3dDeviceContext->VSSetConstantBuffers(0, 1, &g_WVPBuffer);

	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	g_pd3dDevice->CreateRasterizerState(&wfdesc, &g_WireFrame);
	g_pd3dDeviceContext->RSSetState(g_WireFrame);
}
void DrawLine(Vector2 Start, Vector2 End, DWORD color) {
	ID3D11DeviceContext *ctx = g_pd3dDeviceContext;


	D3D11_MAPPED_SUBRESOURCE vtx_resource;
	if (ctx->Map(g_VtxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource) != S_OK)
		return;
	printf("draw\n");
	Vertex *vtx = (Vertex *)vtx_resource.pData;

	XMFLOAT4 fcolor;
	
	fcolor.w = 1.0f;
	fcolor.x = 1.0f;
	fcolor.y = 1.0f;
	fcolor.z = 1.0f;

	vtx[0].color = fcolor;
	vtx[0].pos.x = Start.x;
	vtx[0].pos.y = Start.y;
	vtx[0].pos.z = 1.0f;

	vtx[1].color = fcolor;
	vtx[1].pos.x = End.x;
	vtx[1].pos.y = End.y;
	vtx[1].pos.z = 1.0f;

	ctx->Unmap(g_VtxBuffer, 0);
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	ctx->Draw(2, 0);

}

VOID FnAddText(float x, float y, float size, int col, const char* text) {
	CRender::DrawStringStrokeA(x, y, text, (ULONG)size, col);
}
float FnGetSize(const char* text, float size) {
	return CRender::GetStringPixelLengthA(text, (ULONG)size);
}
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)

HWND hwnd = 0;

PmVec2 GetMousePos() {
	POINT point = { 0 };
	GetCursorPos(&point);
	ScreenToClient(hwnd, &point);

	return PmVec2((float)point.x, (float)point.y);
}

void DrawDispatch(TaskElement* ele) {
	switch (ele->Type)
	{
	case ELEMENT_LINE: {
		auto e = (ElementLine*)&ele->Data;
		CRender::DrawLine(e->x1, e->y1, e->x2, e->y2, e->col);
		break;
	}
	case ELEMENT_CIRCLE: {
		auto e = (ElementCircle*)&ele->Data;
		CRender::DrawCircle(e->centerx, e->centery, e->rad, e->pointCount, e->col);
		break;
	}
	case ELEMENT_SOILDCIRCLE: {
		auto e = (ElementSoildCircle*)&ele->Data;
		CRender::DrawSoildCircle(e->centerx, e->centery, e->rad, e->pointCount, e->col);
		break;
	}
	case ELEMENT_SOILDRECT: {
		auto e = (ElementSoildRect*)&ele->Data;
		CRender::FillRect(e->x, e->y, e->w, e->h, e->col);
		break;
	}
	case ELEMENT_SOILDTRIANGLE: {
		auto e = (ElementSoildTriangle*)&ele->Data;
		CRender::DrawSoildTriangle(e->x1, e->y1, e->x2, e->y2, e->x3, e->y3, e->col);
		break;
	}
	case ELEMENT_STRING: {
		auto e = (ElementString*)&ele->Data;
		CRender::DrawStringA(e->x, e->y, &e->str, e->size, e->col);
		break;
	}
	case ELEMENT_STROKESTRING: {
		auto e = (ElementStrokeString*)&ele->Data;
		CRender::DrawStringStrokeA(e->x, e->y, &e->str, e->size, e->col, e->StrokeCol);
		break;
	}
	default:
		break;
	}
}

ReleasedTaskBuffer g_tsk;
ReleasedTaskSenderCache g_cache;
DWORD WINAPI TaskThread(PVOID) {
	volatile int refs = 0;
	bool rev = false;
	while (1) {
		static float y = 0.f;
		
		

		TaskSenderCache &ctx = g_cache.Cache;

		TaskSender::Begin(ctx);

		TaskSender::AddBox(ctx, 0.f, 0.f, 100.f, 100.f, PM_RED);
		TaskSender::AddCircle(ctx, 200.f, 200.f, 50.f, 30, PM_RED);

		TaskSender::AddLine(ctx, 0.f, y, 600.f, y, PM_RED);
		if (rev)
			y -= 1.f;
		else
			y += 1.f;
		TaskSender::AddStringStroke(ctx, 100.f, y, "asd", 16, PM_RED, PM_BLACK);

		TaskSender::PresentToTask(&g_tsk.Task, ctx);

		refs++;
		if (refs == 50) {
			refs = 0;
			rev = !rev;
		}

		Sleep(10);
	}
}

int main()
{
	//printf("[112233] %1.1f", fmod(2.3f, 1.f));
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL };
	::RegisterClassEx(&wc);
	hwnd = ::CreateWindow(wc.lpszClassName, L"PmGui Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
	CRender::SetWH(1280,800);
	TaskRecv::InitNewTaskBuffer(g_tsk.Task);
	DWORD Tid = 0;
	CreateThread(0, 0, TaskThread, 0, 0, &Tid);

		// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}
	CRender::Init(g_pd3dDevice, g_pd3dDeviceContext,g_pSwapChain);
	CRender::InitFontWithFile("1.hjf");
	Gui::SetFunc_FillRect((PM_FUNC_FillRect)CRender::FillRect);
	Gui::SetFunc_AddLine((PM_FUNC_AddLine)CRender::DrawLine);
	Gui::SetFunc_AddText(FnAddText);
	Gui::SetFunc_FillTriangle((PM_FUNC_FillTriangle)CRender::DrawSoildTriangle);
	Gui::SetFunc_GetTextWidth(FnGetSize);
	PM_SetFunc_GetMousePos(GetMousePos);

	SGUI::SetFunc_AddLine((PM_FUNC_AddLine)CRender::DrawLine);
	SGUI::SetFunc_AddText(FnAddText);
	SGUI::SetFunc_FillRect((PM_FUNC_FillRect)CRender::FillRect);
	SGUI::SetFunc_GetTextWidth(FnGetSize);

	SGUI::Init(16.f, PmRect(200.F, 20.F, 200.F, 500.F));

	Gui::Init(20.f, PmRect(20.F, 20.F, 500.F, 500.F));
	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);
	MSG msg;
	while (1) {
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		FLOAT color[] = { 0.45f, 0.55f, 0.60f, 1.00f };
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (const FLOAT *)&color);
		g_pd3dDeviceContext->ClearDepthStencilView(g_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		CRender::BeginScene();

		CRender::DrawStringStrokeW(200, 200, L"CSlime", 16, PM_GREEN);
		CRender::DrawLine(10.f, 10.f, 100.f, 1000.f, 0xFF00FFFF);

		CRender::DrawStringStrokeA(0.f, 0.f, "1234567890QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm", 30, 0xFF00FFFF);


		POINT point = { 0 };
		GetCursorPos(&point);
		ScreenToClient(hwnd, &point);
		PmUserEvent evt;
		evt.EvtType = PMEVT_M_MOVE;
		evt.EvtData.M.cur_pos.x = (float)point.x;
		evt.EvtData.M.cur_pos.y = (float)point.y;
		PM_UpdateEvent();
		PM_UpdateKeyEvent();


		if (Gui::Begin(2, u8"1234567890")) {
			if (Gui::Tab(u8"Tab1")) {
				Gui::Text(PM_GREEN, u8"Text");
				Gui::Text(PM_GREEN, u8"Text");
				static bool cbox1 = false;
				static bool cbox2 = false;
				static bool cbox3 = false;
				Gui::CheckBox(u8"选择框1", &cbox1);
				Gui::CheckBox(u8"选择框2", &cbox2);
				Gui::CheckBox(u8"选择框3", &cbox3);

				const char* EntryText[] = { u8"选项1",u8"选项2",u8"选项3" };
				static int cbidx = 0;
				if (Gui::ComboBox(u8"组合框1", &cbidx, EntryText, 3)) {
					//printf("combo_selected\n");
				}
				static int cbidx2 = 0;
				if (Gui::ComboBox(u8"组合框2", &cbidx2, EntryText, 3)) {
					//printf("combo_selected\n");
				}
				Gui::CheckBox(u8"选择框3_1", &cbox3);
				static float s_value = 5.f;
				char text[100] = { 0 };
				sprintf_s(text, u8"%1.3f", s_value);
				Gui::Slider(text, &s_value, 5.f, 20.f);
				Gui::Slider(text, &s_value, 5.f, 20.f);
				Gui::Slider(text, &s_value, 5.f, 20.f);

				static float sv = 10.f;
				sprintf_s(text, u8"%1.1f", sv);
				Gui::Slider(text, &sv, 10.f, 20.f, 5.f);
				//printf("cbidx:%d\n", cbidx);
				Gui::Text(PM_GREEN, "Teasdsadsadasdasdxt");
			}
			if (Gui::Tab("Tab2")) {
				Gui::Text(PM_RED, "Text1");

				static bool cbox3 = true;
				Gui::CheckBox(u8"选择框3_1", &cbox3);

				LPCSTR list[6] = { u8"鼠标侧键1",u8"鼠标侧键2",u8"CAPS键","2","3","4" };
				static int cbidx = 0;
				Gui::ComboBox(xs(u8"按键"), &cbidx, list, 6);
			}

		}
		Gui::End();

		SGUI::Begin();
		SGUI::Text("123");
		SGUI::MiddleText(u8"标题", 1.f, PM_RGBA(87, 250, 255, 233));
		static bool bv1 = 1;
		static bool bv2 = 1;
		static bool bv3 = 1;
		SGUI::CheckAndBind(bv1 ? u8"awa [开启]" : u8"awa [关闭]", &bv1, PM_VK_F9);
		SGUI::Check(u8"qwq", &bv2);
		SGUI::Check(u8"pwp", &bv3);

		SGUI::MiddleText(u8"mid", 1.2f, PM_RGBA(87, 250, 255, 233));

		SGUI::End();

		bool r = TaskRecv::ProcessTask(g_tsk.Task, DrawDispatch);
		if (r == false) {
			static int cnt = 0;
			cnt++;
			myDbgPrint1("No Task %d\n", cnt);
		}
		CRender::DrawLine(0, 0, 100, 100, PM_RED);

		CRender::DrawStringStrokeW(500, 50, L"Author: CSlime", 60, PM_GOLD);

		CRender::EndScene();

		//printf("begin present\n");
		//system("pause");
		//__debugbreak();
		g_pSwapChain->Present(1, 0);
		static ID3D11Device *dev = 0;
		g_pSwapChain->GetDevice(__uuidof(dev), (void **)&dev);
		//__debugbreak();
		//printf("end present\n");
		//system("pause");
	}
    std::cout << "Hello World!\n"; 
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
/*
Author: CSlime
Github: https://github.com/cs1ime
*/