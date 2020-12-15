#pragma once
class DYWindow {
public:
    DYWindow() = default;
    virtual ~DYWindow() = 0 {}
    virtual void Init() = 0;
    virtual void PollEvents() = 0;

    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
};
