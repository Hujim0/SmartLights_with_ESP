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
    virtual void update_args(const char *data) = 0;

    static uint32_t toHex(String);
    virtual ~Mode() {}
};

#endif