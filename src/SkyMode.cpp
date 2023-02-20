#include <SkyMode.h>

const unsigned long MILLIS_BEFORE_SUNRISE_START = 0;
#define SECONDS_BEFORE_SKY_SHOWS 40.0F
#define SECONDS_BEFORE_HUE_TILT 0.0F
#define INITIAL_GLOW_RADIUS 200
#define SUNRISE_GLOW_DROPOFF 30
#define SUNRISE_STEEPNESS 1.5F

#define SECONDS_BEFORE_SUN_STARTS_TO_SHOW 20.0F
#define SUN_RADIUS 5.0F
#define SECONDS_BEFORE_SUN_FULLY_CAME_OUT 30.0F

const CRGB SUNRISE_SUN_COLOR = CRGB(255, 0, 0);

void SkyMode::update(CRGB *leds)
{
    if (edit_mode)
    {
        ShowEditMode(leds);
        return;
    }

    if (millis() < sunrise_start_time)
        return;

    float SecondsSinceSunriseStart = (float)(millis() - sunrise_start_time) / (10.0F * ((50 - speed) * 4));

    // initial sunrise ligtht
    if (SecondsSinceSunriseStart <= SECONDS_BEFORE_SKY_SHOWS)
    {
        ShowSunriseLight(SecondsSinceSunriseStart, leds);
    }

    if (SecondsSinceSunriseStart >= SECONDS_BEFORE_SUN_STARTS_TO_SHOW)
    {
        const float DELAY = 20.0F;

        if (SecondsSinceSunriseStart - SECONDS_BEFORE_SUN_STARTS_TO_SHOW > DELAY && SecondsSinceSunriseStart - SECONDS_BEFORE_SUN_STARTS_TO_SHOW < DELAY + 30.0F)
        {
            float phase = (SecondsSinceSunriseStart - SECONDS_BEFORE_SUN_STARTS_TO_SHOW - DELAY);

            SUN_COLOR = CHSV(phase * 1.3F, 255.0F - phase * 2.5F, 255);

            SKY_COLOR = CHSV(skyHue + phase * 3.0F, 255.0F - phase * 6.0F, skyValue - phase * 0.2F);

            for (int i = 0; i < NUMPIXELS; i++)
            {
                leds[i] = SKY_COLOR;
            }
        }

        ShowSunriseSun(SecondsSinceSunriseStart - SECONDS_BEFORE_SUN_STARTS_TO_SHOW, leds);
    }
}

SkyMode::SkyMode()
{
}
void SkyMode::update_args(const char *data)
{
    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> args;
    deserializeJson(args, data);

    edit_mode = args["edit"].as<bool>();
    if (edit_mode)
    {
        sunrise_start_time = millis() + MILLIS_BEFORE_SUNRISE_START;
    }
    sunrise_point = args["start"].as<int>();
    sunset_point = args["end"].as<int>();
    speed = args["speed"].as<int>();

    args.garbageCollect();

    SKY_COLOR = CHSV(0, 255, 60);
    SUN_COLOR = CRGB(255, 0, 0);
}
SkyMode::SkyMode(const char *data)
{
    Serial.print("sunrise start: ");
    Serial.println(sunrise_start_time);
    sunrise_start_time = millis() + MILLIS_BEFORE_SUNRISE_START;
    update_args(data);
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
        skyHue = (SecondsSinceSunriseStart - SECONDS_BEFORE_HUE_TILT) * 0.8F;
        skyValue = 120.0F * phase;
        SKY_COLOR = CHSV(skyHue, 255, skyValue);
    }

    for (int i = 0; i < NUMPIXELS; i++)
    {
        leds[i] = SKY_COLOR;
    }
}

void SkyMode::ShowSunriseSun(float SecondsSinceSunriseStart, CRGB *leds)
{
    float phase = ((SecondsSinceSunriseStart) / SECONDS_BEFORE_SUN_FULLY_CAME_OUT);

    if (phase >= 1.0F)
        phase = 1.0F;

    float SunOffset = SecondsSinceSunriseStart * 0.15F;

    // for (int i = sunrise_point + floor(SunOffset); i < sunrise_point + SUN_RADIUS + 3 + floor(SunOffset); i++)
    for (int i = 0; i < NUMPIXELS; i++)
    {
        if (i < sunrise_point + floor(SunOffset) - (SUN_RADIUS + 1) || i > sunrise_point + floor(SunOffset) + (SUN_RADIUS + 1))
            continue;

        float sunAlpha = cos((((float)(i - sunrise_point) / (float)SUN_RADIUS) * HALF_PI) - SunOffset * (HALF_PI / SUN_RADIUS)); // to simulate that sun is a sphere
        // float sunAlpha = cos((((float)(i - sunrise_point) * HALF_PI) - SunOffset) / SUN_RADIUS); // to simulate that sun is a sphere

        if (sunAlpha < 0.0F)
            sunAlpha = 0.0F;

        float multiplier = phase - (1.0F - sunAlpha);

        if (multiplier <= 0.0F)
            multiplier = 0.0F;

        CRGB new_color = SKY_COLOR + CRGB(SUN_COLOR.r * multiplier,
                                          SUN_COLOR.g * multiplier,
                                          SUN_COLOR.b * multiplier);

        leds[i] = new_color;
    }
}