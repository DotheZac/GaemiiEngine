#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D11")		

#include <Windows.h>
#include <d3d11.h>
#include <tchar.h>
#include <wrl/client.h>
#include "DirectXMath.h"

using Microsoft::WRL::ComPtr;

using namespace DirectX;

typedef DirectX::XMFLOAT4 COLOR;


extern	ID3D11Device* g_pDevice;
extern	ID3D11DeviceContext* g_pDXDC;
extern	IDXGISwapChain* g_pSwapChain;
extern	ID3D11RenderTargetView* g_pRTView;
extern	BOOL 		g_bVSync;


bool DXSetup(HWND hWnd);
void Draw();
void DXRelease();

DWORD	AlignCBSize(DWORD size);

void CreateDynamicConstantBuffer(ID3D11Device* pDev, UINT size, ID3D11Buffer** ppCB);
void UpdateDynamicBuffer(ID3D11DeviceContext* pDXDC, ID3D11Resource* pBuff, LPVOID pData, UINT size);
float GetEngineTime();