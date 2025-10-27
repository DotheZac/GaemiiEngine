#include "DX11.h"
#include "Effect.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler")

Effect::Effect()
{
	m_Name = L"N/A";
	m_FileName = L"N/A";
	m_Profile = L"N/A";
	m_pTexName = L"N/A";
	m_bIsDirty = true;

}

Effect::~Effect()
{
	Release();
}

int Effect::Create(ID3D11Device* pDev, const TCHAR* filename)
{
	m_pDev = pDev;
	m_pDev->GetImmediateContext(m_pDXDC.GetAddressOf());

	Load(filename);

	ZeroMemory(&m_BCBuffer, sizeof(BaseConstBuffer));
	//CreateDynaConstBuffer(sizeof(BaseConstBuffer), &m_BCBuffer, m_pCB.GetAddressOf());
	CreateConstBuffer(sizeof(BaseConstBuffer), m_pCB.GetAddressOf());

	Update();

	return 0;
}

//샘플할 이미지가 있을 경우
int Effect::Create(ID3D11Device* pDev, const TCHAR* shaderfilename, const TCHAR* resourceFileName)
{
	return 0;
}


int Effect::Load(const TCHAR* filename)
{
	HRESULT hr = S_OK;

	ID3D11VertexShader* pVS = nullptr;
	ID3DBlob* pVSCode = nullptr;
	hr = Compile(filename, "VS_Main", "vs_5_0", &pVSCode);
	if (FAILED(hr))
	{
		ERROR_MSG(hr);
		return hr;
	}

	hr = g_pDevice->CreateVertexShader(pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), nullptr, &pVS);
	if (FAILED(hr))
	{
		SafeRelease(pVSCode);
		return hr;
	}

	ID3D11PixelShader* pPS = nullptr;
	ID3DBlob* pPSCode = nullptr;
	hr = Compile(filename, "PS_Main", "ps_5_0", &pPSCode);
	if (FAILED(hr))
	{
		ERROR_MSG(hr);
		return hr;
	}

	hr = g_pDevice->CreatePixelShader(pPSCode->GetBufferPointer(), pPSCode->GetBufferSize(), nullptr, &pPS);
	if (FAILED(hr))
	{
		SafeRelease(pVSCode);
		SafeRelease(pPSCode);
		return hr;
	}
	SafeRelease(pPSCode);

	m_pVS = pVS;
	m_pPS = pPS;
	m_pVSCode = pVSCode;
	m_FileName = filename;
	m_Profile = L"5.0";

	return S_OK;
}

HRESULT Effect::Compile(const TCHAR* FileName, const char* EntryPoint, const char* ShaderModel, ID3DBlob** ppCode)
{
	HRESULT hr = S_OK;
	ID3DBlob* pError = nullptr;

	//컴파일 옵션1.
	UINT Flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;		//열우선 행렬 처리. 구형 DX9 이전까지의 전통적인 방식. 속도가 요구된다면, "행우선" 으로 처리할 것.
	//UINT Flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;	//행우선 행렬 처리. 열 우선 처리보다 속도의 향상이 있지만, 행렬을 전치한수 GPU 에 공급해야 한다.
	//UINT Flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
	Flags |= D3DCOMPILE_DEBUG;							//디버깅 모드시 옵션 추가.
#endif

	//셰이더 소스 컴파일.
	hr = D3DCompileFromFile(FileName,
		nullptr, nullptr,
		EntryPoint,
		ShaderModel,
		Flags,						//컴파일 옵션.1
		0,							//컴파일 옵션2,  Effect 파일 컴파일시 적용됨. 이외에는 무시됨.
		ppCode,						//[출력] 컴파일된 셰이더 코드.
		&pError						//[출력] 컴파일 에러 코드.
	);
	if (FAILED(hr))
	{
		ERROR_MSG(hr);
		return hr;
	}

	SafeRelease(pError);
	return hr;
}

int Effect::CreateLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	HRESULT hr = S_OK;
	ID3D11InputLayout* pLayout = nullptr;
	hr = g_pDevice->CreateInputLayout(layout,
		numElements,
		m_pVSCode->GetBufferPointer(),	
		m_pVSCode->GetBufferSize(),		
		&pLayout
	);
	if (FAILED(hr))
	{
		ERROR_MSG(hr);
		return hr;
	}


	m_pLayout = pLayout;

	return hr;
}

HRESULT Effect::CreateConstBuffer(UINT size, ID3D11Buffer** ppCB)
{
	HRESULT hr = S_OK;
	UINT alignSize = AlignCBSize(size);

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = alignSize;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ID3D11Buffer* pCB = nullptr;
	hr = g_pDevice->CreateBuffer(&bd, nullptr, &pCB);
	if (FAILED(hr))
	{
		ERROR_MSG(hr);
		return hr;
	}

	*ppCB = pCB;

	return hr;
}

//HRESULT Effect::CreateDynaConstBuffer(UINT size, LPVOID pData, ID3D11Buffer** ppCB)
//{
//	return E_NOTIMPL;
//}
//
//HRESULT Effect::UpdateDynaConstBuffer(ID3D11DeviceContext* pDXDC, ID3D11Resource* pBuff, LPVOID pData, UINT size)
//{
//	return E_NOTIMPL;
//}

int Effect::Update(float dTime)
{
	UpdateCB();
	m_pDXDC->VSSetConstantBuffers(0, 1, m_pCB.GetAddressOf());

	return 0;
}

int Effect::Apply(float dTime)
{
	m_pDXDC->VSSetShader(m_pVS.Get(), nullptr, 0);
	m_pDXDC->PSSetShader(m_pPS.Get(), nullptr, 0);

	return 0;
}

void Effect::Release()
{
}

int Effect::UpdateCB()
{
	if (!m_bIsDirty)
	{
		return 0;
	}

	XMMATRIX mV = XMLoadFloat4x4(&m_BCBuffer.mView);
	XMMATRIX mP = XMLoadFloat4x4(&m_BCBuffer.mProj);

	XMMATRIX mVP = mV * mP;
	XMStoreFloat4x4(&m_BCBuffer.mVP, mVP);
	
	m_pDXDC->UpdateSubresource(m_pCB.Get(), 0, nullptr, &m_BCBuffer, 0,0);

	m_bIsDirty = false;
	return 0;
}

//미구현
int Effect::UpdateCB(const XMMATRIX& mTM)
{
	return 0;
}

//미구현
int Effect::UpdateCB(const XMFLOAT4X4& mTM)
{
	return 0;
}

int Effect::SetView(const XMMATRIX& pTM)
{
	XMFLOAT4X4 mTM;
	XMStoreFloat4x4(&mTM, pTM);

	SetView(mTM);

	return 0;
}

int Effect::SetView(const XMFLOAT4X4& pTM)
{
	m_BCBuffer.mView = pTM;

	m_bIsDirty = true;
	return 0;
}

int Effect::SetProj(const XMMATRIX& pTM)
{
	XMFLOAT4X4 mTM;
	XMStoreFloat4x4(&mTM, pTM);

	SetProj(mTM);

	return 0;
}

int Effect::SetProj(const XMFLOAT4X4& pTM)
{
	m_BCBuffer.mProj = pTM;

	m_bIsDirty = true;
	return 0;
}

void Effect::Info(int x, int y, COLOR col, float dTime)
{
}

int EffectCreate(ID3D11Device* pDev, const TCHAR* filename, std::shared_ptr<Effect>& outEffect)
{
	auto pEffect = std::make_shared<Effect>();
	if (!pEffect)
	{
		ERROR_MSG(E_FAIL);
		return -1;
	}

	pEffect->Create(pDev, filename);

	outEffect = pEffect;

	return 0;
}
