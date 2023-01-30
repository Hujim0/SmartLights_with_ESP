#ifndef MODEHANDLER_H
#define MODEHANDLER_H

#include <Mode.h>

class ModeHandler
{
private:
    Mode *current_mode;
    bool ready = false;

public:
    bool led_state = true;
    void LightSwitch(bool);
    void ChangeMode(int, StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> &);
    void ChangeMode(int);
    void ChangeModeFromJson(String);
    void ChangeModeFromJson(String, DynamicJsonDocument &);
    void update(CRGB *);
    ModeHandler();
    ~ModeHandler();
};

#endif