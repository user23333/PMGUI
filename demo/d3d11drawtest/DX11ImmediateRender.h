/*
Author: CSlime
Github: https://github.com/cs1ime
*/
#pragma once

#ifndef __DX11ImmediateRender_INCLUDED__
#define __DX11ImmediateRender_INCLUDED__

#include <d3d11.h>
#include <xnamath.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <intrin.h>
#include "mycrt.h"
#include "DxCol.h"
//#include <d3dcompiler.h>


namespace CRender {
	//bool InitD3D(HWND hwnd);
	ID3D11DeviceContext* GetDeviceContext();
	ID3D11Device* GetDevice();
	IDXGISwapChain* GetSwapChain();
	ID3D11RenderTargetView* GetRenderTargetView();

	VOID SetWH(LONG w, LONG h);
	BOOL Init(ID3D11Device *Device, ID3D11DeviceContext *Context, IDXGISwapChain *SwapChain);
	VOID SetNewCtx(ID3D11Device* Device, ID3D11DeviceContext* Context, IDXGISwapChain* SwapChain);
	BOOL InitFont(const BYTE *file_base, ULONG file_size);
	BOOL InitFontWithFile(LPCSTR FilePath);
	BOOL BeginScene();
	void GetWH(float* w, float* h);
	BOOL EndScene();
	void FillRect(float x, float y, float w, float h, DWORD col);
	void DrawLine(float sx, float sy, float ex, float ey, DWORD color);
	void DrawBox(float x, float y, float w, float h, DWORD col);
	void DrawCornBox(float x, float y, float w, float h, DWORD col);
	void DrawCornBoxOutLine(float x, float y, float w, float h, DWORD col);
	void DrawBox_OutLine(float x, float y, float w, float h, DWORD col);
	void DrawRainbowLine(float sx, float sy, float ex, float ey);
	void DrawRainbowBox(float x, float y, float w, float h);
	void DrawCircle(float centerx, float centery, float rad, long pointCount, DWORD col);
	float GetStringPixelLengthW(LPCWSTR Str, ULONG Size);
	float GetStringPixelLengthA(LPCSTR Str, ULONG Size);
	void DrawStringW(float x, float y, LPCWSTR Str, ULONG Size, DWORD col);
	void DrawStringA(float x, float y, LPCSTR Str, ULONG Size, DWORD col);

	void DrawStringStrokeW(float x, float y, LPCWSTR Str, ULONG Size, DWORD col, DWORD StrokeColor = Color::Black);
	void DrawStringStrokeA(float x, float y, LPCSTR Str, ULONG Size, DWORD col, DWORD StrokeColor = Color::Black);

	void DrawVerticalHealthLine(float x, float y, float w, float h, int health, int max_health, DWORD HealthColor);
	void DrawSoildTriangle(float x1, float y1, float x2, float y2, float x3, float y3, DWORD col);
	void DrawSoildCircle(float centerx, float centery, float rad, long pointCount, DWORD col);


}

static char g_VS_Data[] = {
	0x44, 0x58, 0x42, 0x43, 0xCC, 0x07, 0x13, 0x10, 0x3F, 0x7F, 0x52, 0x3C, 0xDF, 0xC2, 0xD7, 0xF8,
	0xCE, 0x30, 0x3D, 0x92, 0x01, 0x00, 0x00, 0x00, 0xA4, 0x03, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
	0x34, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x6C, 0x01, 0x00, 0x00, 0xE0, 0x01, 0x00, 0x00,
	0x28, 0x03, 0x00, 0x00, 0x52, 0x44, 0x45, 0x46, 0xC0, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x48, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x04, 0xFE, 0xFF,
	0x00, 0x01, 0x00, 0x00, 0x8C, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x62, 0x50, 0x65, 0x72, 0x4F, 0x62, 0x6A,
	0x65, 0x63, 0x74, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00,
	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x57, 0x56, 0x50, 0x00, 0x03, 0x00, 0x03, 0x00, 0x04, 0x00, 0x04, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4D, 0x69, 0x63, 0x72, 0x6F, 0x73, 0x6F, 0x66,
	0x74, 0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4C, 0x53, 0x4C, 0x20, 0x53, 0x68, 0x61, 0x64, 0x65,
	0x72, 0x20, 0x43, 0x6F, 0x6D, 0x70, 0x69, 0x6C, 0x65, 0x72, 0x20, 0x39, 0x2E, 0x32, 0x39, 0x2E,
	0x39, 0x35, 0x32, 0x2E, 0x33, 0x31, 0x31, 0x31, 0x00, 0xAB, 0xAB, 0xAB, 0x49, 0x53, 0x47, 0x4E,
	0x68, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x03, 0x00, 0x00, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x62, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x0F, 0x0F, 0x00, 0x00, 0x50, 0x4F, 0x53, 0x49, 0x54, 0x49, 0x4F, 0x4E, 0x00, 0x54, 0x45, 0x58,
	0x43, 0x4F, 0x4F, 0x52, 0x44, 0x00, 0x43, 0x4F, 0x4C, 0x4F, 0x52, 0x00, 0x4F, 0x53, 0x47, 0x4E,
	0x6C, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0F, 0x00, 0x00, 0x00, 0x5C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x0C, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x0F, 0x00, 0x00, 0x00, 0x53, 0x56, 0x5F, 0x50, 0x4F, 0x53, 0x49, 0x54, 0x49, 0x4F, 0x4E, 0x00,
	0x54, 0x45, 0x58, 0x43, 0x4F, 0x4F, 0x52, 0x44, 0x00, 0x43, 0x4F, 0x4C, 0x4F, 0x52, 0x00, 0xAB,
	0x53, 0x48, 0x44, 0x52, 0x40, 0x01, 0x00, 0x00, 0x40, 0x00, 0x01, 0x00, 0x50, 0x00, 0x00, 0x00,
	0x59, 0x00, 0x00, 0x04, 0x46, 0x8E, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x5F, 0x00, 0x00, 0x03, 0x32, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x00, 0x00, 0x03,
	0x32, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x5F, 0x00, 0x00, 0x03, 0xF2, 0x10, 0x10, 0x00,
	0x02, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00, 0x04, 0xF2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x03, 0x32, 0x20, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x65, 0x00, 0x00, 0x03, 0xF2, 0x20, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x02,
	0x01, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x05, 0x32, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x46, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x05, 0x42, 0x00, 0x10, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x10, 0x00, 0x00, 0x08,
	0x12, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x46, 0x83, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x08,
	0x22, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x46, 0x83, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x08,
	0x42, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x46, 0x83, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x08,
	0x82, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x46, 0x83, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x05,
	0x32, 0x20, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x46, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x36, 0x00, 0x00, 0x05, 0xF2, 0x20, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x46, 0x1E, 0x10, 0x00,
	0x02, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x01, 0x53, 0x54, 0x41, 0x54, 0x74, 0x00, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char g_PS_Data[] = {
	0x44, 0x58, 0x42, 0x43, 0xFE, 0x65, 0x4E, 0xB4, 0x69, 0x6A, 0x17, 0x0E, 0x53, 0x35, 0xAD, 0xD2,
	0xAE, 0xBD, 0xF9, 0x6A, 0x01, 0x00, 0x00, 0x00, 0x88, 0x03, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
	0x34, 0x00, 0x00, 0x00, 0x5C, 0x01, 0x00, 0x00, 0xD0, 0x01, 0x00, 0x00, 0x04, 0x02, 0x00, 0x00,
	0x0C, 0x03, 0x00, 0x00, 0x52, 0x44, 0x45, 0x46, 0x20, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0xA4, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x04, 0xFF, 0xFF,
	0x00, 0x01, 0x00, 0x00, 0xEC, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8C, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x05, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x97, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4F, 0x62, 0x6A, 0x53, 0x61, 0x6D, 0x70, 0x6C,
	0x65, 0x72, 0x53, 0x74, 0x61, 0x74, 0x65, 0x00, 0x4F, 0x62, 0x6A, 0x54, 0x65, 0x78, 0x74, 0x75,
	0x72, 0x65, 0x00, 0x63, 0x62, 0x50, 0x65, 0x72, 0x4F, 0x62, 0x6A, 0x65, 0x63, 0x74, 0x00, 0xAB,
	0x97, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xBC, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x6D, 0x6F, 0x64, 0x65, 0x00, 0xAB, 0xAB, 0xAB, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4D, 0x69, 0x63, 0x72, 0x6F, 0x73, 0x6F, 0x66,
	0x74, 0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4C, 0x53, 0x4C, 0x20, 0x53, 0x68, 0x61, 0x64, 0x65,
	0x72, 0x20, 0x43, 0x6F, 0x6D, 0x70, 0x69, 0x6C, 0x65, 0x72, 0x20, 0x39, 0x2E, 0x32, 0x39, 0x2E,
	0x39, 0x35, 0x32, 0x2E, 0x33, 0x31, 0x31, 0x31, 0x00, 0xAB, 0xAB, 0xAB, 0x49, 0x53, 0x47, 0x4E,
	0x6C, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0F, 0x00, 0x00, 0x00, 0x5C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x0F, 0x0F, 0x00, 0x00, 0x53, 0x56, 0x5F, 0x50, 0x4F, 0x53, 0x49, 0x54, 0x49, 0x4F, 0x4E, 0x00,
	0x54, 0x45, 0x58, 0x43, 0x4F, 0x4F, 0x52, 0x44, 0x00, 0x43, 0x4F, 0x4C, 0x4F, 0x52, 0x00, 0xAB,
	0x4F, 0x53, 0x47, 0x4E, 0x2C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
	0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x53, 0x56, 0x5F, 0x54, 0x41, 0x52, 0x47, 0x45,
	0x54, 0x00, 0xAB, 0xAB, 0x53, 0x48, 0x44, 0x52, 0x00, 0x01, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
	0x40, 0x00, 0x00, 0x00, 0x59, 0x00, 0x00, 0x04, 0x46, 0x8E, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x5A, 0x00, 0x00, 0x03, 0x00, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x58, 0x18, 0x00, 0x04, 0x00, 0x70, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00,
	0x62, 0x10, 0x00, 0x03, 0x32, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x62, 0x10, 0x00, 0x03,
	0xF2, 0x10, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x03, 0xF2, 0x20, 0x10, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x08,
	0x12, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x80, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x1F, 0x00, 0x04, 0x03,
	0x0A, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0x00, 0x00, 0x09, 0xF2, 0x00, 0x10, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x46, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x46, 0x7E, 0x10, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x05,
	0x12, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F,
	0x38, 0x00, 0x00, 0x07, 0xF2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0C, 0x10, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x46, 0x1E, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x01,
	0x36, 0x00, 0x00, 0x05, 0xF2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x1E, 0x10, 0x00,
	0x02, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x01, 0x3E, 0x00, 0x00, 0x01, 0x53, 0x54, 0x41, 0x54,
	0x74, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#endif // !__DX11ImmediateRender_INCLUDED__


