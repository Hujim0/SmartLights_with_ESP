#ifndef SKYMODE_H
#define SKYMODE_H

#include <Mode.h>

class SkyMode : public Mode
{
private:
    bool pivot_mode;
    int sunrise_point;
    int sunset_point;
    int length;

    unsigned long sunrise_start_time;

    void ShowEditMode(CRGB *);
    void ShowSunriseLight(float, CRGB *);
    void ShowSunriseSun(float, CRGB *);

    CRGB SUNRISE_COLOR;

public:
    void update(CRGB *);
    SkyMode();
    SkyMode(const char *);
    ~SkyMode();
};

#endif