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
    void ChangeMode(int, JsonArray &);
    void ChangeMode(int);
    void update(CRGB *);
    ModeHandler();
    ~ModeHandler();
};

#endif