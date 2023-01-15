#include <ModeHandler.h>
#include <StaticMode.h>
#include <RainbowMode.h>

void ModeHandler::LightSwitch(bool state)
{
    if (led_state == state)
        return;

    led_state = state;

    if (!state)
    {
        FastLED.showColor(CRGB::Black);
    }
}

void ModeHandler::ChangeMode(int id, JsonArray &args)
{
    switch (id)
    {
    case 0:
        current_mode = new StaticMode(args);
        ready = true;
        return;

    case 1:
        current_mode = new RainbowMode(args);
        ready = true;
        return;

    default:
        ChangeMode(0);
        return;
    }
}
void ModeHandler::ChangeMode(int id)
{
    switch (id)
    {
    case 0:
        current_mode = new StaticMode();
        ready = true;
        return;
    case 1:
        current_mode = new RainbowMode();
        ready = true;
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