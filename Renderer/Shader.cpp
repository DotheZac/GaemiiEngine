#include <Windows.h>
#include "DX11.h"
#include "d3dcompiler.h"				//DX 셰이더 컴파일러 헤더.
#pragma comment(lib, "d3dcompiler")		//DX 셰이더 컴파일러 라이브러리.  D3DCompiler.dll 필요.

#include "Shader.h"

ID3D11VertexShader* g_pVS = nullptr;
ID3D11PixelShader* g_pPS = nullptr;
ID3DBlob* g_pVSCode = nullptr;


HRESULT ShaderLoad(const TCHAR* fxname, const char* entry, const char* sm, ID3D11VertexShader** ppVS, ID3DBlob** ppCode);
HRESULT ShaderLoad(const TCHAR* fxname, const char* entry, const char* sm, ID3D11PixelShader** ppPS);
HRESULT ShaderCompile(const TCHAR* FileName, const char* EntryPoint, const char* ShaderModel, ID3DBlob** ppCode);


int ShaderLoad(const TCHAR* name)
{
	const TCHAR* filename = name;

	ShaderLoad(filename, "VS_Main", "vs_5_0", &g_pVS, &g_pVSCode);

	ShaderLoad(filename, "PS_Main", "ps_5_0", &g_pPS);

	return 1;
}

HRESULT ShaderLoad(const TCHAR* fxname, const char* entry, const char* sm, ID3D11VertexShader** ppVS, ID3DBlob** ppCode)
{
	HRESULT hr = S_OK;

	ID3DBlob* pCode = nullptr;
	hr = ShaderCompile(fxname, entry, sm, &pCode);
	if (FAILED(hr))
	{
		ERROR_MSG(hr);
		return hr;
	}

	ID3D11VertexShader* pVS = nullptr;
	hr = g_pDevice->CreateVertexShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, &pVS);
	if (FAILED(hr))
	{
		ERROR_MSG(hr);
		return hr;
	}

	*ppVS = pVS;
	*ppCode = pCode;

	return hr;
}


HRESULT ShaderLoad(const TCHAR* fxname, const char* entry, const char* sm, ID3D11PixelShader** ppPS)
{
	HRESULT hr = S_OK;

	ID3DBlob* pCode = nullptr;
	hr = ShaderCompile(fxname, entry, sm, &pCode);
	if (FAILED(hr))
	{
		ERROR_MSG(hr);
		return hr;
	}

	ID3D11PixelShader* pPS = nullptr;
	hr = g_pDevice->CreatePixelShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, &pPS);
	if (FAILED(hr))
	{
		ERROR_MSG(hr);
		return hr;
	}

	if (FAILED(hr))	
	{
		ERROR_MSG(hr);
		return hr;
	}

	*ppPS = pPS;

	return hr;
}


HRESULT ShaderCompile(const TCHAR* FileName, const char* EntryPoint, const char* ShaderModel, ID3DBlob** ppCode)
{
	HRESULT hr = S_OK;
	ID3DBlob* pError = nullptr;

	UINT Flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;			

	hr = D3DCompileFromFile(FileName,
		nullptr, nullptr,
		EntryPoint,
		ShaderModel,
		Flags,				
		0,					
		ppCode,				
		&pError				
	);
	if (FAILED(hr))
	{
		ERROR_MSG(hr);
		return hr;
	}

	return hr;
}

void ShaderCreate(const TCHAR* name)
{
	ShaderLoad(name);
}
