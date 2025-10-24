#include "DX11.h"
#include "Model.h"
// TODO: 라이브러리 함수의 예제입니다.

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



	return 0;
}

int Model::Update(float dTime)
{
	return 0;
}

int Model::Draw(float dTime, DRAWTYPE drawType)
{
	return 0;
}

int ModelCreate(ID3D11Device pDev, VOID* pBuff, UINT size, Model* ppModel)
{
	return 0;
}
