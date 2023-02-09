#ifndef WAVEMODE_H
#define WAVEMODE_H

#include <Mode.h>

class WaveMode : public Mode
{
private:
    CRGB color;

    float offset;

    float speed;
    float length;
    float intensity;
    bool reversed;

public:
    void setup(CRGB *);
    void update(CRGB *);
    WaveMode();
    WaveMode(StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> &);
    ~WaveMode();
};

#endif