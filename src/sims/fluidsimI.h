#pragma once
#include <raylib.h>

struct FluidSimI {
public:
    virtual ~FluidSimI();
    virtual void init() = 0;
    virtual void update(float dt, const Vector2 &mousePos) = 0;
    virtual void draw() = 0;
};
