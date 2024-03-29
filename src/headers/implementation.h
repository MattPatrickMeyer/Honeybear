#ifndef IMPLEMENTATION_H
#define IMPLEMENTATION_H

namespace Implementation
{
    void Init();
    void Draw();
    void BeginFrame();
    void UpdateFixed(const double dt);
    void InterpolateState(const double t);
    void UpdateBuffers(const float window_width, const float window_height);
}; 

#endif