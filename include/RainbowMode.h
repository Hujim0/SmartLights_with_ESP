#ifndef RAINBOWMODE_H
#define RAINBOWMODE_H

#include <Mode.h>

class RainbowMode : public Mode
{
private:
    float hue;
    bool reversed;
    float hueConst;

    float speed;
    int count;

public:
    void update(CRGB *);
    RainbowMode();
    RainbowMode(StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> &);
    ~RainbowMode();
};

#endif