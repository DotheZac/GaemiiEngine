#pragma once
#include "DX11.h"

enum class DRAWTYPE
{
	POINTLIST = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
	LINELIST = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
	LINESTRIP = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
	TRIANGLELIST = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	TRIANGLESTRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
};

class Model
{
public:
	//버텍스 정보
	struct VTX
	{
		float x, y, z;
	};

protected:
	ComPtr<ID3D11Device>		m_pDev;
	ComPtr<ID3D11DeviceContext> m_pDXDC;

protected:
	const TCHAR* m_Name;						//쉐이더 객체 이름

	ComPtr<ID3D11Buffer> m_pVB[2];				//일단 2개만
	UINT m_buffSize;							//버퍼 크기
	UINT m_Stride;								//정점 크기
	UINT m_Offset;								//렌더링 시작 정점, 대부분 0
	UINT m_VtxCnt;								//정점 개수, Draw할 때 쓰는거


public:
	virtual int CreateVB(void* pBuff, UINT vtxCnt);
	virtual int CreateLayout();

public:
	Model();
	virtual ~Model();

	virtual const TCHAR* SetName(const TCHAR* name);
	virtual const TCHAR* GetName() { return m_Name; }

	virtual int Create(ID3D11Device* pDev, VOID* pBuff, UINT size);
	virtual int Update(float dTime = 0);
	virtual int Draw(float dTime = 0, DRAWTYPE drawType = DRAWTYPE::TRIANGLELIST);
	virtual int Set(float dTime);

protected:
	int CreateVertexBuffer(VOID* pBuff, UINT size);
};

int  ModelCreate(ID3D11Device pDev, VOID* pBuff, UINT size, Model** ppModel);
