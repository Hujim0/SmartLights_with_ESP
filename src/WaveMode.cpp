#include <WaveMode.h>

void WaveMode::update(CRGB *)
{
    // for (int i = 0; i < NUMPIXELS; i++)
    // {
    //     leds[i] =
    // }
}

WaveMode::WaveMode(StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> &args)
{
    // speed = args[0].toFloat();
    // length = args[1].toFloat();
}

WaveMode::~WaveMode()
{
    free(&offset);
}