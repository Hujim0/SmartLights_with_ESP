#include <StaticMode.h>
using std::strtol;

void StaticMode::update(CRGB *leds)
{
    for (int i = 0; i < NUMPIXELS; i++)
    {
        leds[i] = color;
    }
}

StaticMode::StaticMode()
{
    color = CRGB::White;
}

StaticMode::StaticMode(JsonArray &args)
{
    color = CRGB(args[0].as<int>(),
                 args[1].as<int>(),
                 args[2].as<int>());
}
StaticMode::~StaticMode()
{
    free(&color);
}