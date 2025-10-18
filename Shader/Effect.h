#pragma once
class Effect
{
public:
	struct BaseConstBuffer
	{
		XMFLOAT4X4 mView;
		XMFLOAT4X4 mProj;
		XMFLOAT4X4 mVP;
		//XMFLOAT4X4 mWVP;		추후에 추가. 버텍스가 많아지면

	};

protected:
	//ID3D11Device* 
};

