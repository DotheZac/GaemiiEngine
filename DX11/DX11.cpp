#include <iostream>
#include "DX11.h"

ID3D11Device* g_pDevice = nullptr;
ID3D11DeviceContext* g_pDXDC = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_pRTView = nullptr;

BOOL g_bVSync = FALSE;


HRESULT CreateDeviceSwapChain(HWND hWnd);
HRESULT CreateRenderTarget();
void SetViewPort();

bool DXSetup(HWND hWnd)
{
    CreateDeviceSwapChain(hWnd);
    CreateRenderTarget();
    //CreateDepthStencil();
    //스텐실 넣으면 바뀌어야함
    //nullptr부분에 뎁스스텐실뷰 포인터 넣기
    g_pDXDC->OMSetRenderTargets(1, &g_pRTView, nullptr);
    SetViewPort();
    return true;
}

void Draw()
{
    float clearColor[4] = { 0.2f, 0.2f, 0.6f, 1.0f }; // 파란색 배경
    g_pDXDC->ClearRenderTargetView(g_pRTView, clearColor);

    g_pSwapChain->Present(1, 0);
}

void DXRelease()
{
    if (g_pDXDC)
    {
        g_pDXDC->ClearState();
    }
    g_pRTView->Release();
    g_pSwapChain->Release();
    g_pDXDC->Release();
    g_pDevice->Release();

}

DWORD AlignCBSize(DWORD size)
{
    DWORD sizeAligned = 0;
    BOOL bAligned = (size % 16) ? FALSE : TRUE;		//정렬(필요) 확인.
    TCHAR dbgMsg[256] = _T("");						//디버깅 메세지.


    if (bAligned)
    {
        sizeAligned = size;

        //_stprintf(dbgMsg, _T("[알림] 상수버퍼 : 16바이트 정렬됨. \n> ConstBuffer = %d \n> 필요 정렬 크기 = %d"), size, sizeAligned);
    }
    else
    {
        sizeAligned = (size / 16) * 16 + 16;		//정렬(필요) 크기 재산출.

        //_stprintf(dbgMsg, _T("[경고] 상수버퍼 : 16바이트 미정렬. \n> ConstBuffer = %d \n> 필요 정렬 크기 = %d"), size, sizeAligned);
    }

    return sizeAligned;
}

void CreateDynamicConstantBuffer(ID3D11Device* pDev, UINT size, ID3D11Buffer** ppCB)
{
    HRESULT hr = S_OK;
    ID3D11Buffer* pCB = nullptr;

    //정렬된 버퍼 크기 계산 
    DWORD sizeAligned = AlignCBSize(size);

    //상수 버퍼 정보 설정.
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;				//동적 정점버퍼 설정.
    bd.ByteWidth = sizeAligned;						//버퍼 크기 : 128비트 정렬 추가.
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;				//CPU 접근 설정. 

    /*//서브리소스 설정.
    D3D11_SUBRESOURCE_DATA sd;
    sd.pSysMem = pData;										//상수 데이터 설정.
    sd.SysMemPitch = 0;
    sd.SysMemSlicePitch = 0;
    */

    //상수 버퍼 생성.
    hr = pDev->CreateBuffer(&bd, nullptr, &pCB);

    //외부로 전달.
    *ppCB = pCB;

}

void UpdateDynamicBuffer(ID3D11DeviceContext* pDXDC, ID3D11Resource* pBuff, LPVOID pData, UINT size)
{
    D3D11_MAPPED_SUBRESOURCE mr = {};
    mr.pData = nullptr;

    pDXDC->Map(pBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);

    memcpy(mr.pData, pData, size);
    pDXDC->Unmap(pBuff, 0);
}

float GetEngineTime()
{
    static ULONGLONG oldtime = GetTickCount64();
    ULONGLONG 		 nowtime = GetTickCount64();
    float dTime = (nowtime - oldtime) * 0.001f;
    oldtime = nowtime;

    return dTime;
}

HRESULT CreateDeviceSwapChain(HWND hWnd)
{
    HRESULT hr = 1;
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;

    hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr, 0,
        D3D11_SDK_VERSION,
        &sd,
        &g_pSwapChain,
        &g_pDevice,
        nullptr,
        &g_pDXDC
    );

    if (FAILED(hr))
    {
        std::cerr << "Error: CreateDeviceSwapChain" << std::endl;
    }

    return hr;
}

HRESULT CreateRenderTarget()
{
    HRESULT hr = 1;

    ID3D11Texture2D* backBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

    if (FAILED(hr))	return hr;


    hr = g_pDevice->CreateRenderTargetView(backBuffer, nullptr, &g_pRTView);

    if (FAILED(hr))
    {
        std::cerr << "Error: CreateRenderTarget" << std::endl;

    }

    backBuffer->Release();

    return hr;
}

void SetViewPort()
{
    D3D11_VIEWPORT vp = {};
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = 800;
    vp.Height = 600;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    g_pDXDC->RSSetViewports(1, &vp);
}
