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

StaticMode::StaticMode(StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> &args)
{
    color = CRGB(args["r"].as<int>(),
                 args["g"].as<int>(),
                 args["b"].as<int>());

    serializeJson(args, Serial);
}
StaticMode::~StaticMode()
{
    free(&color);
}