#include <RainbowMode.h>

void RainbowMode::update(CRGB *leds)
{
    if (hue >= 255)
        hue = 0;

    for (int i = 0; i < NUMPIXELS; i++)
    {
        leds[i] = CHSV(hue + (i * hueConst), 255, 255);
    }

    if (hueConst != 0)
    {
        hue += ((speed * 0.1F) * std::abs(hueConst));
        return;
    }

    hue += (speed * 0.1F);
}

RainbowMode::RainbowMode()
{
    hue = 0;
    speed = 10;
    count = 1;
    reversed = false;

    hueConst = (255.0 * count) / (NUMPIXELS);
    if (!reversed) // should be the other way around but i like more when its reversed, so i will keep it a default
    {
        hueConst *= -1;
    }
}

RainbowMode::RainbowMode(JsonArray &args)
{
    speed = args[0].as<float>();
    count = args[1].as<int>();
    reversed = args[2].as<bool>();

    hueConst = (255.0 * count) / (NUMPIXELS);
    if (!reversed)
    {
        hueConst *= -1;
    }
}
RainbowMode::~RainbowMode()
{
    hue = 0;
    speed = 0;
    count = 0;
    reversed = false;
    Serial.print("freeed");
}
