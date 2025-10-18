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
	ComPtr<ID3D11Device>		m_pDev;
	ComPtr<ID3D11DeviceContext> m_pDXDC;

protected:
	const TCHAR* m_Name;		//쉐이더 객체 이름
	const TCHAR* m_FileName;	//파일 이름
	const TCHAR* m_Profile;		//쉐이더 컴파일 타겟 정보: "vs_5.0등

	ComPtr<ID3D11VertexShader>	m_pVS;
	ComPtr<ID3D11PixelShader>	m_pPS;
	ComPtr<ID3DBlob>			m_pVSCode;		//정점 쉐이더 바이너리 코드
	ComPtr<ID3D11Buffer>		m_pCB;

	BaseConstBuffer m_BCBuffer;

	//리소스 관련 데이터
	ComPtr<ID3D11ShaderResourceView>	m_pTextureRV;
	const TCHAR*						m_pTexName;
	ComPtr<ID3D11SamplerState>			m_pSampler;


	bool m_bIsDirty;
protected:
	int	Load(TCHAR* filename);			//이펙트 구성 함수들

	//이펙트/셰이더 컴파일.
	HRESULT Compile(WCHAR* FileName, char* EntryPoint, char* ShaderModel, ID3DBlob** ppCode);

	//상수 버퍼 운용 메소드.
	HRESULT CreateConstBuffer(UINT size, ID3D11Buffer** ppCB);
	HRESULT CreateDynaConstBuffer(UINT size, LPVOID pData, ID3D11Buffer** ppCB);
	HRESULT UpdateDynaConstBuffer(ID3D11DeviceContext* pDXDC, ID3D11Resource* pBuff, LPVOID pData, UINT size);

public:
	explicit Effect();
	virtual ~Effect();

	virtual int Create(ID3D11Device* pDev, TCHAR* filename);
	virtual int Create(ID3D11Device* pDev, TCHAR* shaderfilename, TCHAR* resourceFileName);
	virtual int Update(float dTime = 0);
	virtual int Apply(float dTime = 0);
	virtual void Release();



	virtual int UpdateCB();
	virtual int UpdateCB(const XMMATRIX& mTM);
	virtual int UpdateCB(const XMFLOAT4X4& mTM);

	virtual int SetView(const XMMATRIX& mTM);
	virtual int SetView(const XMFLOAT4X4& mTM);
	virtual int SetProj(const XMMATRIX& mTM);
	virtual int SetProj(const XMFLOAT4X4& mTM);

	virtual void Info(int x, int y, COLOR col, float dTime = 0);

	BaseConstBuffer* GetCBuffer() { return &m_BCBuffer; }
};

int EffectCreate(ID3D11Device* pDev, TCHAR* filename, std::shared_ptr<Effect>& outEffect);