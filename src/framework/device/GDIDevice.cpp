#include "GDIDevice.h"

void GDIDevice::Present(const FrameBuffer* frameBuffer) {
    int width = frameBuffer->GetWidth();
    int height = frameBuffer->GetHeight();

    HDC hdc = GetDC(_hwnd);
    BITMAPINFO info;
    ZeroMemory(&info, sizeof(BITMAPINFO));
    info.bmiHeader.biBitCount = 24;
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biSizeImage = 0;
    info.bmiHeader.biCompression = BI_RGB;
    StretchDIBits(hdc, 0, 0, width, height, 0, 0, width, height, frameBuffer->GetBuffer(), &info, DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(_hwnd, hdc);
}
