#ifndef SKYMODE_H
#define SKYMODE_H

#include <Mode.h>

class SkyMode : public Mode
{
private:
    bool edit_mode;
    int sunrise_point;
    int sunset_point;
    int speed;

    unsigned long sunrise_start_time;

    void ShowEditMode(CRGB *);
    void ShowSunriseLight(float, CRGB *);
    void ShowSunriseSun(float, CRGB *);

    CRGB SUN_COLOR;
    CRGB SKY_COLOR;

    float skyHue;
    float skyValue;

public:
    void update(CRGB *);
    void update_args(const char *);
    void update_arg(String arg, String value);
    SkyMode();
    SkyMode(const char *);
    ~SkyMode();
};

#endif