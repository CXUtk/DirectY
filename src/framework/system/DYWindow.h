#pragma once
#include <memory>
#include "../device/GraphicDevice.h"
#include "SystemUtils.h"

#include "UserInput.h"
class DYWindow {
public:
    DYWindow() = default;
    virtual ~DYWindow() = 0 {}
    virtual void Init() = 0;
    virtual void PollEvents() = 0;

    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual std::shared_ptr<GraphicDevice> getGraphicDevice() const = 0;
    virtual MouseInfo getMouseInfo() const = 0;
    virtual std::shared_ptr<UserInput> getUserInputController() const = 0;
};
