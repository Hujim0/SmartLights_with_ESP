#include <SkyMode.h>

const unsigned long MILLIS_BEFORE_SUNRISE_START = 0;
#define SECONDS_BEFORE_SKY_SHOWS 40.0F
#define SECONDS_BEFORE_HUE_TILT 0.0F
#define INITIAL_GLOW_RADIUS 200
#define SUNRISE_GLOW_DROPOFF 30
#define SUNRISE_STEEPNESS 1.5F

#define SECONDS_BEFORE_SUN_STARTS_TO_SHOW 20.0F
#define SUN_RADIUS 3
#define SECONDS_BEFORE_SUN_FULLY_CAME_OUT 30.0F

const CRGB SUNRISE_SUN_COLOR = CRGB(255, 0, 0);
const CRGB SUN_COLOR = CRGB(255, 40, 0);

void SkyMode::update(CRGB *leds)
{
    if (pivot_mode)
    {
        ShowEditMode(leds);
        return;
    }

    if (millis() < sunrise_start_time)
        return;

    float SecondsSinceSunriseStart = (float)(millis() - sunrise_start_time) / 1000.0F;

    // initial sunrise ligtht
    if (SecondsSinceSunriseStart <= SECONDS_BEFORE_SKY_SHOWS)
    {
        ShowSunriseLight(SecondsSinceSunriseStart, leds);
    }

    if (SecondsSinceSunriseStart >= SECONDS_BEFORE_SUN_STARTS_TO_SHOW)
    {
        ShowSunriseSun(SecondsSinceSunriseStart - SECONDS_BEFORE_SUN_STARTS_TO_SHOW, leds);
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
        sunrise_start_time = millis() + MILLIS_BEFORE_SUNRISE_START;
        Serial.print("sunrise start: ");
        Serial.println(sunrise_start_time);
    }
    sunrise_point = args["start"].as<int>();
    sunset_point = args["end"].as<int>();
    length = args["length"].as<float>();

    args.garbageCollect();

    SUNRISE_COLOR = CHSV(0, 255, 60);
}
SkyMode::~SkyMode() {}

void SkyMode::ShowEditMode(CRGB *leds)
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

void SkyMode::ShowSunriseLight(float SecondsSinceSunriseStart, CRGB *leds)
{
    float phase = (SecondsSinceSunriseStart / SECONDS_BEFORE_SKY_SHOWS);

    if (SecondsSinceSunriseStart > SECONDS_BEFORE_HUE_TILT && SecondsSinceSunriseStart < 23.0F)
    {
        SUNRISE_COLOR = CHSV((SecondsSinceSunriseStart - SECONDS_BEFORE_HUE_TILT) * 0.8, 255, 150 * phase);
    }

    for (int i = sunrise_point; i <= sunrise_point + INITIAL_GLOW_RADIUS; i++)
    {
        float intensity = 1.0F;

        if (i - sunrise_point > INITIAL_GLOW_RADIUS - SUNRISE_GLOW_DROPOFF)
        {
            intensity *= (float)(INITIAL_GLOW_RADIUS - (i - sunrise_point)) / (float)SUNRISE_GLOW_DROPOFF;
        }

        if (intensity <= 0.0F)
            continue;

        intensity = pow(intensity, SUNRISE_STEEPNESS);

        if (intensity >= 1.0F)
            intensity = 1.0F;

        CRGB new_color = CRGB(SUNRISE_COLOR.r * intensity,
                              SUNRISE_COLOR.g * intensity,
                              SUNRISE_COLOR.b * intensity);

        leds[i] = new_color;

        if (sunrise_point * 2 - i < 0)
            continue;

        leds[sunrise_point * 2 - i] = new_color;
    }
}

void SkyMode::ShowSunriseSun(float SecondsSinceSunriseStart, CRGB *leds)
{
    float phase = ((SecondsSinceSunriseStart) / SECONDS_BEFORE_SUN_FULLY_CAME_OUT);

    if (phase >= 1.0F)
        phase = 1.0F;

    float SunOffset = SecondsSinceSunriseStart * 0.3F;

    // for (int i = sunrise_point + floor(SunOffset); i < sunrise_point + SUN_RADIUS + 3 + floor(SunOffset); i++)
    for (int i = 0; i < NUMPIXELS; i++)
    {
        if (i < sunrise_point + floor(SunOffset) - (SUN_RADIUS + 1) || i > sunrise_point + floor(SunOffset) + (SUN_RADIUS + 1))
            continue;

        float sunAlpha = cos((((float)(i - sunrise_point) / (float)SUN_RADIUS) * HALF_PI) - SunOffset * 0.525F); // to simulate that sun is a sphere

        if (sunAlpha < 0.0F)
            sunAlpha = 0.0F;

        float multiplier = phase - (1.0F - sunAlpha);

        if (multiplier <= 0.0F)
            multiplier = 0.0F;

        CRGB new_color = SUNRISE_COLOR + CRGB(SUNRISE_SUN_COLOR.r * multiplier,
                                              SUNRISE_SUN_COLOR.g * multiplier,
                                              SUNRISE_SUN_COLOR.b * multiplier);

        if (i < NUMPIXELS)
            leds[i] = new_color;

        // if ((sunrise_point)*2 - i > 0)
        //     leds[(sunrise_point)*2 - i] = new_color;

        // const float threshold = 0.66F;

        // if (phase < threshold)
        //     continue;

        // float _phase = (phase - threshold) / (1 - threshold) * multiplier;

        // if (_phase >= 1.0F)
        //     _phase = 1.0F;

        // CRGB additional_color = CRGB(SUN_COLOR.r * _phase,
        //                              SUN_COLOR.g * _phase,
        //                              SUN_COLOR.b * _phase);

        // leds[i] += additional_color;
        // leds[sunrise_point * 2 - i] += additional_color;
    }
}