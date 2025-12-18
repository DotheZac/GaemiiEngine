//
//! Demo.fx : 기본 셰이더 소스.
//!
//! \author	김기홍 Kihong Kim / onlysonim@gmail.com 
//! \date   2003.11.04. Updated. DX9.x 
//! \date   2010.12.01. Updated. DX11, Jun.2010
//! \date   2016.12.27. Updated. DX11/12, Window SDK 8.1 / Window 10 SDK 10.0.18362
//! \date   2018.12.30. Updated. DX11.x/12.x, Windows 10 SDK 10.0.18362
//! \date   2020.08.22. Updated. DX11.x/12.x, Windows 10 SDK 10.0.19041 
//! \date   2024.12.10. Updated. DX11.x/12.x, Windows 10 SDK 10.0.22621 (VS22)
//! \date   2025.09.01. Updated. DX11.x/12.x, Windows 10 SDK 10.0.26100 (VS22)
//


// 상수 버퍼
cbuffer BaseConstBuffer : register(b0)
{
    matrix mWorld; //월드 변환 행렬. 
    matrix mView;       //뷰 변환 행렬. 
    matrix mProj;       //투영 변환 행렬. 
    matrix mVP;
};

cbuffer ConstBuffer : register(b1)
{
};

 
//VS 출력 구조체.
struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
};




////////////////////////////////////////////////////////////////////////////// 
//
// Vertex Shader Main : 정점 셰이더 메인 함수.
//
////////////////////////////////////////////////////////////////////////////// 

VSOutput VS_Main(
				  float4 pos : POSITION    //[입력] 정점좌표. Vertex Position (Model Space, 3D)
				)
{
	//정점 정리
    pos.w = 1;

	//월드 변환.(World Transform) 
    pos = mul(pos, mWorld); //pos = pos * mTM
	
    ////시야-뷰 변환 (View Transform)
    pos = mul(pos, mView);

    ////원근 투영 변환 (Projection Transform)
    pos = mul(pos, mProj);


    //정보 출력.
    VSOutput o = (VSOutput)0;
    o.pos = pos;
    o.col = float4(1, 0, 0, 1);
         
	return o;
}






////////////////////////////////////////////////////////////////////////////// 
//
// Pixel Shader Main : 픽셀 셰이더 메인 함수.
//
////////////////////////////////////////////////////////////////////////////// 

float4 PS_Main(
				float4 pos : SV_POSITION,   //[입력] (보간된) 픽셀별 좌표. (Screen, 2D)
				float4 col : COLOR0         //[입력] (보간된) 픽셀별 색상. (Pixel Color : "Diffuse")
				) : SV_TARGET               //[출력] 색상.(필수), "렌더타겟" 으로 출력합니다.
{
	//특별한 변환 없이 입력된 정보를 그대로 출력합니다.
    //...

    return col;
}



/**************** end of file "Demo.fx" ***********************/
