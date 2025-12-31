#pragma once
#include <functional>
#include "ITransformController.h"

class Object : public ITransformController
{
public:
	using Behavior = std::function<void(float)>;

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

	//업데이트 함수를 저장하는 변수
	Behavior m_Behavior; 

public:		//protected였는데 람다로 Update한다고 바꿈
	virtual void UpdateTransform(const XMMATRIX& mScale, const XMMATRIX& mRot, const XMMATRIX& mTrans, const XMMATRIX& mTM);

public:

	explicit Object();
	virtual ~Object();

	virtual int Create(ID3D11Device* pDev, std::shared_ptr<Model>& pModel, std::shared_ptr<Effect>& pFx, XMFLOAT3 pos = XMFLOAT3(0, 0, 0));
	virtual int Update(float dTime = 0);
	virtual int Draw(float dTime = 0);
	void SetBehavior(Behavior behavior) { m_Behavior = std::move(behavior); }

	virtual void Reset();
	virtual void Backup();

//행동들
public:
	Behavior YTurn(float speed)
	{
		return [this, speed](float dTime)
			{
				AddRotationY(speed * dTime);
			};
	}

	Behavior SetPos(const XMFLOAT3& pos)
	{
		return [this, pos](float dTime)
			{
				AddPosition(pos);
			};
	}

private:
	void GenerateID();

//인터페이스 구현
private:
	void AddRotationY(float delta) override
	{
		m_vRot.y += delta;
	}

public:
	void AddPosition(const XMFLOAT3& pos) override
	{
		m_vPos.x += pos.x;
		m_vPos.y += pos.y;
		m_vPos.z += pos.z;
	}
	
};

int ObjectCreate(ID3D11Device* pDev, std::shared_ptr<Model>& pModel, std::shared_ptr<Effect>& pFx, XMFLOAT3 pos, std::shared_ptr<Object>& outObject);