#pragma once

void ShaderCreate(const TCHAR* name);
//int ShaderLoad(const TCHAR* name);

struct ConstBuffer
{
	XMMATRIX mTM;		//"World" 변환 행렬 : DirectXMath, 16바이트 정렬 버전. 
};

//셰이더 상수 버퍼.
extern ID3D11Buffer* g_pCB;


extern ID3D11VertexShader* g_pVS;
extern ID3D11PixelShader* g_pPS;
extern ID3DBlob* g_pVSCode;
