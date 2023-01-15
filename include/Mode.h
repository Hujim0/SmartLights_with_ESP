#ifndef MODE_H
#define MODE_H

#include <FastLED.h>
#include <main.h>
#include <ArduinoJson.h>
using std::array;

class Mode
{
public:
    // pure virtual methods
    virtual void update(CRGB *) = 0;
    virtual ~Mode() {}
};

#endif