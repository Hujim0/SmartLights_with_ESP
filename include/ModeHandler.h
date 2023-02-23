#ifndef MODEHANDLER_H
#define MODEHANDLER_H

#include <Mode.h>

#define LIGHT_SWITCH "light_switch"
#define MODE_SWITCH "mode_switch"
#define MODE_UPDATE "mode_update"
#define BRIGHTNESS "brightness"
#define ARGS_REQUEST "mode_args_request"
#define STREAM_OPEN "open_stream"
#define STREAM_CLOSE "close_stream"

class ModeHandler
{
private:
    Mode *current_mode = NULL;

public:
    int current_mode_id;
    bool led_state = true;
    void LightSwitch(bool);
    void ChangeMode(int, const char *);
    void ChangeMode(int);
    void UpdateArgs(const char *);
    void PushArg(String, String);
    void update(CRGB *);
    ModeHandler();
    ~ModeHandler();
};

#endif