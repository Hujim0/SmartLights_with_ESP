#ifndef WAVEMODE_H
#define WAVEMODE_H

#include <Mode.h>

class WaveMode : public Mode
{
private:
    float offset;

    float speed;
    float length;

public:
    void setup(CRGB *);
    void update(CRGB *);
    WaveMode();
    WaveMode(JsonArray &);
    ~WaveMode();
};

#endif