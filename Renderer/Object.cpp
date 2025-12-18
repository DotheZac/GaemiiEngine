#include "DX11.h"
#include "Effect.h"
#include "Model.h"
#include "Object.h"

UINT m_InstCnt = 0;

Object::Object()
{
	GenerateID();
	
	m_vPos = XMFLOAT3(0, 0, 0);
	m_vRot = XMFLOAT3(0, 0, 0);
	m_vScale = XMFLOAT3(1, 1, 1);

	XMStoreFloat4x4(&m_mTM, XMMatrixIdentity());
	XMStoreFloat4x4(&m_mRot, XMMatrixIdentity());
	XMStoreFloat4x4(&m_mScale, XMMatrixIdentity());
	XMStoreFloat4x4(&m_mTrans, XMMatrixIdentity());

	
}

Object::~Object()
{
}

int Object::Create(ID3D11Device* pDev, std::shared_ptr<Model>& pModel, std::shared_ptr<Effect>& pFx, XMFLOAT3 pos)
{
	int hr = S_OK;

	m_vPos = pos;

	m_pModel = pModel;
	m_pFx = pFx;
	m_pModel.get()->SetShader(m_pFx);

	return 0;
}

int Object::Update(float dTime)
{
	//테스트용
	m_vRot.y += dTime;


	XMMATRIX mScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
	XMMATRIX mRotX = XMMatrixRotationX(m_vRot.x); // 회전 넣기
	XMMATRIX mRotY = XMMatrixRotationY(m_vRot.y); // 회전 넣기
	XMMATRIX mRotZ = XMMatrixRotationZ(m_vRot.z); // 회전 넣기
	XMMATRIX mRot = mRotX * mRotY * mRotZ;
	XMMATRIX mTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

	XMMATRIX localTM = mScale * mRot * mTrans;

	//XMMATRIX worldTM = localTM;
	//if (m_pParent)
	//	worldTM *= XMLoadFloat4x4(&m_pParent->m_mTM);

	UpdateTransform(mScale, mRot, mTrans, localTM);


	return S_OK;
}

int Object::Draw(float dTime)
{
	m_pFx.get()->UpdateCB(m_mTM);

	m_pFx.get()->Apply();

	m_pModel->Draw();

	return 0;
}

void Object::Reset()
{
	m_vPos = m_vPosBk;
	m_vRot = m_vRotBk;
	m_vScale = m_vScaleBk;

}

void Object::Backup()
{
	m_vPosBk = m_vPos;
	m_vRotBk = m_vRot;
	m_vScaleBk = m_vScale;

}

void Object::GenerateID()
{
	m_ID = m_InstCnt;
	m_InstCnt++;
}

void Object::UpdateTransform(const XMMATRIX& mScale, const XMMATRIX& mRot, const XMMATRIX& mTrans, const XMMATRIX& mTM)
{
	XMStoreFloat4x4(&m_mScale, mScale);
	XMStoreFloat4x4(&m_mRot, mRot);
	XMStoreFloat4x4(&m_mTrans, mTrans);
	XMStoreFloat4x4(&m_mTM, mTM);

}


int ObjectCreate(ID3D11Device* pDev, std::shared_ptr<Model>& pModel, std::shared_ptr<Effect>& pFx, XMFLOAT3 pos, std::shared_ptr<Object>& outObject)
{
	auto pObject = std::make_shared<Object>();

	if (!pObject)
	{
		ERROR_MSG(E_FAIL);
		return -1;
	}

	pObject->Create(pDev, pModel, pFx, pos);

	outObject = pObject;

	return S_OK;
}
