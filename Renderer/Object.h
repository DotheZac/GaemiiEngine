#pragma once
class Object
{
private:
	//전체 유닛 객체 인스턴스 관리용 데이터

	UINT m_ID;

protected:
	XMFLOAT3 m_vPos;
	XMFLOAT3 m_vRot;
	XMFLOAT3 m_vScale;
	XMFLOAT4X4	m_mTM;
	XMFLOAT4X4	m_mScale, m_mRot, m_mTrans;

	//리셋용 데이터
	XMFLOAT3 m_vPosBk;
	XMFLOAT3 m_vRotBk;
	XMFLOAT3 m_vScaleBk;

	std::shared_ptr<Model> m_pModel;
	std::shared_ptr<Effect> m_pFx;

protected:
	virtual void UpdateTransform(const XMMATRIX& mScale, const XMMATRIX& mRot, const XMMATRIX& mTrans, const XMMATRIX& mTM);

public:

	explicit Object();
	virtual ~Object();

	virtual int  Create(ID3D11Device* pDev, std::shared_ptr<Model>& pModel, std::shared_ptr<Effect>& pFx, XMFLOAT3 pos = XMFLOAT3(0, 0, 0));
	virtual int  Update(float dTime = 0);
	virtual int  Draw(float dTime = 0);

	virtual void Reset();
	virtual void Backup();

private:
	void GenerateID();
	
};

int ObjectCreate(ID3D11Device* pDev, std::shared_ptr<Model>& pModel, std::shared_ptr<Effect>& pFx, XMFLOAT3 pos, std::shared_ptr<Object>& outObject);