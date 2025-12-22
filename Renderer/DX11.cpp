#include <iostream>
#include "DX11.h"

ComPtr<ID3D11Device>            g_pDevice;
ComPtr<ID3D11DeviceContext>     g_pDXDC;
ComPtr<IDXGISwapChain>          g_pSwapChain; 
ComPtr<ID3D11RenderTargetView>  g_pRTView; 
ComPtr<ID3D11Texture2D>         g_pDS;


DXGI_ADAPTER_DESC1 g_Adc;

BOOL g_bVSync = FALSE;

//Depth/Stencil
ComPtr<ID3D11DepthStencilView> g_pDSView;
ComPtr<ID3D11DepthStencilState> g_DSState[static_cast<int>(DS::MAX)];
int  DepthStencilStateCreate();


HRESULT CreateDeviceSwapChain(HWND hWnd);
HRESULT CreateRenderTarget();
HRESULT CreateDepthStencil();
void SetViewPort();

//모니터 해상도 관련
ComPtr<IDXGIAdapter> g_pAdapter;
//ComPtr<IDXGIOutput> g_pOutput;
int g_MonitorWidth = 0;
int g_MonitorHeight = 0;
DISPLAY g_DisPlay = { 960,  600, 0, 1, DXGI_FORMAT_R8G8B8A8_UNORM };


//안티에일리어싱
DWORD		g_dwAA = 1;
DWORD		g_dwAF = 1;
BOOL		g_bMipMap = TRUE;



bool DXSetup(HWND hWnd)
{
    CreateDeviceSwapChain(hWnd);
    CreateRenderTarget();
    CreateDepthStencil();
    //스텐실 넣으면 바뀌어야함
    //nullptr부분에 뎁스스텐실뷰 포인터 넣기
    g_pDXDC->OMSetRenderTargets(1, g_pRTView.GetAddressOf(), g_pDSView.Get());
    SetViewPort();
    DepthStencilStateCreate();
    return true;
}

void Draw()
{
    float clearColor[4] = { 0.2f, 0.2f, 0.6f, 1.0f }; // 파란색 배경
    g_pDXDC->ClearRenderTargetView(g_pRTView.Get(), clearColor);

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

int ClearBackBuffer(COLOR col)
{
    g_pDXDC->ClearRenderTargetView(g_pRTView.Get(), (float*)&col);

    return S_OK;
}


int ClearBackBuffer(UINT flag, COLOR col, float depth, UINT stencil)
{
    g_pDXDC->ClearRenderTargetView(g_pRTView.Get(), (float*)&col);
    g_pDXDC->ClearDepthStencilView(g_pDSView.Get(), flag, depth, stencil);	//깊이/스텐실 지우기.

    return 0;
}

int Flip()
{
    g_pSwapChain->Present(g_bVSync, 0);			//화면출력 : Flip! (+수직동기화)

    return 0;
}

void GetDeviceInfo()
{
    //GetFeatureLevel();

    GetAdapterInfo(&g_Adc);

    IDXGIDevice* dxgiDevice = nullptr;
    g_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

    dxgiDevice->GetAdapter(g_pAdapter.GetAddressOf());

    IDXGIOutput* output = nullptr;
    g_pAdapter->EnumOutputs(0, &output);

    DXGI_OUTPUT_DESC desc;
    output->GetDesc(&desc);

    g_MonitorWidth = desc.DesktopCoordinates.right - desc.DesktopCoordinates.left;
    g_MonitorHeight = desc.DesktopCoordinates.bottom - desc.DesktopCoordinates.top;


}

HRESULT GetAdapterInfo(DXGI_ADAPTER_DESC1* pAd)
{
    IDXGIAdapter1* pAdapter;
    IDXGIFactory1* pFactory = NULL;

    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&pFactory))))
    {
        return E_FAIL;
    }

    pFactory->EnumAdapters1(0, &pAdapter);		//어뎁터 획득.
    pAdapter->GetDesc1(pAd);					//어뎁터 정보 획득.
    //*pAd = ad;								//외부로 복사.

    //정보 취득후, 접근한 인터페이스 해제. (메모리 누수 방지)
    SafeRelease(pAdapter);
    SafeRelease(pFactory);

    return S_OK;
}

void SystemUpdate(float dTime)
{
    //bool before = *fullscreen;
    //g_pSwapChain->GetFullscreenState(fullscreen, nullptr);

    //if (IsKeyUp(VK_SPACE))	g_bWireFrame ^= TRUE;
    //if (IsKeyUp(VK_F4))		g_bCullBack ^= TRUE;


    //if (g_bWireFrame) g_BkColor = g_ColDGray;
    //else			  g_BkColor = g_ColGray;


    //// 렌더링 모드 전환.	  
    //RenderModeUpdate();

    //// 깊이 연산 모드 전환.	 
    //if (g_bZEnable)
    //    g_pDXDC->OMSetDepthStencilState(g_DSState[DS_DEPTH_ON], 0);	//깊이 버퍼 동작 (기본값) 
    //else  g_pDXDC->OMSetDepthStencilState(g_DSState[DS_DEPTH_OFF], 0);	//깊이 버퍼 비활성화 : Z-Test Off + Z-Write Off.

    g_pDXDC->OMSetDepthStencilState(g_DSState[static_cast<int>(DS::OFF)].Get(), 0);

}

int CreateBuffer(ID3D11Device* pDev, UINT size, ID3D11Buffer** ppBuff)
{
    return S_OK;
}

int UpdateBuffer(ID3D11Buffer* pBuff, LPVOID pData, UINT size)
{
    return S_OK;
}

int CreateVertexBuffer(ID3D11Device* pDev, LPVOID pData, UINT size, UINT stride, ID3D11Buffer** ppVB)
{
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;			
    bd.ByteWidth = size;							
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;		
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA rd;
    ZeroMemory(&rd, sizeof(rd));
    rd.pSysMem = pData;									

    ID3D11Buffer* pVB = nullptr;
    hr = g_pDevice->CreateBuffer(&bd, &rd, &pVB);
    if (FAILED(hr))
    {
        ERROR_MSG(hr);
        return hr;
    }

    *ppVB = pVB;

    return S_OK;
}

int CreateIndexBuffer(ID3D11Device* pDev, LPVOID pData, UINT size, ID3D11Buffer** ppIB)
{
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bd = {};

    bd.Usage = D3D11_USAGE_DEFAULT;			
    bd.ByteWidth = size;					
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;	
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA rd;
    ZeroMemory(&rd, sizeof(rd));
    rd.pSysMem = pData;									

    ID3D11Buffer* pIB = nullptr;
    hr = g_pDevice->CreateBuffer(&bd, &rd, &pIB);
    if (FAILED(hr))
    {
        ERROR_MSG(hr);
        return hr;
    }

    *ppIB = pIB;

    return S_OK;
}

int CreateConstantBuffer(ID3D11Device* pDev, UINT size, ID3D11Buffer** ppCB)
{
    HRESULT hr = S_OK;

    DWORD sizeAligned = AlignCBSize(size);

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;				
    bd.ByteWidth = sizeAligned;						
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		

    ID3D11Buffer* pCB = nullptr;
    hr = pDev->CreateBuffer(&bd, nullptr, &pCB);
    if (FAILED(hr))
    {
        ERROR_MSG(hr);
        return hr;
    }

    *ppCB = pCB;

    return S_OK;
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

int CreateInputLayout(ID3D11Device* pDev, D3D11_INPUT_ELEMENT_DESC* ed, DWORD num, ID3DBlob* pVSCode, ID3D11InputLayout** ppLayout)
{
    HRESULT hr = S_OK;

    // 정접 입력구조 객체 생성 Create the input layout
    // 함께 사용될 셰이더(컴파일된 바이너리 코드)가 필요합니다.
    ID3D11InputLayout* pLayout = nullptr;
    hr = pDev->CreateInputLayout(ed, num, pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &pLayout);
    if (FAILED(hr))
    {
        ERROR_MSG(hr);
        return hr;
    }

    //외부로 리턴.
    *ppLayout = pLayout;

    return S_OK;
}

HRESULT CreateDynamicConstantBuffer(ID3D11Device* pDev, UINT size, ID3D11Buffer** ppCB)
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
    if (FAILED(hr))
    {
        ERROR_MSG(hr);
        return hr;
    }

    //외부로 전달.
    *ppCB = pCB;

    return hr;
}

HRESULT CreateDynamicConstantBuffer(ID3D11Device* pDev, UINT size, LPVOID pData, ID3D11Buffer** ppCB)
{
    HRESULT hr = S_OK;
    ID3D11Buffer* pCB = nullptr;

    DWORD sizeAligned = AlignCBSize(size);

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;				
    bd.ByteWidth = sizeAligned;						
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;				

    D3D11_SUBRESOURCE_DATA sd;
    sd.pSysMem = pData;							
    sd.SysMemPitch = 0;
    sd.SysMemSlicePitch = 0;

    hr = pDev->CreateBuffer(&bd, &sd, &pCB);
    if (FAILED(hr))
    {
        ERROR_MSG(hr);
        return hr;
    }

    *ppCB = pCB;

    return S_OK;
}

HRESULT UpdateDynamicBuffer(ID3D11DeviceContext* pDXDC, ID3D11Resource* pBuff, LPVOID pData, UINT size)
{
    HRESULT hr = S_OK;

    //DWORD sizeAligned = AlignCBSize(size);


    D3D11_MAPPED_SUBRESOURCE mr = {};
    mr.pData = nullptr;							

    //버퍼 버퍼 접근
    hr = pDXDC->Map(pBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
    if (FAILED(hr))
    {
        ERROR_MSG(hr);
        return hr;
    }

    memcpy(mr.pData, pData, size);				
    pDXDC->Unmap(pBuff, 0);			

    return S_OK;
}

float GetEngineTime()
{
    static ULONGLONG oldtime = GetTickCount64();
    ULONGLONG 		 nowtime = GetTickCount64();
    float dTime = (nowtime - oldtime) * 0.001f;
    oldtime = nowtime;

    return dTime;
}

int DepthStencilStateCreate()
{
    D3D11_DEPTH_STENCIL_DESC  ds;
    ds.DepthEnable = TRUE;
    ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    ds.DepthFunc = D3D11_COMPARISON_LESS;
    ds.StencilEnable = FALSE;
    ds.DepthEnable = TRUE;				
    ds.StencilEnable = FALSE;				
    g_pDevice->CreateDepthStencilState(&ds, g_DSState[static_cast<int>(DS::ON)].GetAddressOf());

    ds.DepthEnable = FALSE;
    g_pDevice->CreateDepthStencilState(&ds, g_DSState[static_cast<int>(DS::OFF)].GetAddressOf());

    return S_OK;
}

HRESULT CreateDeviceSwapChain(HWND hWnd)
{
    HRESULT hr = S_OK;
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
        g_pSwapChain.GetAddressOf(),
        g_pDevice.GetAddressOf(),
        nullptr,
        g_pDXDC.GetAddressOf()
    );

    if (FAILED(hr))
    {
        ERROR_MSG(hr);
        return hr;
    }

    return hr;
}

HRESULT CreateRenderTarget()
{
    HRESULT hr = S_OK;

    ComPtr<ID3D11Texture2D> backBuffer;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));

    if (FAILED(hr))	
    {
        ERROR_MSG(hr);
        return hr;
    }


    hr = g_pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, g_pRTView.GetAddressOf());

    if (FAILED(hr))
    {
        ERROR_MSG(hr);
        return hr;
    }

    backBuffer->Release();

    return hr;
}

HRESULT CreateDepthStencil()
{
    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC   td = {};
    td.Width                    = g_DisPlay.Width;
    td.Height                   = g_DisPlay.Height;
    td.MipLevels                = 1;
    td.ArraySize                = 1;
    td.Format                   = DXGI_FORMAT_D32_FLOAT;
    td.SampleDesc.Count         = g_dwAA;
    td.SampleDesc.Quality       = 0;
    td.Usage                    = D3D11_USAGE_DEFAULT;
    td.BindFlags                = D3D11_BIND_DEPTH_STENCIL;
    td.CPUAccessFlags           = 0;
    td.MiscFlags                = 0;
    hr = g_pDevice->CreateTexture2D(&td, NULL, &g_pDS);
    if (FAILED(hr))
    {
        ERROR_MSG(hr);
        return hr;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC  dd = {};
    dd.Format = td.Format;
    dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;		//AA 없음.
    //dd.ViewDimension  = D3D11_DSV_DIMENSION_TEXTURE2DMS;	//+AA 설정 "MSAA"

    dd.Texture2D.MipSlice = 0;

    hr = g_pDevice->CreateDepthStencilView(g_pDS.Get(), &dd, &g_pDSView);
    if (FAILED(hr))
    {
        ERROR_MSG(hr);
        return hr;
    }

    return hr;
}

void SetViewPort()
{
    D3D11_VIEWPORT vp = {};
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = (float)g_DisPlay.Width;
    vp.Height = (float)g_DisPlay.Height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    g_pDXDC->RSSetViewports(1, &vp);
}
