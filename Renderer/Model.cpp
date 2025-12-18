#include "DX11.h"
#include "Effect.h"
#include "Model.h"
// TODO: 라이브러리 함수의 예제입니다.

Model::VTX g_TestModel[] =
{
		{ -0.5f,  0.0f, 0.0f },
		{  0.0f,  1.0f, 0.0f },
		{  0.5f,  0.0f, 0.0f }

};

UINT g_TestModelSize = sizeof(g_TestModel);

Model::Model()
{
	m_Name = L"N/A";
	m_buffSize = 0;
	m_Stride = 0;
	m_Offset = 0;
	m_VtxCnt = 0;
}

Model::~Model()
{
}

const TCHAR* Model::SetName(const TCHAR* name)
{
	m_Name = name;
	return m_Name;
}

int Model::Create(ID3D11Device* pDev, VOID* pBuff, UINT size)
{
	int res = S_OK;
	m_pDev = pDev;
	m_pDev->GetImmediateContext(m_pDXDC.GetAddressOf());

	CreateVertexBuffer(pBuff, size);

	return 0;
}

int Model::Update(float dTime)
{
	m_pEffect->Update();


	//돌리기
	


	return 0;
}

int Model::Draw(float dTime, DRAWTYPE drawType)
{
	Set(dTime);
	m_pEffect->Apply();
	m_pDXDC->Draw(m_VtxCnt, 0);
	return 0;
}

int Model::Set(float dTime)
{
	m_pDXDC->IASetVertexBuffers(0, 1, m_pVB[0].GetAddressOf(), &m_Stride, &m_Offset);
	m_pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //일단 하드코딩
	return 0;
}

void Model::SetShader(std::shared_ptr<Effect> effect)
{
	m_pEffect = effect;
}


int Model::CreateVertexBuffer(VOID* pBuff, UINT size)
{
	ID3D11Buffer* pVB = nullptr;

	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;		//버퍼 사용 방식
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;		//버텍스 버퍼 용도로 설정
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = pBuff;			//버퍼에 들어갈 데이터 설정

	//버퍼 생성
	hr = m_pDev->CreateBuffer(&bd, &rd, &pVB);
	if (FAILED(hr))
	{
		ERROR_MSG(hr);
		return hr;
	}


	//없으면 넣기
	for (auto& vb : m_pVB)
	{
		if (vb == nullptr)
		{
			vb = pVB;
			break;
		}
	}

	m_buffSize = size;
	m_Stride = sizeof(VTX);
	m_Offset = 0;
	m_VtxCnt = size / sizeof(VTX);



	return S_OK;
}

int  ModelCreate(ID3D11Device* pDev, VOID* pBuff, UINT size, std::shared_ptr<Model>& outModel) 
{
	int hr = S_OK;

	//모델 객체 생성
	auto pModel = std::make_shared<Model>();
	if (pModel == nullptr)
	{
		//예외처리
	}

	//모델 정보 구성
	hr = pModel->Create(pDev, pBuff, size);
	if (FAILED(hr))
	{
		ERROR_MSG(hr);
	}

	outModel = pModel;

	return hr;
}
