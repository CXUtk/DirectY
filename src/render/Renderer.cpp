#include "Renderer.h"

Renderer::Renderer(int width, int height) {
    _frameBuffer = new FrameBuffer(width, height);
}

void Renderer::ClearFrameBuffer() {
    _frameBuffer->Clear();
}


void Renderer::Present(HWND hwnd) {
    int width = _frameBuffer->GetWidth();
    int height = _frameBuffer->GetHeight();

    HDC hdc = GetDC(hwnd);
    BITMAPINFO info;
    ZeroMemory(&info, sizeof(BITMAPINFO));
    info.bmiHeader.biBitCount = 24;
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biSizeImage = width * height;
    info.bmiHeader.biCompression = BI_RGB;
    StretchDIBits(hdc, 0, 0, width, height, 0, 0, width, height, _frameBuffer->GetBuffer(), &info, DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(hwnd, hdc);
}
