#define _CRTDBG_MAP_ALLOC

#include <windows.h>
#include "tchar.h"
#include "DX11.h"
#include "Shader.h"
#include "Engine.h"

#pragma region 윈도우 관련 전역변수
HWND g_hWnd = nullptr;

struct WindowSize
{
    int width;
    int height;
};
WindowSize g_WindowDefaultSize{ g_DisPlay.Width, g_DisPlay.Height };

const TCHAR* g_ClassName = _T("Gaemii");
const TCHAR* g_WindowName = _T("GaemiiEngine");
#pragma endregion



#pragma region MyRegion
BOOL MessagePump();
BOOL InitWindow(int width, int height);
void ResizeWindow(HWND hWnd, UINT width, UINT height);
void MoveToCenter(HWND hwnd, int width, int height);

#pragma endregion


// 윈도우 프로시저: 창에서 발생하는 메시지를 처리
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0); // 창이 닫히면 메시지 루프 종료
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    if (!InitWindow(g_WindowDefaultSize.width, g_WindowDefaultSize.height))
    {
        return 0;
    }

    if (!DXSetup(g_hWnd))
        return 0;

    //ShaderCreate(L"../fx/Demo.fx");

    Init();
    

    while (true)
    {
        if (!MessagePump())
        {
            break;
        }

        Render();
        //Draw();
    }

    return 0;
}

LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //유저 입력 받는거 해야됨
    //if (UserMsgProc(hwnd, msg, wParam, lParam) == 0)
    //{
    //    return 0;
    //}

    switch (msg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            SendMessage(hwnd, WM_DESTROY, 0, 0);
            break;
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);

}


BOOL MessagePump()
{
    MSG msg;	::ZeroMemory(&msg, sizeof(msg));

    while (1)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                return FALSE;

            //메세지 처리...
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else
        {
            //특별한 메세지가 없다면 리턴후 게임(렌더링)을 진행합니다..
            return TRUE;
        }
    }

    return FALSE;
}

BOOL InitWindow(int width, int height)
{
    const WNDCLASSEX wc = {
        sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0, 0,
            ::GetModuleHandle(NULL), NULL, NULL,
            (HBRUSH)GetStockObject(DKGRAY_BRUSH), NULL,
            g_ClassName, NULL
    };

    RegisterClassEx(&wc);

    // 2. 윈도우 생성
    HWND hWnd = ::CreateWindow(g_ClassName, g_WindowName,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        //WS_OVERLAPPEDWINDOW, 
        0, 0,
        width, height,
        GetDesktopWindow(), NULL,
        wc.hInstance, NULL
    );

    if (hWnd == NULL) return FALSE;

    //윈도 크기 조정 (필수) 
    ResizeWindow(hWnd, width, height);

    ::ShowWindow(hWnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hWnd);
    ::SetCursor(LoadCursor(NULL, IDC_ARROW));

    //윈도우 핸들 저장.
    g_hWnd = hWnd;

    return TRUE;

}

void ResizeWindow(HWND hWnd, UINT width, UINT height)
{
    //현재 윈도 정보 획득. 
    RECT oldrc;	::GetWindowRect(hWnd, &oldrc);
    DWORD sytle = (DWORD) ::GetWindowLong(hWnd, GWL_STYLE);
    DWORD exstyle = (DWORD) ::GetWindowLong(hWnd, GWL_EXSTYLE);

    //새로 생성될 윈도우의 '클라이언트' 영역.
    RECT newrc = { 0, 0, (LONG)width, (LONG)height };

    //newrc 만큼의 클라이언트 영역을 포함하는 윈도 '전체' 크기를 구합니다.
    //현재 메뉴는 없다는 가정하에 처리되고 있습니다.
    //계산된 결과는 newrc 에 다시 들어옵니다. (스크린 좌표)
    //exstyle &= ~WS_EX_TOPMOST;
    ::AdjustWindowRectEx(&newrc, sytle, NULL, exstyle);

    //보정된 윈도우의 너비와 폭을 구합니다. 
    int newWidth = (newrc.right - newrc.left);
    int newHeight = (newrc.bottom - newrc.top);

    //새로운 크기를 윈도우에 설정합니다.
    ::SetWindowPos(hWnd, HWND_NOTOPMOST, oldrc.left, oldrc.top,
        newWidth, newHeight, SWP_SHOWWINDOW);

    //화면 중앙으로 이동.
    MoveToCenter(hWnd, newWidth, newHeight);

}

void MoveToCenter(HWND hwnd, int width, int height)
{
    RECT rc = {};
    ::GetClientRect(GetDesktopWindow(), &rc);
    rc.left = rc.right / 2 - width / 2;
    rc.top = rc.bottom / 2 - height / 2;

    ::MoveWindow(hwnd, rc.left, rc.top, width, height, TRUE);

}
