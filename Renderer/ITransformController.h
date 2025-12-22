#pragma once
class ITransformController
{
public:
	virtual void AddRotationY(float delta) = 0;
	virtual void AddPosition(const XMFLOAT3& delta) = 0;

protected:
	~ITransformController() = default;
};

