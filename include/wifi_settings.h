#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>

#define LIGHT_SWITCH "light_switch"
#define MODE_SWITCH "mode_switch"
#define BRIGHTNESS "brightness"

#define HTTP_PORT 10200

#define INITIAL_DELAY 3000

#define ATTEMPT_DURATION 30000