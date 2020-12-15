#pragma once
#include <Windows.h>
#include "../framework.h"
#include "DYWindow.h"

class Win32Window :public DYWindow {
public:
    void Init() override;
    void PollEvents() override;
    int getWidth() const override { return WIDTH; }
    int getHeight() const override { return HEIGHT; }

private:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;
    static constexpr int MAX_LOADSTRING = 100;

    HWND hWnd;
    // 全局变量:
    HINSTANCE hInst;                                // 当前实例
    WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
    WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

    HACCEL hAccelTable;

    ATOM register_window();
    BOOL init_instance();
};
