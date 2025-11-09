#pragma once
#include <raylib.h>

struct FluidSimI {
public:
    virtual ~FluidSimI() = default;
    virtual void init() = 0;
    virtual void update(float dt, const Vector2 &mousePos);
    virtual void draw() = 0;
};
