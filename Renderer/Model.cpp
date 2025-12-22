#include "DX11.h"
#include "Effect.h"
#include "Model.h"

Model::VTX g_TestModel[] =
{
		{ -0.5f,  0.0f, 0.0f },
		{  0.0f,  1.0f, 0.0f },
		{  0.5f,  0.0f, 0.0f }

};

vector<Model::VTX> g_TestModelIndex =
{
	
	{ -0.5f,  0.0f,  0.5f },			//좌상단
	{  0.5f,  0.0f,  0.5f },			//우상단
	{ -0.5f,  0.0f, -0.5f },			//좌하단
	{ 0.5f,  0.0f, -0.5f }				//우하단
	
};

vector<WORD> g_pFlatindices = 
{
	0, 1, 2,
	1, 2, 3 
};


UINT g_TestModelSize = sizeof(g_TestModel);

Model::Model()
{
	m_Name = L"N/A";
	m_buffSize = 0;
	m_Stride = 0;
	m_Offset = 0;
	m_VtxCnt = 0;

	m_IB.FaceCnt = 0;
	m_IB.IndexCnt = 0;
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

//테스트 필요
int Model::Create(ID3D11Device* pDev, vector<vector<Model::VTX>>& vVTX, vector<vector<WORD>>& vIndex)
{
	m_vVTX = vVTX;
	m_vIndex = vIndex;

	m_pDev = pDev;
	m_pDev->GetImmediateContext(&m_pDXDC);

	m_PartsNum = m_vVTX.size();

	for (auto vtx : m_vVTX)
	{
		ComPtr<ID3D11Buffer> vb;
		UINT size = vtx.size() * sizeof(vtx[0]);
		Model::CreateVertexBuffer(vtx.data(), size, vb);
		m_vpVB.push_back(vb);
	}

	for (auto index : m_vIndex)
	{
		MyIndexBuffer mib;
		_CreateIB(index, mib);
		m_vIB.push_back(mib);
	}

	return 0;
}

int Model::Create(ID3D11Device* pDev, vector<Model::VTX>& vVTX, vector<WORD>& vIndex)
{
	m_vVTX.push_back(vVTX);
	m_vIndex.push_back(vIndex);

	m_pDev = pDev;
	m_pDev->GetImmediateContext(&m_pDXDC);

	m_PartsNum = m_vVTX.size();

	for (auto vtx : m_vVTX)
	{
		ComPtr<ID3D11Buffer> vb;
		UINT size = vtx.size() * sizeof(vtx[0]);
		Model::CreateVertexBuffer(vtx.data(), size, vb);
		m_vpVB.push_back(vb);
	}

	for (auto index : m_vIndex)
	{
		MyIndexBuffer mib;
		_CreateIB(index, mib);
		m_vIB.push_back(mib);
	}

	m_IB = m_vIB[0];

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
	if (m_IB.pIB != nullptr)
	{
		for (int i = 0; i < m_PartsNum; i++)
		{
			Set(i, dTime);
			m_pDXDC->DrawIndexed(m_IB.IndexCnt, 0, 0);
		}
	}
	else
	{
		m_pDXDC->Draw(m_VtxCnt, 0);
	}

	return 0;
}

int Model::Set(float dTime)
{
	m_pDXDC->IASetVertexBuffers(0, 1, m_pVB.GetAddressOf(), &m_Stride, &m_Offset);
	m_pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //일단 하드코딩
	return 0;
}

int Model::Set(int index, float dTime)
{
	m_pDXDC->IASetVertexBuffers(0, 1, m_vpVB[index].GetAddressOf(), &m_Stride, &m_Offset);
	m_pDXDC->IASetIndexBuffer(m_vIB[index].pIB.Get(), DXGI_FORMAT_R16_UINT, 0);
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

	m_pVB = pVB;

	m_buffSize = size;
	m_Stride = sizeof(VTX);
	m_Offset = 0;
	m_VtxCnt = size / sizeof(VTX);



	return S_OK;
}

int Model::CreateVertexBuffer(VOID* pBuff, UINT size, ComPtr<ID3D11Buffer>& outVB)
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

	outVB = pVB;

	m_buffSize = size;
	m_Stride = sizeof(VTX);
	m_Offset = 0;
	m_VtxCnt = size / sizeof(VTX);

	return hr;
}

int Model::_CreateIB(std::vector<WORD>& ibdata, MyIndexBuffer& mib)
{
	if (ibdata.empty())
	{
		ERROR_MSG("인덱스 데이터 비어있음");
		return S_FALSE;
	}

	WORD maxIndex = 0;
	for (WORD idx : ibdata)
		maxIndex = max(maxIndex, idx);

	if (maxIndex >= 65536)
	{
		ERROR_MSG(L"인덱스 초과");
		return S_FALSE;
	}

	UINT indexCount = (UINT)ibdata.size();

	UINT ibSize = sizeof(WORD) * indexCount;

	LPVOID ib = ibdata.data();

	mib.IndexCnt = indexCount;
	mib.FaceCnt = mib.IndexCnt / 3;



	HRESULT hr = CreateIndexBuffer(m_pDev.Get(), ib, ibSize, &mib.pIB);
	if (FAILED(hr))
	{
		ERROR_MSG(hr);
		return hr;
	}

	return hr;
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

int ModelCreateIndex(ID3D11Device* pDev, std::vector<std::vector<Model::VTX>>& vVTX, std::vector<std::vector<WORD>>& vIndex, std::shared_ptr<Model>& outModel)
{
	int hr = S_OK;

	//모델 객체 생성
	auto pModel = std::make_shared<Model>();
	if (pModel == nullptr)
	{
		//예외처리
	}

	//모델 정보 구성
	hr = pModel->Create(pDev, vVTX, vIndex);
	if (FAILED(hr))
	{
		ERROR_MSG(hr);
	}

	outModel = pModel;

	return hr;
}

int ModelCreateIndex(ID3D11Device* pDev, std::vector<Model::VTX>& vVTX, std::vector<WORD>& vIndex, std::shared_ptr<Model>& outModel)
{
	int hr = S_OK;

	//모델 객체 생성
	auto pModel = std::make_shared<Model>();
	if (pModel == nullptr)
	{
		//예외처리
	}

	//모델 정보 구성
	hr = pModel->Create(pDev, vVTX, vIndex);
	if (FAILED(hr))
	{
		ERROR_MSG(hr);
	}

	outModel = pModel;

	return hr;
}
