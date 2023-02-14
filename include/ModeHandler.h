#ifndef MODEHANDLER_H
#define MODEHANDLER_H

#include <Mode.h>

#define LIGHT_SWITCH "light_switch"
#define MODE_SWITCH "mode_switch"
#define MODE_UPDATE "mode_update"
#define BRIGHTNESS "brightness"
#define ARGS_REQUEST "mode_args_request"

class ModeHandler
{
private:
    Mode *current_mode;

public:
    int current_mode_id;
    bool led_state = true;
    void LightSwitch(bool);
    void ChangeMode(int, const char *);
    void ChangeMode(int);
    // void ChangeModeFromJson(StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE>);
    void update(CRGB *);
    ModeHandler();
    ~ModeHandler();
};

#endif