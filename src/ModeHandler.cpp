#include <ModeHandler.h>
#include <StaticMode.h>
#include <RainbowMode.h>

#define LIGHT_SWITCH "light_switch"
#define MODE_SWITCH "mode_switch"
#define BRIGHTNESS "brightness"

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

void ModeHandler::ChangeMode(int id, StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> &args)
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

void ModeHandler::ChangeModeFromJson(String data)
{

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, data.c_str());

    String event_type = doc["event"];

    if (event_type == LIGHT_SWITCH)
    {
        bool light_switch = doc["value"].as<bool>();
        LightSwitch(light_switch);
    }
    else if (event_type == MODE_SWITCH)
    {
        int id = doc["value"].as<int>();

        if (!doc.containsKey("args"))
        {
            ChangeMode(id);
            return;
        }

        StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> args = doc["args"];

        ChangeMode(id, args);

        args.garbageCollect();
    }
    else if (event_type == BRIGHTNESS)
    {
        int brightness = doc["value"].as<int>();
        FastLED.setBrightness(brightness);
    }
    doc.garbageCollect();
}

void ModeHandler::ChangeModeFromJson(String data, DynamicJsonDocument &preferences)
{
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, data.c_str());

    String event_type = doc["event"];

    if (event_type == LIGHT_SWITCH)
    {
        bool light_switch = doc["value"].as<bool>();
        LightSwitch(light_switch);
        preferences["light_switch"] = light_switch;
    }
    else if (event_type == MODE_SWITCH)
    {
        int id = doc["value"].as<int>();

        if (!doc.containsKey("args"))
        {
            ChangeMode(id);
            preferences["mode"] = id;
            return;
        }

        StaticJsonDocument<512> args = doc["args"];

        ChangeMode(id, args);
        preferences["mode"] = id;
        preferences["args"][id] = args;

        args.garbageCollect();
    }
    else if (event_type == BRIGHTNESS)
    {
        int brightness = doc["value"].as<int>();
        FastLED.setBrightness(brightness);
        preferences["brightness"] = brightness;
    }

    doc.garbageCollect();
}

ModeHandler::ModeHandler()
{
}

ModeHandler::~ModeHandler()
{
}