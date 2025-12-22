#pragma once
#include "DX11.h"
#include <vector>

enum class DRAWTYPE
{
	POINTLIST = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
	LINELIST = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
	LINESTRIP = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
	TRIANGLELIST = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	TRIANGLESTRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
};

using std::vector;

class Model
{
public:
	//버텍스 정보
	struct VTX
	{
		float x, y, z;
	};

	struct INDEX
	{
		WORD A, B, C;
	};

	struct MyIndexBuffer
	{
		ComPtr<ID3D11Buffer> pIB;
		DWORD FaceCnt;
		DWORD IndexCnt;

	};

protected:
	ComPtr<ID3D11Device>		m_pDev;
	ComPtr<ID3D11DeviceContext> m_pDXDC;

protected:
	const TCHAR* m_Name;						//쉐이더 객체 이름

	ComPtr<ID3D11Buffer> m_pVB;					//일단 2개만
	UINT m_buffSize;							//버퍼 크기
	UINT m_Stride;								//정점 크기
	UINT m_Offset;								//렌더링 시작 정점, 대부분 0
	UINT m_VtxCnt;								//정점 개수, Draw할 때 쓰는거

	MyIndexBuffer m_IB;

	std::shared_ptr<Effect> m_pEffect;


	vector<vector<Model::VTX>> m_vVTX;
	vector<vector<WORD>> m_vIndex;
	vector<vector<Model::INDEX>> m_vIndexes;

	vector<ComPtr<ID3D11Buffer>> m_vpVB;
	vector<MyIndexBuffer> m_vIB;

	UINT m_PartsNum = 0;


public:
	//virtual int CreateVB(void* pBuff, UINT vtxCnt);
	//virtual int CreateLayout();

public:
	Model();
	virtual ~Model();

	virtual const TCHAR* SetName(const TCHAR* name);
	virtual const TCHAR* GetName() { return m_Name; }

	virtual int Create(ID3D11Device* pDev, VOID* pBuff, UINT size);
	virtual int Create(ID3D11Device* pDev, vector<vector<Model::VTX>>& vVTX, vector<vector<WORD>>& vIndex);
	virtual int Create(ID3D11Device* pDev, vector<Model::VTX>& vVTX, vector<WORD>& vIndex);
	virtual int Update(float dTime = 0);
	virtual int Draw(float dTime = 0, DRAWTYPE drawType = DRAWTYPE::TRIANGLELIST);
	virtual int Set(float dTime);
	virtual int Set(int index, float dTime);

	virtual void SetShader(std::shared_ptr<Effect> effect);
protected:
	int CreateVertexBuffer(VOID* pBuff, UINT size);
	int CreateVertexBuffer(VOID* pBuff, UINT size, ComPtr<ID3D11Buffer>& outVB);
	int _CreateIB(vector<WORD>& ibdata, MyIndexBuffer& mib);

};

//매개변수: 디바이스, 정점버퍼, 크기, 모델명
int  ModelCreate(ID3D11Device* pDev, VOID* pBuff, UINT size, std::shared_ptr<Model>& outModel);
//메쉬 여러개, 매개변수: 디바이스, 정점버퍼, 인덱스 버퍼, 모델명
int  ModelCreateIndex(ID3D11Device* pDev, vector<vector<Model::VTX>>& vVTX, vector<vector<WORD>>& vIndex, std::shared_ptr<Model>& outModel);
//메쉬 1개 매개변수: 디바이스, 정점버퍼, 인덱스 버퍼, 모델명
int  ModelCreateIndex(ID3D11Device* pDev, vector<Model::VTX>& vVTX, vector<WORD>& vIndex, std::shared_ptr<Model>& outModel);


//테스트용
extern Model::VTX g_TestModel[];

extern vector<Model::VTX> g_TestModelIndex;
extern vector<WORD>	g_pFlatindices;
extern UINT g_TestModelSize;
