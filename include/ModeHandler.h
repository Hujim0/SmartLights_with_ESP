#ifndef MODEHANDLER_H
#define MODEHANDLER_H

#include <Mode.h>

#define LIGHT_SWITCH "light_switch"
#define MODE_SWITCH "mode_switch"
#define MODE_UPDATE "mode_update"
#define BRIGHTNESS "brightness"

class ModeHandler
{
private:
    Mode *current_mode;

public:
    bool led_state = true;
    void LightSwitch(bool);
    void ChangeMode(int, StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> &);
    void ChangeMode(int);
    void ChangeModeFromJson(int, StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE>);
    void update(CRGB *);
    ModeHandler();
    ~ModeHandler();
};

#endif