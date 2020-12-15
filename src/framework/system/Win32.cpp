#include "Win32.h"
#include "../Resource.h"

#include <windowsx.h>
#include <iostream>

Win32Window* Win32Window::_instance = nullptr;

Win32Window::Win32Window() {
    _instance = this;
}

void Win32Window::Init() {
    hInst = GetModuleHandle(NULL);
    // 初始化全局字符串
    LoadStringW(hInst, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInst, IDC_DIRECTY, szWindowClass, MAX_LOADSTRING);
    register_window();

    // 执行应用程序初始化:
    if (!init_instance()) {
        std::cerr << "Failed to initialize window" << std::endl;
        throw;
    }
    hAccelTable = LoadAccelerators(hInst, MAKEINTRESOURCE(IDC_DIRECTY));
    _graphicDevice = std::make_shared<GDIDevice>(hWnd);
}

void Win32Window::PollEvents() {
    MSG msg;
    GetMessage(&msg, nullptr, 0, 0);
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

ATOM Win32Window::register_window() {
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInst;
    wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DIRECTY));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL Win32Window::init_instance() {
    int nCmdShow = SW_NORMAL;
    RECT rect = { 0, 0, WIDTH, HEIGHT };
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, 0, 0);

    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        200, 200, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInst, nullptr);

    if (!hWnd) {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
        switch (wmId) {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        //PAINTSTRUCT ps;
        //HDC hdc = BeginPaint(hWnd, &ps);
        //FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        //EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
    {
        /* switch (wParam) {
         case VK_LEFT: {
             rasterizer->RotateCamera(-0.1f, glm::vec3(0, 1, 0));
             break;
         }
         case VK_RIGHT: {
             rasterizer->RotateCamera(0.1f, glm::vec3(0, 1, 0));
             break;
         }
         case VK_UP: {
             rasterizer->RotateCamera(-0.1f, glm::vec3(1, 0, 0));
             break;
         }
         case VK_DOWN: {
             rasterizer->RotateCamera(0.1f, glm::vec3(1, 0, 0));
             break;
         }
         }*/

        break;
    }
    case WM_LBUTTONDOWN: {
        Win32Window::_instance->_userInput.MouseDown();
        break;
    }
    case WM_LBUTTONUP: {
        Win32Window::_instance->_userInput.MouseUp();
        break;
    }
    case WM_MOUSEMOVE: {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        Win32Window::_instance->_userInput.MouseMove(glm::ivec2(xPos, yPos));
        break;
    }
    case WM_ERASEBKGND:
    {
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
