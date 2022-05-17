/*
Author: CSlime
Github: https://github.com/cs1ime
*/
#include "DX11ImmediateRender.h"
#include "DxImmediateVar.h"

//#pragma optimize( "", off )

struct BACKUP_DX11_STATE
{
	UINT                        ScissorRectsCount, ViewportsCount;
	D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	ID3D11RasterizerState*      RS;
	ID3D11BlendState*           BlendState;
	FLOAT                       BlendFactor[4];
	UINT                        SampleMask;
	UINT                        StencilRef;
	ID3D11DepthStencilState*    DepthStencilState;
	ID3D11ShaderResourceView*   PSShaderResource;
	ID3D11SamplerState*         PSSampler;
	ID3D11PixelShader*          PS;
	ID3D11VertexShader*         VS;
	UINT                        PSInstancesCount, VSInstancesCount;
	ID3D11ClassInstance*        PSInstances[256], *VSInstances[256];   // 256 is max according to PSSetShader documentation
	D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
	ID3D11Buffer*               IndexBuffer, *VertexBuffer, *VSConstantBuffer;
	UINT                        IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
	DXGI_FORMAT                 IndexBufferFormat;
	ID3D11InputLayout*          InputLayout;
	ID3D11RenderTargetView *	RenderTargetView;
	ID3D11DepthStencilView *	DepthStencilView;
};
BACKUP_DX11_STATE old;
bool using_old = false;


static ID3D11RenderTargetView* g_renderTargetView;
static ID3D11DepthStencilView* depthStencilView;
static ID3D11Texture2D* depthStencilBuffer;

static IDXGISwapChain* g_SwapChain = 0;
static ID3D11SamplerState* g_pDefaultSampler = NULL;
static ID3D11Texture2D* g_pDefaultTexture = NULL;
static ID3D11ShaderResourceView* g_pDefaultTextureView = NULL;
static ID3D11Device* g_pd3dDevice = 0;
static ID3D11DeviceContext* g_pd3dDeviceContext = 0;
static ID3D11InputLayout* g_InputLayout = 0;
static ID3D11DepthStencilView* g_depthStencilView = 0;
static ID3D11DepthStencilState* g_pDepthStencilState = NULL;
static ID3D11BlendState* g_pBlendState = NULL;
static ID3D11Buffer* g_VBuffer = 0;
static ID3D11Buffer* g_IBuffer = 0;
static ID3D11VertexShader* g_VS = 0;
static ID3D11PixelShader* g_PS = 0;
static ID3D11Buffer* g_WVPBuffer = 0;
static ID3D11Buffer* g_ModeBuffer = 0;
static ID3D11RasterizerState* g_WireFrameState = 0;
static ID3D11RasterizerState* g_TextureFrameState = 0;

static ID3D10Blob* g_pVertexShaderBlob = NULL;
static ID3D10Blob* g_pPixelShaderBlob = NULL;

static FLOAT g_width = 0.0f;
static FLOAT g_height = 0.0f;


bool use_static_wh = false;

namespace CRender {
	
	void CreateRenderTarget()
	{
		ID3D11Texture2D* pBackBuffer;
		g_SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_renderTargetView);
		pBackBuffer->Release();
	}
	ID3D11DeviceContext *GetDeviceContext() {
		return g_pd3dDeviceContext;
	}
	ID3D11Device* GetDevice() {
		return g_pd3dDevice;
	}
	IDXGISwapChain* GetSwapChain() {
		return g_SwapChain;
	}
	ID3D11RenderTargetView* GetRenderTargetView() {
		return g_renderTargetView;
	}

	XMMATRIX MakeWVPMatrix(float wid, float hei) {
		XMVECTOR camPosition = XMVectorSet(wid / 2.0f, (hei / 2.0f), -1.0f*hei, 0.0f);
		XMVECTOR camTarget = XMVectorSet(wid / 2.0f, (hei / 2.0f), 0.0f, 0.0f);
		XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMMATRIX View = XMMatrixLookAtLH(camPosition, camTarget, camUp);
		XMMATRIX Projection = XMMatrixPerspectiveFovLH(0.2955f*3.1415926535f, wid / hei, 1.0f, 1000.0f);
		Projection = Projection * XMMatrixRotationX(3.1415926535f);
		XMMATRIX WVP = View * Projection;
		return XMMatrixTranspose(WVP);
	}
#pragma optimize( "", off )
	VOID InitValues() {
		g_renderTargetView = 0;
		depthStencilView = 0;
		depthStencilBuffer = 0;

		g_SwapChain = 0;
		g_pDefaultSampler = NULL;
		g_pDefaultTexture = NULL;
		g_pDefaultTextureView = NULL;
		g_pd3dDevice = 0;
		g_pd3dDeviceContext = 0;
		g_InputLayout = 0;
		g_depthStencilView = 0;
		g_pDepthStencilState = NULL;
		g_pBlendState = NULL;
		g_VBuffer = 0;
		g_IBuffer = 0;
		g_VS = 0;
		g_PS = 0;
		g_WVPBuffer = 0;
		g_ModeBuffer = 0;
		g_WireFrameState = 0;
		g_TextureFrameState = 0;

		g_pVertexShaderBlob = NULL;
		g_pPixelShaderBlob = NULL;

		//g_width = 0.0f;
		//g_height = 0.0f;

		g_GlyphListSize = 0;
		g_DrawListSize = 0;
		g_vtx_buffer_size = 0;
		g_idx_buffer_size = 0;

		//g_bitmaps = 0;
		g_VBufferSize = 5000;
		g_IBufferSize = 10000;

		ZeroMemory(g_FontList, sizeof(g_FontList));
		ZeroMemory(g_FontVaildList, sizeof(g_FontVaildList));
	}
	VOID SetWH(LONG w, LONG h) {
		use_static_wh = true;
		g_width = (float)w;
		g_height = (float)h;
	}
	BOOL Init(ID3D11Device *Device, ID3D11DeviceContext *Context, IDXGISwapChain *SwapChain) {
		
		crt::init(FALSE);
		InitValues();
		g_pd3dDevice = Device;
		g_pd3dDeviceContext = Context;
		g_SwapChain = SwapChain;

		CreateRenderTarget();
		
		//printf("swapchain:%x\n", SwapChain);
		/*static const char *vs_data =
			"cbuffer cbPerObject{ float4x4 WVP;};\
			 Texture2D ObjTexture;\
			 SamplerState ObjSamplerState;\
			 struct VS_OUTPUT {\
			 float4 Pos : SV_POSITION;\
			 float2 TexCoord : TEXCOORD;\
			 float4 Color : COLOR;\
			 };\
			 VS_OUTPUT VS(float2 inPos : POSITION, float2 inTexCoord : TEXCOORD, float4 inColor : COLOR) {\
			 VS_OUTPUT output;\
			 output.Pos = mul(float4(inPos, 0.f, 1.f), WVP);\
			 output.Color = inColor;\
			 output.TexCoord = inTexCoord;\
			 return output;\
			 }";
		static const char *ps_data =
			"cbuffer cbPerObject{ float mode;};\
			 Texture2D ObjTexture;\
			 SamplerState ObjSamplerState;\
			 struct VS_OUTPUT {\
			 float4 Pos : SV_POSITION;\
			 float2 TexCoord : TEXCOORD;\
			 float4 Color : COLOR;\
			 };\
			 float4 PS(VS_OUTPUT input) : SV_TARGET{\
			float4 col = input.Color;\
			if(mode == 1.f){\
			float4 texcol = ObjTexture.Sample(ObjSamplerState, input.TexCoord);\
			texcol.rgb = 1;\
			col = col * texcol;}\
			return col;\
			}";
		typedef HRESULT (WINAPI *D3DCOMPILE)(__in_bcount(SrcDataSize) LPCVOID pSrcData,__in SIZE_T SrcDataSize,__in_opt LPCSTR pSourceName,
				__in_xcount_opt(pDefines->Name != NULL) CONST D3D_SHADER_MACRO* pDefines,__in_opt ID3DInclude* pInclude,__in LPCSTR pEntrypoint,
			__in LPCSTR pTarget, __in UINT Flags1, __in UINT Flags2, __out ID3DBlob** ppCode, __out_opt ID3DBlob** ppErrorMsgs);
		D3DCOMPILE D3DCompile = (D3DCOMPILE)GetProcAddress(LoadLibrary(L"d3dcompiler_43.dll"), "D3DCompile");
		//printf("addr:%p\n", D3DCompile);
		//Sleep(1000);
		D3DCompile(vs_data, strlen(vs_data), NULL, NULL, NULL, "VS", "vs_4_0", 0, 0, &g_pVertexShaderBlob, NULL);
		D3DCompile(ps_data, strlen(ps_data), NULL, NULL, NULL, "PS", "ps_4_0", 0, 0, &g_pPixelShaderBlob, NULL);
		if (g_pVertexShaderBlob == 0) return FALSE;
		g_pd3dDevice->CreateVertexShader(g_pVertexShaderBlob->GetBufferPointer(), g_pVertexShaderBlob->GetBufferSize(), NULL, &g_VS);
		g_pd3dDevice->CreatePixelShader(g_pPixelShaderBlob->GetBufferPointer(), g_pPixelShaderBlob->GetBufferSize(), NULL, &g_PS);
		//printf("vbuf:%p vsize:%x\n", g_pVertexShaderBlob->GetBufferPointer(), g_pVertexShaderBlob->GetBufferSize());
		//printf("pbuf:%p psize:%x\n", g_pPixelShaderBlob->GetBufferPointer(), g_pPixelShaderBlob->GetBufferSize());
		//PrintHexValue((BYTE *)g_pVertexShaderBlob->GetBufferPointer(), g_pVertexShaderBlob->GetBufferSize());
		//PrintHexValue((BYTE *)g_pPixelShaderBlob->GetBufferPointer(), g_pPixelShaderBlob->GetBufferSize());
		*/
		g_pd3dDevice->CreateVertexShader(g_VS_Data, sizeof(g_VS_Data), NULL, &g_VS);
		g_pd3dDevice->CreatePixelShader(g_PS_Data, sizeof(g_PS_Data), NULL, &g_PS);
		//g_pd3dDeviceContext->VSSetShader(g_VS, 0, 0);
		//g_pd3dDeviceContext->PSSetShader(g_PS, 0, 0);

		D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (size_t)(&((CVertex*)0)->x), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (size_t)(&((CVertex*)0)->u), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((CVertex*)0)->col), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		g_pd3dDevice->CreateInputLayout(layout, 3, g_VS_Data, sizeof(g_VS_Data), &g_InputLayout);
		//g_pd3dDevice->CreateInputLayout(layout, 3, g_pVertexShaderBlob->GetBufferPointer(), g_pVertexShaderBlob->GetBufferSize(), &g_InputLayout);
		//g_pd3dDeviceContext->IASetInputLayout(g_InputLayout);

		D3D11_BUFFER_DESC vertexBufferDesc;
		RtlZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(CVertex) * g_VBufferSize;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		g_pd3dDevice->CreateBuffer(&vertexBufferDesc, NULL, &g_VBuffer);

		/*if (g_pd3dDeviceContext->Map(g_VBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &g_vtx_resource) != S_OK)
			return FALSE;
		g_vtx_buffer = (CVertex*)(g_vtx_resource.pData);*/

		//UINT stride = sizeof(CVertex);
		//UINT offset = 0;
		//g_pd3dDeviceContext->IASetVertexBuffers(0, 1, &g_VBuffer, &stride, &offset);

		D3D11_BUFFER_DESC indexBufferDesc;
		RtlZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * g_IBufferSize;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		indexBufferDesc.MiscFlags = 0;
		g_pd3dDevice->CreateBuffer(&indexBufferDesc, NULL, &g_IBuffer);

		/*if (g_pd3dDeviceContext->Map(g_IBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &g_idx_resource) != S_OK)
			return FALSE;
		g_idx_buffer = (DWORD*)(g_idx_resource.pData);*/
		//g_pd3dDeviceContext->IASetIndexBuffer(g_IBuffer, DXGI_FORMAT_R32_UINT, 0);

		D3D11_BUFFER_DESC cbbd;
		RtlZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = sizeof(XMMATRIX);
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;
		g_pd3dDevice->CreateBuffer(&cbbd, NULL, &g_WVPBuffer);

		RtlZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = sizeof(FLOAT) * 4;
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;
		g_pd3dDevice->CreateBuffer(&cbbd, NULL, &g_ModeBuffer);

		D3D11_RASTERIZER_DESC wfdesc;
		RtlZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
		wfdesc.FillMode = D3D11_FILL_WIREFRAME;
		wfdesc.CullMode = D3D11_CULL_NONE;
		g_pd3dDevice->CreateRasterizerState(&wfdesc, &g_WireFrameState);

		RtlZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
		wfdesc.FillMode = D3D11_FILL_SOLID;
		wfdesc.CullMode = D3D11_CULL_NONE;
		g_pd3dDevice->CreateRasterizerState(&wfdesc, &g_TextureFrameState);

		// Create the blending setup
		{
			D3D11_BLEND_DESC desc;
			RtlZeroMemory(&desc, sizeof(desc));
			desc.AlphaToCoverageEnable = false;
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			g_pd3dDevice->CreateBlendState(&desc, &g_pBlendState);
		}
		// Create depth-stencil State
		{
			D3D11_DEPTH_STENCIL_DESC desc;
			RtlZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			desc.StencilEnable = true;
			desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			desc.BackFace = desc.FrontFace;
			g_pd3dDevice->CreateDepthStencilState(&desc, &g_pDepthStencilState);
		}
		{
			D3D11_SAMPLER_DESC desc;
			RtlZeroMemory(&desc, sizeof(desc));
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.MipLODBias = 0.f;
			desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			desc.MinLOD = 0.f;
			desc.MaxLOD = 0.f;
			g_pd3dDevice->CreateSamplerState(&desc, &g_pDefaultSampler);
		}
		{
			D3D11_TEXTURE2D_DESC desc;
			RtlZeroMemory(&desc, sizeof(desc));
			desc.Width = 1;
			desc.Height = 1;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA subResource;
			DWORD pixel = 0xFFFFFFFF;
			subResource.pSysMem = &pixel;
			subResource.SysMemPitch = 1;
			subResource.SysMemSlicePitch = 0;
			g_pd3dDevice->CreateTexture2D(&desc, &subResource, &g_pDefaultTexture);
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			RtlZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;
			g_pd3dDevice->CreateShaderResourceView(g_pDefaultTexture, &srvDesc, &g_pDefaultTextureView);
		}

		//g_pd3dDevice->AddRef();
		//g_pd3dDeviceContext->AddRef();
		return TRUE;
	}
	VOID SetNewCtx(ID3D11Device* Device, ID3D11DeviceContext* Context, IDXGISwapChain* SwapChain) {
		g_pd3dDevice = Device;
		g_pd3dDeviceContext = Context;
		g_SwapChain = SwapChain;
		CreateRenderTarget();
		g_pd3dDevice->CreateVertexShader(g_VS_Data, sizeof(g_VS_Data), NULL, &g_VS);
		g_pd3dDevice->CreatePixelShader(g_PS_Data, sizeof(g_PS_Data), NULL, &g_PS);
		//g_pd3dDeviceContext->VSSetShader(g_VS, 0, 0);
		//g_pd3dDeviceContext->PSSetShader(g_PS, 0, 0);

		D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (size_t)(&((CVertex*)0)->x), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (size_t)(&((CVertex*)0)->u), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((CVertex*)0)->col), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		g_pd3dDevice->CreateInputLayout(layout, 3, g_VS_Data, sizeof(g_VS_Data), &g_InputLayout);
		//g_pd3dDevice->CreateInputLayout(layout, 3, g_pVertexShaderBlob->GetBufferPointer(), g_pVertexShaderBlob->GetBufferSize(), &g_InputLayout);
		//g_pd3dDeviceContext->IASetInputLayout(g_InputLayout);

		D3D11_BUFFER_DESC vertexBufferDesc;
		RtlZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(CVertex) * g_VBufferSize;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		g_pd3dDevice->CreateBuffer(&vertexBufferDesc, NULL, &g_VBuffer);

		/*if (g_pd3dDeviceContext->Map(g_VBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &g_vtx_resource) != S_OK)
			return FALSE;
		g_vtx_buffer = (CVertex*)(g_vtx_resource.pData);*/

		//UINT stride = sizeof(CVertex);
		//UINT offset = 0;
		//g_pd3dDeviceContext->IASetVertexBuffers(0, 1, &g_VBuffer, &stride, &offset);

		D3D11_BUFFER_DESC indexBufferDesc;
		RtlZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * g_IBufferSize;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		indexBufferDesc.MiscFlags = 0;
		g_pd3dDevice->CreateBuffer(&indexBufferDesc, NULL, &g_IBuffer);

		/*if (g_pd3dDeviceContext->Map(g_IBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &g_idx_resource) != S_OK)
			return FALSE;
		g_idx_buffer = (DWORD*)(g_idx_resource.pData);*/
		//g_pd3dDeviceContext->IASetIndexBuffer(g_IBuffer, DXGI_FORMAT_R32_UINT, 0);


		D3D11_BUFFER_DESC cbbd;
		RtlZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = sizeof(XMMATRIX);
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;
		g_pd3dDevice->CreateBuffer(&cbbd, NULL, &g_WVPBuffer);

		RtlZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = sizeof(FLOAT) * 4;
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;
		g_pd3dDevice->CreateBuffer(&cbbd, NULL, &g_ModeBuffer);

		D3D11_RASTERIZER_DESC wfdesc;
		RtlZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
		wfdesc.FillMode = D3D11_FILL_WIREFRAME;
		wfdesc.CullMode = D3D11_CULL_NONE;
		g_pd3dDevice->CreateRasterizerState(&wfdesc, &g_WireFrameState);

		RtlZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
		wfdesc.FillMode = D3D11_FILL_SOLID;
		wfdesc.CullMode = D3D11_CULL_NONE;
		g_pd3dDevice->CreateRasterizerState(&wfdesc, &g_TextureFrameState);

		// Create the blending setup
		{
			D3D11_BLEND_DESC desc;
			RtlZeroMemory(&desc, sizeof(desc));
			desc.AlphaToCoverageEnable = false;
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			g_pd3dDevice->CreateBlendState(&desc, &g_pBlendState);
		}
		// Create depth-stencil State
		{
			D3D11_DEPTH_STENCIL_DESC desc;
			RtlZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			desc.StencilEnable = true;
			desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			desc.BackFace = desc.FrontFace;
			g_pd3dDevice->CreateDepthStencilState(&desc, &g_pDepthStencilState);
		}
		{
			D3D11_SAMPLER_DESC desc;
			RtlZeroMemory(&desc, sizeof(desc));
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.MipLODBias = 0.f;
			desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			desc.MinLOD = 0.f;
			desc.MaxLOD = 0.f;
			g_pd3dDevice->CreateSamplerState(&desc, &g_pDefaultSampler);
		}
		{
			D3D11_TEXTURE2D_DESC desc;
			RtlZeroMemory(&desc, sizeof(desc));
			desc.Width = 1;
			desc.Height = 1;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA subResource;
			DWORD pixel = 0xFFFFFFFF;
			subResource.pSysMem = &pixel;
			subResource.SysMemPitch = 1;
			subResource.SysMemSlicePitch = 0;
			g_pd3dDevice->CreateTexture2D(&desc, &subResource, &g_pDefaultTexture);
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			RtlZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;
			g_pd3dDevice->CreateShaderResourceView(g_pDefaultTexture, &srvDesc, &g_pDefaultTextureView);
		}
	}
	ULONG g_MaxSize = 0;
	BOOL InitFont(const BYTE *file_base, ULONG file_size) {

		struct FileHeader {
			ULONG tag;
			ULONG Count;
		};
		struct CharFileInfo {
			wchar_t Char;
			int size;
			int bitmap_left;
			int bitmap_top;
			int bitmap_width;
			int bitmap_height;
		};

		FileHeader* header = (FileHeader*)file_base;
		if (header->tag != 'JH') {
			return FALSE;
		}
		ULONG bitmaps_size = 0;
		CharFileInfo* info = (CharFileInfo*)(file_base + sizeof(FileHeader));
		g_MaxSize = info->size;
		for (int i = 0; i < header->Count; i++) {
			ULONG size = info->bitmap_height * info->bitmap_width;
			bitmaps_size += size;
			info = (CharFileInfo*)(((PUCHAR)info) + sizeof(CharFileInfo) + size);
		}
		info = (CharFileInfo*)(file_base + sizeof(FileHeader));
		for (int i = 0; i < header->Count; i++) {
			ULONG size = info->bitmap_height * info->bitmap_width;
			CFontInfo Glyph;
			memset(&Glyph, 0, sizeof(CFontInfo));

			Glyph.Char = info->Char;
			Glyph.Size = info->size;
			Glyph.Height = info->bitmap_height;
			Glyph.Width = info->bitmap_width;
			Glyph.Left = info->bitmap_left;
			Glyph.Top = info->bitmap_top;
			Glyph.bitmap = (BYTE *)(info + 1);
			//g_FontList.push_back(Glyph);
			g_FontList[Glyph.Char] = Glyph;
			g_FontVaildList[Glyph.Char] = 1;

			info = (CharFileInfo*)(((PUCHAR)info) + sizeof(CharFileInfo) + size);
		}


		return TRUE;

	}
	BOOL InitFontWithFile(LPCSTR FilePath) {
		HANDLE hFile = CreateFileA(FilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
		if (hFile == INVALID_HANDLE_VALUE) {
			return FALSE;
		}
		ULONG FileSize = GetFileSize(hFile, 0);
		UCHAR* FileBuffer = (UCHAR *)crt::malloc(FileSize);
		DWORD read = 0;
		if (ReadFile(hFile, FileBuffer, FileSize, &read, 0)) {
			CloseHandle(hFile);
			BOOL result = InitFont(FileBuffer, FileSize);
			//crt::free(FileBuffer);
			return result;
		}
		CloseHandle(hFile);
		return FALSE;
		
		
		
	}
#pragma optimize( "", on )
	BOOL CreateFontGlyph(USHORT Char, ULONG Size, CFontGlyph *Out) {
		//printf("Char:%x\n", Char);
		CFontInfo* info = NULL;

		if (g_FontVaildList[Char]) {
			info = &g_FontList[Char];
		}
		if (info == NULL) {
			return FALSE;
		}
		size_t charHeight = info->Height;
		size_t charWidth = info->Width;

		ID3D11Texture2D *Tex2D = 0;
		ID3D11ShaderResourceView *TexView = 0;
		//printf("geted\n");
		D3D11_TEXTURE2D_DESC desc;
		RtlZeroMemory(&desc, sizeof(desc));
		desc.Width = charWidth;
		desc.Height = charHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		g_pd3dDevice->CreateTexture2D(&desc, NULL, &Tex2D);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		RtlZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		g_pd3dDevice->CreateShaderResourceView(Tex2D, &srvDesc, &TexView);

		D3D11_MAPPED_SUBRESOURCE tex_resource;
		g_pd3dDeviceContext->Map(Tex2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &tex_resource);

		//CHAR str[1024];
		//sprintf(str,"%p\n", tex_resource.pData);
		//OutputDebugStringA(str);
		//DebugBreak();
		DWORD *pTex;
		UCHAR *buffer = (UCHAR *)(info->bitmap);
		UCHAR *src;
		//printf("\n");
		for (int i = 0; i < charHeight; i++) {
			src = buffer + (info->Width * i);
			pTex = (DWORD *)(((UCHAR *)tex_resource.pData) + (tex_resource.RowPitch*i));
			for (int j = 0; j < charWidth; j++) {
				UCHAR bit = src[j];
				/*if (bit) {
					pTex[j] = 0xFFFFFFFF;
				}
				else {
					pTex[j] = 0;
				}*/
				pTex[j] = (0x00FFFFFF) | (bit << 24);
			}
			//printf("\n");
		}
		g_pd3dDeviceContext->Unmap(Tex2D, 0);

		float bz = ((float)Size) / ((float)info->Size);

		Out->Char = Char;
		Out->Height = ((float)charHeight * bz);
		Out->Width = ((float)charWidth * bz);
		Out->Left = ((float)info->Left * bz);
		Out->Top = ((float)info->Top * bz);
		Out->Size = Size;
		Out->TexId = TexView;
		Out->Tex2D = Tex2D;

		return TRUE;
	}
	CFontGlyph *FindGlyphList(USHORT Char, ULONG Size) {
		if (g_GlyphMap[Char].SizeIndexMap[Size]) {
			return &g_GlyphList[g_GlyphMap[Char].SizeIndexMap[Size]];
		}
		return NULL;
	}
	CFontGlyph *GetFontGlyph(USHORT Char, ULONG Size) {
		if (Size > MAX_FONT_SIZE)
			Size = MAX_FONT_SIZE;
		CFontGlyph *FindResult = FindGlyphList(Char, Size);
		if (FindResult) {
			return FindResult;
		}
		if (g_GlyphListSize >= MAX_GLYPH_COUNT)
			return 0;
		//printf("Begin CreateFont\n");
		CFontGlyph Glyph;
		RtlZeroMemory(&Glyph, sizeof(Glyph));
		Glyph.Char = Char;
		Glyph.Size = Size;
		BOOL result = CreateFontGlyph(Char, Size, &Glyph);

		//printf("Glyph:%p\n", Glyph);
		g_GlyphList[g_GlyphListSize++] = Glyph;
		g_GlyphMap[Char].SizeIndexMap[Size] = g_GlyphListSize - 1;
		
		return &g_GlyphList[g_GlyphListSize - 1];
		//return &g_GlyphList[Char];
	}

	BOOL SetupRenderState() {
		ID3D11DeviceContext* ctx = g_pd3dDeviceContext;
		D3D11_VIEWPORT vp;
		RtlZeroMemory(&vp, sizeof(vp));
		vp.Width = g_width;
		vp.Height = g_height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = vp.TopLeftY = 0;
		ctx->RSSetViewports(1, &vp);
		ctx->IASetInputLayout(g_InputLayout);

		unsigned int stride = sizeof(CVertex);
		unsigned int offset = 0;
		ctx->IASetVertexBuffers(0, 1, &g_VBuffer, &stride, &offset);
		ctx->IASetIndexBuffer(g_IBuffer, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX WVP = MakeWVPMatrix(g_width, g_height);
		ctx->UpdateSubresource(g_WVPBuffer, 0, 0, &WVP, 0, 0);
		ctx->VSSetConstantBuffers(0, 1, &g_WVPBuffer);
		ctx->VSSetShader(g_VS, 0, 0);
		ctx->PSSetShader(g_PS, 0, 0);
		ctx->PSSetSamplers(0, 1, &g_pDefaultSampler);
		ctx->PSSetConstantBuffers(0, 1, &g_ModeBuffer);

		ctx->OMSetRenderTargets(1, &g_renderTargetView, 0);
		const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
		ctx->OMSetBlendState(g_pBlendState, blend_factor, 0xffffffff);
		ctx->OMSetDepthStencilState(g_pDepthStencilState, 0);
		ctx->RSSetState(g_TextureFrameState);
		ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		return TRUE;
	}
	void EnableTextureRender() {
		ID3D11DeviceContext* ctx = g_pd3dDeviceContext;
		struct Modes {
			FLOAT mmode = 1.f;
			FLOAT pending1 = 1.f;
			FLOAT pending2 = 1.f;
			FLOAT pending3 = 1.f;
		}mode;
		ctx->UpdateSubresource(g_ModeBuffer, 0, 0, &mode, 0, 0);
	}
	void DisableTextureRender() {
		ID3D11DeviceContext* ctx = g_pd3dDeviceContext;
		struct Modes {
			FLOAT mmode = 0.f;
			FLOAT pending1 = 0.f;
			FLOAT pending2 = 0.f;
			FLOAT pending3 = 0.f;
		}mode;
		
		ctx->UpdateSubresource(g_ModeBuffer, 0, 0, &mode, 0, 0);
		//ctx->PSSetShaderResources(0, 1, &g_pDefaultTextureView);
	}
	BOOL DrawOut() {
		ID3D11DeviceContext* ctx = g_pd3dDeviceContext;
		//check buffer size
		if (g_VBufferSize < g_vtx_buffer_size) {
			g_VBufferSize = g_vtx_buffer_size + 5000;
			if (g_VBuffer)g_VBuffer->Release();
			D3D11_BUFFER_DESC desc;
			memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = g_VBufferSize * sizeof(CVertex);
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			if (g_pd3dDevice->CreateBuffer(&desc, NULL, &g_VBuffer) < 0)
				return FALSE;

		}
		if (g_IBufferSize < g_idx_buffer_size) {
			g_IBufferSize = g_idx_buffer_size + 10000;
			if (g_IBuffer)g_IBuffer->Release();
			D3D11_BUFFER_DESC desc;
			memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = g_IBufferSize * sizeof(DWORD);
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			if (g_pd3dDevice->CreateBuffer(&desc, NULL, &g_IBuffer) < 0)
				return FALSE;
		}
		D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
		if (ctx->Map(g_VBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource) != S_OK)
			return FALSE;
		if (ctx->Map(g_IBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource) != S_OK)
			return FALSE;

		memcpy(vtx_resource.pData, g_vtx_buffer, g_vtx_buffer_size * sizeof(CVertex));
		memcpy(idx_resource.pData, g_idx_buffer, g_idx_buffer_size * sizeof(DWORD));

		ctx->Unmap(g_VBuffer, 0);
		ctx->Unmap(g_IBuffer, 0);

		UINT idx_offset = 0;
		INT vtx_offset = 0;

		bool flag_enable_tex = false;
		bool flag_disable_tex = false;
		//bool flag_change_primitive = false;
		for (size_t i = 0; i < g_DrawListSize; i++) {
			if (g_DrawList[i].tex_id) {
				flag_disable_tex = false;
				if (flag_enable_tex == false) {
					EnableTextureRender();
					flag_enable_tex = true;
				}
				ctx->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView **)&g_DrawList[i].tex_id);

			}
			else {
				flag_enable_tex = false;
				if (flag_disable_tex == false) {
					DisableTextureRender();
					flag_disable_tex = true;
				}
			}
			ctx->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)g_DrawList[i].primitive_type);
			ctx->DrawIndexed(g_DrawList[i].idx_size, idx_offset, vtx_offset);
			idx_offset += g_DrawList[i].idx_size;
			vtx_offset += g_DrawList[i].vtx_size;
		}
		return TRUE;
	}
	BOOL BeginScene() {
		if (using_old == true) {
		}
		using_old = true;
		ID3D11DeviceContext* ctx = g_pd3dDeviceContext;

		DXGI_SWAP_CHAIN_DESC des;

		RtlZeroMemory(&old, sizeof(old));
		old.ScissorRectsCount = old.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
		ctx->RSGetScissorRects(&old.ScissorRectsCount, old.ScissorRects);
		ctx->RSGetViewports(&old.ViewportsCount, old.Viewports);
		ctx->RSGetState(&old.RS);
		ctx->OMGetRenderTargets(1, &old.RenderTargetView, &old.DepthStencilView);
		ctx->OMGetBlendState(&old.BlendState, old.BlendFactor, &old.SampleMask);
		ctx->OMGetDepthStencilState(&old.DepthStencilState, &old.StencilRef);
		ctx->PSGetShaderResources(0, 1, &old.PSShaderResource);
		ctx->PSGetSamplers(0, 1, &old.PSSampler);
		old.PSInstancesCount = old.VSInstancesCount = 256;
		ctx->PSGetShader(&old.PS, old.PSInstances, &old.PSInstancesCount);
		ctx->VSGetShader(&old.VS, old.VSInstances, &old.VSInstancesCount);
		ctx->VSGetConstantBuffers(0, 1, &old.VSConstantBuffer);
		ctx->IAGetPrimitiveTopology(&old.PrimitiveTopology);
		ctx->IAGetIndexBuffer(&old.IndexBuffer, &old.IndexBufferFormat, &old.IndexBufferOffset);
		ctx->IAGetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset);
		ctx->IAGetInputLayout(&old.InputLayout);

		if (use_static_wh == false) {
			g_width = 0.f;
			g_height = 0.f;
			if (old.ViewportsCount > 0) {
				g_width = (float)(old.Viewports[0].Width);
				g_height = (float)(old.Viewports[0].Height);
			}
			else if (old.ScissorRectsCount > 0) {
				g_width = (float)(old.ScissorRects[0].right - old.ScissorRects[0].left);
				g_height = (float)(old.ScissorRects[0].bottom - old.ScissorRects[0].top);
			}
		}
		//myDbgPrint1("[112233] old.ScissorRectsCount:%d\n", old.ScissorRectsCount);
		//myDbgPrint1("[112233] old.ViewportsCount:%d\n", old.ViewportsCount);
		//myDbgPrint1("[112233] old.PSInstancesCount:%d\n", old.PSInstancesCount);
		//myDbgPrint1("[112233] old.VSInstancesCount:%d\n", old.VSInstancesCount);
		//myDbgPrint2("[112233] old.bottom:%d %d\n", old.ScissorRectsCount, old.ScissorRects[0].bottom);

		SetupRenderState();

		g_vtx_buffer_size = 0;
		g_idx_buffer_size = 0;
		g_DrawListSize = 0;

		return TRUE;
	}
	void GetWH(float* w, float* h) {
		*w = g_width;
		*h = g_height;
	}
	BOOL EndScene() {

		ID3D11DeviceContext* ctx = g_pd3dDeviceContext;

		DrawOut();


		ctx->RSSetScissorRects(old.ScissorRectsCount, old.ScissorRects);
		ctx->RSSetViewports(old.ViewportsCount, old.Viewports);
		ctx->RSSetState(old.RS);
		ctx->OMSetRenderTargets(1, &old.RenderTargetView, old.DepthStencilView);
		ctx->OMSetBlendState(old.BlendState, old.BlendFactor, old.SampleMask);
		ctx->OMSetDepthStencilState(old.DepthStencilState, old.StencilRef);
		ctx->PSSetShaderResources(0, 1, &old.PSShaderResource);
		ctx->PSSetSamplers(0, 1, &old.PSSampler);
		ctx->PSSetShader(old.PS, old.PSInstances, old.PSInstancesCount);
		ctx->VSSetShader(old.VS, old.VSInstances, old.VSInstancesCount);
		ctx->VSSetConstantBuffers(0, 1, &old.VSConstantBuffer);
		ctx->IASetPrimitiveTopology(old.PrimitiveTopology);
		ctx->IASetIndexBuffer(old.IndexBuffer, old.IndexBufferFormat, old.IndexBufferOffset);
		ctx->IASetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset);
		ctx->IASetInputLayout(old.InputLayout);

		//ctx->Unmap(g_VBuffer, 0);
		//ctx->Unmap(g_IBuffer, 0);

		using_old = false;
		return TRUE;
	}
	void FillRect(float x, float y, float w, float h, DWORD col) {
		if (g_DrawListSize >= MAX_DRAWLIST_COUNT)
			return;
		ID3D11DeviceContext *ctx = g_pd3dDeviceContext;
		CVertex vtx[4];
		vtx[0] = CVertex(x, y, col);
		vtx[1] = CVertex(x + w, y, col);
		vtx[2] = CVertex(x + w, y + h, col);
		vtx[3] = CVertex(x, y + h, col);
		PushVtx(&vtx[0]);
		PushVtx(&vtx[1]);
		PushVtx(&vtx[2]);
		PushVtx(&vtx[3]);

		DWORD idx[6];
		idx[0] = 0; idx[1] = 1; idx[2] = 2;
		idx[3] = 0; idx[4] = 3; idx[5] = 2;
		PushIdx(idx[0]);
		PushIdx(idx[1]);
		PushIdx(idx[2]);
		PushIdx(idx[3]);
		PushIdx(idx[4]);
		PushIdx(idx[5]);

		CDrawList Draw;
		RtlZeroMemory(&Draw, sizeof(CDrawList));
		Draw.vtx_size = 4;
		Draw.idx_size = 6;
		Draw.primitive_type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		Draw.tex_id = 0;

		PushDrawList(&Draw);

	}
	void DrawLine(float sx, float sy, float ex, float ey, DWORD col) {
		if (g_DrawListSize >= MAX_DRAWLIST_COUNT)
			return;
		ID3D11DeviceContext *ctx = g_pd3dDeviceContext;

		
		CVertex vtx[2];
		vtx[0] = CVertex(sx, sy , col);
		vtx[1] = CVertex(ex, ey, col);
		PushVtx(&vtx[0]);
		PushVtx(&vtx[1]);

		DWORD idx[2];
		idx[0] = 0;
		idx[1] = 1;
		PushIdx(idx[0]);
		PushIdx(idx[1]);

		CDrawList Draw;
		RtlZeroMemory(&Draw, sizeof(CDrawList));
		Draw.vtx_size = 2;
		Draw.idx_size = 2;
		Draw.primitive_type = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		Draw.tex_id = 0;

		PushDrawList(&Draw);
	}

	void DrawBox(float x, float y, float w, float h, DWORD col) {
		DrawLine(x, y, x + w, y, col);//top
		DrawLine(x, y, x, y + h, col);//left
		DrawLine(x + w, y, x + w, y + h, col);//right
		DrawLine(x, y + h, x + w, y + h, col);//bottom
	}
	void DrawBox_OutLine(float x, float y, float w, float h, DWORD col) {
		DrawLine(x, y - 1.f, x + w, y - 1.f, Color::Black);//top
		DrawLine(x, y, x + w, y, col);//top
		DrawLine(x, y + 1.f, x + w, y + 1.f, Color::Black);//top

		DrawLine(x - 1.f, y, x - 1.f, y + h, Color::Black);//left
		DrawLine(x, y, x, y + h, col);//left
		DrawLine(x + 1.f, y, x + 1.f, y + h, Color::Black);//left

		DrawLine(x + w - 1.f, y, x + w - 1.f, y + h, Color::Black);//right
		DrawLine(x + w, y, x + w, y + h, col);//right
		DrawLine(x + w + 1.f, y, x + w + 1.f, y + h, Color::Black);//right

		DrawLine(x, y + h - 1.f, x + w, y + h - 1.f, Color::Black);//bottom
		DrawLine(x, y + h, x + w, y + h, col);//bottom
		DrawLine(x, y + h + 1.f, x + w, y + h + 1.f, Color::Black);//bottom
	}
	void DrawCornBox(float x, float y, float w, float h, DWORD col) {
		float xm = w / 3;
		float ym = h / 3;

		DrawLine(x, y, x + xm, y, col); DrawLine(x, y, x, y + ym, col);
		DrawLine(x, y + h, x + xm, y + h, col); DrawLine(x, y + h, x, y + h - ym, col);
		DrawLine(x + w, y, x + w - xm, y, col); DrawLine(x + w, y, x + w, y + ym, col);
		DrawLine(x + w, y + h, x + w - xm, y + h, col); DrawLine(x + w, y + h, x + w, y + h - ym, col);
	}
	void DrawCornBoxOutLine(float x, float y, float w, float h, DWORD col) {
		float xm = w / 3;
		float ym = h / 3;

		DrawLine(x, y, x + xm, y, col); DrawLine(x, y, x, y + ym, col);
		DrawLine(x, y + 1, x + xm, y + 1, Color::Black); DrawLine(x + 1, y, x + 1, y + ym, Color::Black);
		DrawLine(x, y - 1, x + xm, y - 1, Color::Black); DrawLine(x - 1, y, x - 1, y + ym, Color::Black);

		DrawLine(x, y + h, x + xm, y + h, col); DrawLine(x, y + h, x, y + h - ym, col);
		DrawLine(x, y + h + 1, x + xm, y + h + 1, Color::Black); DrawLine(x + 1, y + h, x + 1, y + h - ym, Color::Black);
		DrawLine(x, y + h - 1, x + xm, y + h - 1, Color::Black); DrawLine(x - 1, y + h, x - 1, y + h - ym, Color::Black);

		DrawLine(x + w, y, x + w - xm, y, col); DrawLine(x + w, y, x + w, y + ym, col);
		DrawLine(x + w, y + 1, x + w - xm, y + 1, Color::Black); DrawLine(x + w + 1, y, x + w + 1, y + ym, Color::Black);
		DrawLine(x + w, y - 1, x + w - xm, y - 1, Color::Black); DrawLine(x + w - 1, y, x + w - 1, y + ym, Color::Black);

		DrawLine(x + w, y + h, x + w - xm, y + h, col); DrawLine(x + w, y + h, x + w, y + h - ym, col);
		DrawLine(x + w, y + h + 1, x + w - xm, y + h + 1, Color::Black); DrawLine(x + w + 1, y + h, x + w + 1, y + h - ym, Color::Black);
		DrawLine(x + w, y + h - 1, x + w - xm, y + h - 1, Color::Black); DrawLine(x + w - 1, y + h, x + w - 1, y + h - ym, Color::Black);

	}
	void DrawRainbowLine(float sx, float sy, float ex, float ey) {
		if (g_DrawListSize >= MAX_DRAWLIST_COUNT)
			return;

		ID3D11DeviceContext *ctx = g_pd3dDeviceContext;

		CVertex vtx[4];
		vtx[0] = CVertex(sx, sy, 0xFF7FFF00);
		vtx[1] = CVertex(sx + 1, sy + 1, 0xFFFF0000);
		vtx[2] = CVertex(ex + 1, ey + 1, 0xFF0000FF);
		vtx[3] = CVertex(ex, ey, 0xFF00FF7F);
		PushVtx(&vtx[0]);
		PushVtx(&vtx[1]);
		PushVtx(&vtx[2]);
		PushVtx(&vtx[3]);

		DWORD idx[6];
		idx[0] = 0; idx[1] = 1; idx[2] = 2;
		idx[3] = 0; idx[4] = 3; idx[5] = 2;
		PushIdx(idx[0]);
		PushIdx(idx[1]);
		PushIdx(idx[2]);
		PushIdx(idx[3]);
		PushIdx(idx[4]);
		PushIdx(idx[5]);

		CDrawList Draw;
		RtlZeroMemory(&Draw, sizeof(CDrawList));
		Draw.vtx_size = 4;
		Draw.idx_size = 6;
		Draw.primitive_type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		Draw.tex_id = 0;

		PushDrawList(&Draw);

	}
	void DrawRainbowBox(float x, float y, float w, float h) {
		DrawRainbowLine(x, y, x + w, y);//top
		DrawRainbowLine(x, y, x, y + h);//left
		DrawRainbowLine(x + w, y, x + w, y + h);//right
		DrawRainbowLine(x, y + h, x + w, y + h);//bottom
	}
	void DrawCircle(float centerx, float centery, float rad, long pointCount, DWORD col) {
		float interval = 3.1415926535f * 2 / pointCount;
		//float lineLen = tan(interval / 2)*rad * 2;
		float sx, sy, ex, ey;
		for (long i = 1; i < pointCount + 1; i++) {
			sx = centerx + sin(interval*i)*rad;
			sy = centery + cos(interval*i)*rad;
			ex = centerx + sin(interval*(i + 1))*rad;
			ey = centery + cos(interval*(i + 1))*rad;
			DrawLine(sx, sy, ex, ey, col);
		}
	}
	void DrawChar(float x, float y, CFontGlyph *Glyph, DWORD col) {
		if (g_DrawListSize >= MAX_DRAWLIST_COUNT)
			return;
		ID3D11DeviceContext *ctx = g_pd3dDeviceContext;
		ID3D11ShaderResourceView *texid = (ID3D11ShaderResourceView*)Glyph->TexId;
		if (!texid)return;

		float width = Glyph->Width;
		float height = Glyph->Height;

		CVertex vtx[4];
		vtx[0] = CVertex(x, y, 0.01f, 0.01f, col);//左上
		vtx[1] = CVertex(x + width, y, 0.99f, 0.01f, col);//右上
		vtx[2] = CVertex(x + width, y + height, 0.99f, 0.99f, col);//右下
		vtx[3] = CVertex(x, y + height, 0.01f, 0.99f, col);//左下
		//vtx[0] = CVertex(x, y, 0.00f, 0.00f, col);//左上
		//vtx[1] = CVertex(x + width, y, 1.f, 0.00f, col);//右上
		//vtx[2] = CVertex(x + width, y + height, 1.f, 1.f, col);//右下
		//vtx[3] = CVertex(x, y + height, 0.00f, 1.f, col);//左下
		PushVtx(&vtx[0]);
		PushVtx(&vtx[1]);
		PushVtx(&vtx[2]);
		PushVtx(&vtx[3]);

		DWORD idx[6];
		idx[0] = 0; idx[1] = 1; idx[2] = 2;
		idx[3] = 0; idx[4] = 3; idx[5] = 2;
		PushIdx(idx[0]);
		PushIdx(idx[1]);
		PushIdx(idx[2]);
		PushIdx(idx[3]);
		PushIdx(idx[4]);
		PushIdx(idx[5]);

		CDrawList Draw;
		RtlZeroMemory(&Draw, sizeof(CDrawList));
		Draw.vtx_size = 4;
		Draw.idx_size = 6;
		Draw.primitive_type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		Draw.tex_id = texid;
		
		PushDrawList(&Draw);
	}

	float GetStringPixelLengthW(LPCWSTR Str, ULONG Size) {
		Size = (ULONG)((float)Size / 1.3);
		if (Size > g_MaxSize)
			Size = g_MaxSize;
		int len = wcslen(Str);
		float left = 0.f;
		float fSize = (float)Size;
		for (int i = 0; i < len; i++) {
			if (Str[i] == L' ') {
				left += Size / 2;
				continue;
			}
			CFontGlyph* Glyph = GetFontGlyph(Str[i], Size);
			//printf("TexId:%p\n", Glyph->TexId);
			if (i)
				left += (float)Glyph->Left;
			left += (float)Glyph->Width + 1;
		}
		return left;
	}
	float GetStringPixelLengthA(LPCSTR Str, ULONG Size) {
		static WCHAR wstr[300];
		RtlZeroMemory(wstr, sizeof(wstr));
		DxUTF8ToUnicode(Str, wstr, 260);
		return GetStringPixelLengthW(wstr, Size);
	}
	void DrawStringW(float x, float y, LPCWSTR Str, ULONG uSize, DWORD col) {
		float Size = (float)uSize / 1.3;
		if (Size > g_MaxSize)
			Size = g_MaxSize;
		int len = wcslen(Str);
		float left = 0.f;
		float fSize = (float)Size;
		for (int i = 0; i < len; i++) {
			if (Str[i] == L' ') {
				left += Size / 2;
				continue;
			}
			CFontGlyph *Glyph = GetFontGlyph(Str[i], Size);
			//printf("TexId:%p\n", Glyph->TexId);
			left += (float)Glyph->Left;
			DrawChar(x + left, y - (float)Glyph->Top + Size, Glyph, col);
			left += (float)Glyph->Width + 1;
		}
	}
	void DrawStringA(float x, float y, LPCSTR Str, ULONG Size, DWORD col) {
		static WCHAR wstr[300];
		RtlZeroMemory(wstr, sizeof(wstr));
		DxUTF8ToUnicode(Str, wstr, 260);
		DrawStringW(x, y, wstr, Size, col);
	}
	void DrawStringStrokeW(float x, float y, LPCWSTR Str, ULONG uSize, DWORD col, DWORD StrokeColor) {
		float Size = (float)uSize / 1.3;
		if (Size > g_MaxSize)
			Size = g_MaxSize;
		int len = wcslen(Str);
		float left = 0.f;
		float fSize = (float)Size;
		for (int i = 0; i < len; i++) {
			if (Str[i] == L' ') {
				left += Size / 2;
				continue;
			}
			CFontGlyph *Glyph = GetFontGlyph(Str[i], Size);
			//printf("TexId:%p\n", Glyph->TexId);
			left += (float)Glyph->Left;

			DrawChar(x + left - 1.f, y - Glyph->Top + Size, Glyph, StrokeColor);
			DrawChar(x + left + 1.f, y - Glyph->Top + Size, Glyph, StrokeColor);

			DrawChar(x + left, y - Glyph->Top + Size - 1.f, Glyph, StrokeColor);
			DrawChar(x + left, y - Glyph->Top + Size + 1.f, Glyph, StrokeColor);

			DrawChar(x + left, y - (float)Glyph->Top + Size, Glyph, col);

			left += (float)Glyph->Width + 1;
		}

	}
	void DrawStringStrokeA(float x, float y, LPCSTR Str, ULONG Size, DWORD col, DWORD StrokeColor) {
		static WCHAR wstr[300];
		RtlZeroMemory(wstr, sizeof(wstr));
		DxUTF8ToUnicode(Str, wstr, 260);

		DrawStringStrokeW(x, y, wstr, Size, col, StrokeColor);
	}
	void DrawSoildTriangle(float x1, float y1, float x2, float y2, float x3, float y3, DWORD col) {
		if (g_DrawListSize >= MAX_DRAWLIST_COUNT)
			return;

		CVertex vtx[3];
		vtx[0] = CVertex(x1, y1, col);
		vtx[1] = CVertex(x2, y2, col);
		vtx[2] = CVertex(x3, y3, col);
		PushVtx(&vtx[0]);
		PushVtx(&vtx[1]);
		PushVtx(&vtx[2]);

		DWORD idx[3];
		idx[0] = 0;
		idx[1] = 1;
		idx[2] = 2;
		PushIdx(idx[0]);
		PushIdx(idx[1]);
		PushIdx(idx[2]);

		CDrawList Draw;
		RtlZeroMemory(&Draw, sizeof(CDrawList));
		Draw.vtx_size = 3;
		Draw.idx_size = 3;
		Draw.primitive_type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		Draw.tex_id = 0;

		PushDrawList(&Draw);
	}
	void DrawSoildCircle(float centerx, float centery, float rad, long pointCount, DWORD col) {
		float interval = 3.1415926535f * 2 / pointCount;
		//float lineLen = tan(interval / 2)*rad * 2;
		float x1, y1, x2, y2, x3, y3;
		for (long i = 1; i < pointCount + 1; i++) {
			x1 = centerx;
			y1 = centery;
			x2 = centerx + sin(interval * i) * rad;
			y2 = centery + cos(interval * i) * rad;
			x3 = centerx + sin(interval * (i + 1)) * rad;
			y3 = centery + cos(interval * (i + 1)) * rad;
			DrawSoildTriangle(x1, y1, x2, y2, x3, y3, col);
		}
	}

	void DrawVerticalHealthLine(float x, float y, float w, float h, int health, int max_health, DWORD HealthColor) {
		DrawBox(x, y, w, h, Color::Black);
		float l_health = health;
		float l_max_health = max_health;
		if (health > max_health)l_health = l_max_health;
		float abs = (float)l_health / (float)l_max_health;
		float startx = x + 1;
		float starty = y + h - 1;
		float high = -h + 2;
		float width = w - 2;
		high *= abs;
		FillRect(startx, starty, width, high, HealthColor);
	}

}



//#pragma optimize( "", on )
