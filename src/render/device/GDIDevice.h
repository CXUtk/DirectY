#pragma once
#include "GraphicDevice.h"
#include <Windows.h>
class GDIDevice : public GraphicDevice {
public:
    GDIDevice(HWND hwnd) :_hwnd(hwnd) {}
    void Present(const FrameBuffer* frameBuffer) override;
private:
    HWND _hwnd;
};
