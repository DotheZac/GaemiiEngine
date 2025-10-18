#include "DX11.h"
#include "Effect.h"

int Effect::Load(TCHAR* filename)
{
	return 0;
}

HRESULT Effect::Compile(WCHAR* FileName, char* EntryPoint, char* ShaderModel, ID3DBlob** ppCode)
{
	return E_NOTIMPL;
}

HRESULT Effect::CreateConstBuffer(UINT size, ID3D11Buffer** ppCB)
{
	return E_NOTIMPL;
}

HRESULT Effect::CreateDynaConstBuffer(UINT size, LPVOID pData, ID3D11Buffer** ppCB)
{
	return E_NOTIMPL;
}

HRESULT Effect::UpdateDynaConstBuffer(ID3D11DeviceContext* pDXDC, ID3D11Resource* pBuff, LPVOID pData, UINT size)
{
	return E_NOTIMPL;
}

Effect::Effect()
{
}

Effect::~Effect()
{
}

int Effect::Create(ID3D11Device* pDev, TCHAR* filename)
{
	return 0;
}

int Effect::Create(ID3D11Device* pDev, TCHAR* shaderfilename, TCHAR* resourceFileName)
{
	return 0;
}

int Effect::Update(float dTime)
{
	return 0;
}

int Effect::Apply(float dTime)
{
	return 0;
}

void Effect::Release()
{
}

int Effect::UpdateCB()
{
	return 0;
}

int Effect::UpdateCB(const XMMATRIX& mTM)
{
	return 0;
}

int Effect::UpdateCB(const XMFLOAT4X4& mTM)
{
	return 0;
}

int Effect::SetView(const XMMATRIX& mTM)
{
	return 0;
}

int Effect::SetView(const XMFLOAT4X4& mTM)
{
	return 0;
}

int Effect::SetProj(const XMMATRIX& mTM)
{
	return 0;
}

int Effect::SetProj(const XMFLOAT4X4& mTM)
{
	return 0;
}

void Effect::Info(int x, int y, COLOR col, float dTime)
{
}

int EffectCreate(ID3D11Device* pDev, TCHAR* filename, std::shared_ptr<Effect>& outEffect)
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
