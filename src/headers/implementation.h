#ifndef IMPLEMENTATION_H
#define IMPLEMENTATION_H

#include "honeybear/engine.h"

struct Implementation : public Honeybear::Engine
{
    Implementation();
    void Draw();
    void Update(const float dt);
    void BeginFrame();
    void UpdateFixed(const double dt);
    void InterpolateState(const double t);
    void HandleInput();
}; 

#endif