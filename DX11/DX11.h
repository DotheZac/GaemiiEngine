#pragma once
//구형 C/C++ Runtime 호환성 경고 해제용 정의. (25.0118)
//#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS


#include "Windows.h"
#include "tchar.h"  
#include "stdio.h"

#include "memory"
#include "chrono"

using namespace std::chrono;
#define TIME_MAX  9223372036854775807   //

#include "wrl.h"
using namespace Microsoft::WRL;

#include "D3D11.h"					//DX 표준 헤더. (DX 11.0)
#pragma comment(lib, "D3D11")		//DX 라이브러리 로딩.  D3D11.dll 필요.
