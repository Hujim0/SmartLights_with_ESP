#ifndef STATICMODE_H
#define STATICMODE_H

#include <Mode.h>

class StaticMode : public Mode
{
private:
    CRGB color;

public:
    void update(CRGB *);
    StaticMode();
    StaticMode(JsonArray &);
    ~StaticMode();
};

#endif