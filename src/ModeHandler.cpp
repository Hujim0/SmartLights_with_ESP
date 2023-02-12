#include <ModeHandler.h>
#include <StaticMode.h>
#include <RainbowMode.h>
#include <WaveMode.h>
#include <SkyMode.h>

void ModeHandler::LightSwitch(bool state)
{
    if (led_state == state)
        return;

    led_state = state;

    if (!state)
    {
        FastLED.showColor(CRGB::Black);
        return;
    }
}

void ModeHandler::ChangeMode(int id, const char *args)
{
    FastLED.clearData();

    current_mode_id = id;

    switch (id)
    {
    case 0:
        current_mode = new StaticMode(args);
        return;

    case 1:
        current_mode = new RainbowMode(args);
        return;

    case 2:
        current_mode = new WaveMode(args);
        return;
    case 3:
        current_mode = new SkyMode(args);
        return;

    default:
        ChangeMode(0);
        return;
    }
}
void ModeHandler::ChangeMode(int id)
{
    FastLED.clearData();

    current_mode_id = id;

    switch (id)
    {
    case 0:
        current_mode = new StaticMode();
        return;
    case 1:
        current_mode = new RainbowMode();
        return;
    case 2:
        current_mode = new WaveMode();
        return;
    case 3:
        current_mode = new SkyMode();
        return;

    default:
        ChangeMode(0);
        break;
    }
}

void ModeHandler::update(CRGB *leds)
{
    current_mode->update(leds);
}

ModeHandler::ModeHandler()
{
}

ModeHandler::~ModeHandler()
{
}