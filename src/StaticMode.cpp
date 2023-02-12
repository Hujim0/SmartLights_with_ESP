#include <StaticMode.h>

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

StaticMode::StaticMode(const char *data)
{
    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> args;
    deserializeJson(args, data);

    color = CRGB(args["r"].as<int>(),
                 args["g"].as<int>(),
                 args["b"].as<int>());

    args.garbageCollect();
}
StaticMode::~StaticMode()
{
    free(&color);
}