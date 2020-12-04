#ifndef IMPLEMENTATION_H
#define IMPLEMENTATION_H

#include "honeybear/engine.h"

struct Implementation : public Honeybear::Engine
{
    Implementation();
    void Draw();
    void Update(const float dt);
}; 

#endif