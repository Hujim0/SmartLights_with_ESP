#include <WaveMode.h>

void WaveMode::update(CRGB *)
{
    // for (int i = 0; i < NUMPIXELS; i++)
    // {
    //     leds[i] =
    // }
}

WaveMode::WaveMode(JsonArray &doc)
{
    // speed = args[0].toFloat();
    // length = args[1].toFloat();
}

WaveMode::~WaveMode()
{
    free(&offset);
}