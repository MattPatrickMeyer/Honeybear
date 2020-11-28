#include "engine.h"

int main()
{
    Honeybear::Engine engine;
    engine.Init(1920, 1080, "Honeybear!");
    engine.Run();
    return 0;
}