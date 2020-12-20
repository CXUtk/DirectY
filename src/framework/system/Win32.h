#pragma once
#include <Windows.h>
#include "../framework.h"
#include "DYWindow.h"
#include "../device/GDIDevice.h"
#include "UserInput.h"

class UserInput;
LRESULT CALLBACK  WndProc(HWND, UINT, WPARAM, LPARAM);

class Win32Window :public DYWindow {
    friend LRESULT CALLBACK  WndProc(HWND, UINT, WPARAM, LPARAM);
public:
    Win32Window();
    void Init() override;
    void PollEvents() override;
    int getWidth() const override { return WIDTH; }
    int getHeight() const override { return HEIGHT; }
    MouseInfo getMouseInfo() const override {
        auto info = _userInput->getMouseInfo();
        info.mousePos.y = HEIGHT - info.mousePos.y - 1;
        return info;
    }
    std::shared_ptr<UserInput> getUserInputController() const override {
        return _userInput;
    }
    std::shared_ptr<GraphicDevice> getGraphicDevice() const override { return _graphicDevice; }


private:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;
    static constexpr int MAX_LOADSTRING = 100;
    static Win32Window* _instance;

    HWND hWnd;
    // 全局变量:
    HINSTANCE hInst;                                // 当前实例
    WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
    WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

    HACCEL hAccelTable;

    std::shared_ptr<GraphicDevice> _graphicDevice;

    std::shared_ptr<UserInput> _userInput;

    ATOM register_window();
    BOOL init_instance();
};
