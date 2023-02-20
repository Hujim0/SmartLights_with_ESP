#ifndef STATICMODE_H
#define STATICMODE_H

#include <Mode.h>

class StaticMode : public Mode
{
private:
    CRGB color;

public:
    void update(CRGB *);
    void update_args(const char *data);
    StaticMode();
    StaticMode(const char *);
    ~StaticMode();
};

#endif