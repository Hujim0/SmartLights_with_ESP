#include <WaveMode.h>

void WaveMode::update(CRGB *leds)
{
    for (int i = 0; i < NUMPIXELS; i++)
    {
        float multiplier = sin((i * (length)) + offset);

        if (multiplier <= 0.01F)
        {
            leds[i] = color;
            continue;
        }

        float intensity_const = 1.0F - (multiplier * intensity);

        leds[i] = CRGB(color.r * intensity_const,
                       color.g * intensity_const,
                       color.b * intensity_const);
    }
    if (offset >= TWO_PI * 10)
    {
        offset = 0;
        return;
    }
    offset += speed * (((length + 1.0F) * 2) / 255.0F); // to make it the same as in rainbow mode
}

WaveMode::WaveMode(StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> &args)
{
    color = CRGB(args["r"].as<int>(),
                 args["g"].as<int>(),
                 args["b"].as<int>());

    speed = args["speed"].as<float>();
    intensity = args["intensity"].as<float>() * 0.01F;
    if (args["length"].as<float>() == 0.0F)
    {
        length = 0.0F;
    }
    else
    {
        length = PI / (args["length"].as<float>());
    }

    reversed = args["reversed"].as<bool>();

    if (!reversed)
    {
        speed = speed * -1.0F;
    }
}

WaveMode::WaveMode()
{
    color = CRGB::White;

    speed = 10;
    intensity = 90;
    length = 10;

    reversed = false;
}

WaveMode::~WaveMode()
{
    free(&offset);
}
