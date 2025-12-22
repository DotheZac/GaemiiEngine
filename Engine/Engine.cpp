#include "DX11.h"
#include "Shader.h"
#include "Effect.h"
#include "Model.h"
#include "Object.h"
#include "Engine.h"
#include <vector>

struct VERTEX
{
	float x, y, z;
};

ID3D11Buffer* g_pVB = nullptr;
ID3D11InputLayout* g_pVBLayout = nullptr;
ID3D11RasterizerState* g_RState = nullptr;

XMFLOAT3 g_vPos(0, 0, 5);		//위치 : "월드 공간"
XMFLOAT3 g_vRot(0, 0, 0);		//회전 
XMFLOAT3 g_vScale(1, 1, 1);		//스케일 

std::shared_ptr<Effect> g_pObjFx;

void RasterStateCreate();
void CameraUpdate(float dTime);




std::shared_ptr<Model> g_pTestModel;
std::shared_ptr<Object> g_pTestObject;


std::shared_ptr<Object> g_pTestObject2;



void Init()
{

	//EffectCreate(g_pDevice.Get(), L"../fx/Demo.fx", g_pFx);
	EffectCreate(g_pDevice.Get(), L"../fx/Demo.fx", g_pObjFx);

	ModelCreate(g_pDevice.Get(), g_TestModel, g_TestModelSize, g_pTestModel);
	
	ObjectCreate(g_pDevice.Get(), g_pTestModel, g_pObjFx, XMFLOAT3(0, 0, 0), g_pTestObject);

	ObjectCreate(g_pDevice.Get(), g_pTestModel, g_pObjFx, XMFLOAT3(0, 0, 0), g_pTestObject2);
	g_pTestObject2->SetUpdateFunc([](Object& o, float dt)
		{
			
		});


	RasterStateCreate();
}

void RasterStateCreate()
{
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = false;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = false;
	rd.AntialiasedLineEnable = false;
	//레스터라이져 객체 생성.
	g_pDevice->CreateRasterizerState(&rd, &g_RState);
}

void RasterStateRelease()
{
	g_RState->Release();
}

float EngineUpdate()
{
	float dTime = GetEngineTime();
	SystemUpdate(dTime);
	g_pDXDC->RSSetState(g_RState);

	CameraUpdate(dTime);

	return dTime;
}


void CameraUpdate(float dTime)
{
	XMFLOAT3 eyePos = XMFLOAT3(0.0f, 2.0f, -10.0f);
	//XMFLOAT3 lookAt = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 upDir = XMFLOAT3(0.0f, 1.0f, 0.0f);

	XMVECTOR eye = XMLoadFloat3(&eyePos);
	XMVECTOR lookat = XMLoadFloat3(&g_vPos);
	XMVECTOR up = XMLoadFloat3(&upDir);
	XMMATRIX mView = XMMatrixLookAtLH(eye, lookat, up);


	// 카메라 "렌즈" 및 "촬영 영역" 정보.  
	FLOAT fov = XMConvertToRadians(45);
	FLOAT aspect = g_DisPlay.Width / (float)g_DisPlay.Height;
	FLOAT zn = 1.0f;
	FLOAT zf = 100.0f;
	// 원근 투영 변환 행렬 생성 : Projection Transform.
	XMMATRIX mProj = XMMatrixPerspectiveFovLH(fov, aspect, zn, zf);


	//상수 버퍼에 행렬 설정.
	//g_pFx->SetView(mView);
	//g_pFx->SetProj(mProj);
	g_pObjFx->SetView(mView);
	g_pObjFx->SetProj(mProj);
}



void Render()
{

	float dTime = EngineUpdate();

	g_pTestObject->Update(dTime);


	COLOR col(0, 0.125f, 0.3f, 1.0f);
	ClearBackBuffer(D3D11_CLEAR_DEPTH, col);



	g_pTestObject->Draw();

	//ObjDraw();
	Flip();
}
