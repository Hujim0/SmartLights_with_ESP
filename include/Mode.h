#ifndef MODE_H
#define MODE_H

#include <FastLED.h>
#include <main.h>
#include <ArduinoJson.h>

#define STATIC_DOCUMENT_MEMORY_SIZE 512

class Mode
{
public:
    // pure virtual method
    virtual void update(CRGB *) = 0;
    virtual ~Mode() {}
};

#endif