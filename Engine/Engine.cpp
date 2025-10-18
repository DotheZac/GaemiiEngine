#include "DX11.h"
#include "Shader.h"
#include "Engine.h"

struct VERTEX
{
	float x, y, z;
};

ID3D11Buffer* g_pVB = nullptr;
ID3D11InputLayout* g_pVBLayout = nullptr;
ID3D11RasterizerState* g_RState = nullptr;

ConstBuffer g_CBuffer;

XMFLOAT3 g_vPos(0, 0, 5);		//위치 : "월드 공간"
XMFLOAT3 g_vRot(0, 0, 0);		//회전 
XMFLOAT3 g_vScale(1, 1, 1);		//스케일 




int ObjLoad();
void RasterStateCreate();
void ObjUpdate(float dTime);
void ShaderUpdate(float dTime);
void CameraUpdate(float dTime);


void Init()
{
	ObjLoad();
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

	ShaderUpdate(dTime);
	CameraUpdate(dTime);
	ObjUpdate(dTime);
	return dTime;
}

void ObjUpdate(float dTime)
{
	XMMATRIX mScale = XMMatrixScaling(g_vScale.x, g_vScale.y, g_vScale.z);

	//이동 처리.★
	float mov = 5.0f * dTime;		// 5.0 / sec 이동량.
	if (IsKeyDown(VK_LEFT))  g_vPos.x -= mov;
	if (IsKeyDown(VK_RIGHT)) g_vPos.x += mov;
	if (IsKeyDown(VK_UP))	 g_vPos.z += mov;
	if (IsKeyDown(VK_DOWN))  g_vPos.z -= mov;
	XMMATRIX mTrans = XMMatrixTranslation(g_vPos.x, g_vPos.y, g_vPos.z);

	//회전 처리.★ 
	g_vRot.y += XM_PI * dTime;						//180º/sec 씩 회전.(라디안)
	//g_vRot.y += XMConvertToRadians(180) * dTime;	//180º/sec 씩 회전.(DirectXMath 사용)	
	XMMATRIX mRot = XMMatrixRotationY(g_vRot.y);


	XMMATRIX mTM = mScale * mRot * mTrans;		//[테스트4] 권장 조합.★

	XMFLOAT4X4 testf;
	XMStoreFloat4x4(&testf, mTM);

	XMFLOAT4X4 tt = testf;

	g_CBuffer.mTM = mTM;

	UpdateDynamicBuffer(g_pDXDC.Get(), g_pCB, &g_CBuffer, sizeof(ConstBuffer));

}

void ShaderUpdate(float dTime)
{
	//g_pDXDC->VSSetShader(g_pVS, nullptr, 0);
	//g_pDXDC->PSSetShader(g_pPS, nullptr, 0);
}

void CameraUpdate(float dTime)
{
	XMFLOAT3 eyePos = XMFLOAT3(0.0f, 2.0f, -10.0f);
	XMFLOAT3 lookAt = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 upDir = XMFLOAT3(0.0f, 1.0f, 0.0f);

	XMVECTOR eye = XMLoadFloat3(&eyePos);
	XMVECTOR lookat = XMLoadFloat3(&lookAt);
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
	g_CBuffer.mView = mView;
	g_CBuffer.mProj = mProj;

	UpdateDynamicBuffer(g_pDXDC.Get(), g_pCB, &g_CBuffer, sizeof(ConstBuffer));
}

int ObjLoad()
{
	HRESULT hr = S_OK;

	VERTEX	verts[] = {
		{ -0.5f,  0.0f, 0.0f },
		{  0.0f,  1.0f, 0.0f },
		{  0.5f,  0.0f, 0.0f }
	};

	//정점 버퍼 생성
	hr = CreateVertexBuffer(g_pDevice.Get(), verts, sizeof(verts), sizeof(VERTEX), &g_pVB);

	if (FAILED(hr))
	{
		ERROR_MSG(hr);
		return hr;
	}


	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                       offset         classification             
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// Inputlayout 객체 생성
	ID3D11InputLayout* pLayout = nullptr;
	hr = CreateInputLayout(g_pDevice.Get(), layout, ARRAYSIZE(layout), g_pVSCode, &g_pVBLayout);

	if (FAILED(hr)) 
	{
		ERROR_MSG(hr);
		return hr;
	}


	return hr;
}

void ObjDraw()
{
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	g_pDXDC->IASetVertexBuffers(0, 1, &g_pVB, &stride, &offset);

	//입력 레이아웃 설정.
	g_pDXDC->IASetInputLayout(g_pVBLayout);

	//기하 위상구조 설정 
	g_pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_pDXDC->VSSetShader(g_pVS, nullptr, 0);
	g_pDXDC->PSSetShader(g_pPS, nullptr, 0);
	g_pDXDC->VSSetConstantBuffers(0, 1, &g_pCB);


	g_pDXDC->Draw(3, 0);

}

void Render()
{

	EngineUpdate();

	COLOR col(0, 0.125f, 0.3f, 1.0f);
	ClearBackBuffer(D3D11_CLEAR_DEPTH, col);

	ObjDraw();
	Flip();
}
