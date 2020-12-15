#pragma once
#include "../../render/buffers/FrameBuffer.h"
class GraphicDevice {
public:
    GraphicDevice() {}
    virtual void Present(const FrameBuffer* frameBuffer) = 0;
};
