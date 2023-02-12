#include <SkyMode.h>

#define MILLIS_BEFORE_SUNRISE_START 2000
#define MILLIS_BEFORE_SUN_SHOWS 10000
#define INITIAL_GLOW_DIAMETER 150

const CRGB SUNRISE_COLOR = CRGB(247, 170, 111);

void SkyMode::update(CRGB *leds)
{
    if (!pivot_mode)
    {
        sunrise_start_time += MILLIS_BEFORE_SUNRISE_START;

        if (sunrise_start_time > millis() && sunrise_start_time + MILLIS_BEFORE_SUN_SHOWS < millis())
        {
            for (int i = 0; i < INITIAL_GLOW_DIAMETER - 4; i++)
            {

                float brigthness = (float)(sunrise_start_time - millis()) / (float)MILLIS_BEFORE_SUN_SHOWS;

                leds[i] = CRGB(SUNRISE_COLOR.r * brigthness,
                               SUNRISE_COLOR.g * brigthness,
                               SUNRISE_COLOR.b * brigthness);
            }
        }
    }
    else
    {
        if (sunrise_point != 0)
            leds[sunrise_point - 1] = CRGB(64, 64, 0);

        leds[sunrise_point] = CRGB(255, 255, 0);
        leds[sunrise_point + 1] = CRGB(64, 64, 0);

        if (sunset_point != NUMPIXELS)
            leds[sunset_point + 1] = CRGB(64, 0, 0);

        leds[sunset_point] = CRGB(255, 0, 0);
        leds[sunset_point - 1] = CRGB(64, 0, 0);
    }
}

SkyMode::SkyMode()
{
}

SkyMode::SkyMode(const char *data)
{
    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> args;
    deserializeJson(args, data);

    pivot_mode = args["edit"].as<bool>();
    if (!pivot_mode)
    {
        sunrise_start_time = millis();
    }
    sunrise_point = args["start"].as<int>();
    sunset_point = args["end"].as<int>();
    length = args["length"].as<float>();

    args.garbageCollect();
}
SkyMode::~SkyMode() {}