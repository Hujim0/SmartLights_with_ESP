#pragma region fastled include

#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_ESP8266_D1_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0

#include <Arduino.h>
#include <FastLED.h>

#pragma endregion

// #define DEBUG_HEAP
//  #define DEBUG_PREFERENCES
#define DEBUG_WIFI

#include <main.h>
#include <ModeHandler.h>
#include <NetworkManager.h>
#include <FileSystem.h>

#include <ArduinoJson.h>

CRGB leds[NUMPIXELS];

ModeHandler modeHandler;
NetworkManager network = NetworkManager();
#define INITIAL_DELAY 3000

void OnClientConnected(int);
void OnWebSocketMessage(String);
void ChangeSettingsFromPreferences(String data);

void ledSetup()
{
    pinMode(STRIP_PIN, OUTPUT);

    FastLED.addLeds<STRIP, STRIP_PIN, COLOR_ORDER>(leds, NUMPIXELS).setCorrection(TypicalLEDStrip);
    FastLED.setMaxRefreshRate(60);

    FastLED.clearData();
    FastLED.clear();
    FastLED.show();
}

void setup()
{
    delay(INITIAL_DELAY);

    Serial.begin(115200);
    Serial.println("[ESP] loaded");

    FSBegin();

    // preferences load
    Serial.println("[ESP] loading preferences...");

    ChangeSettingsFromPreferences(GetPreferences());

#ifdef DEBUG_PREFERENCES
    Serial.print("Loaded settings: ");
    Serial.println(preferences_json);
#endif

    Serial.println("------------------------------------------------------------------");

    // network setup
    String wifi_data[2];
    GetWifiSettings(wifi_data);

#ifdef DEBUG_WIFI
    Serial.println("Wifi config:");
    Serial.print("ssid: ");
    Serial.println(wifi_data[0].c_str());
    Serial.print("pass: ");
    Serial.println(wifi_data[1].c_str());
    Serial.println("------------------------------------------------------------------");
#endif

    network.Begin(wifi_data[0].c_str(), wifi_data[1].c_str());

    network.OnNewClient(OnClientConnected);
    network.OnNewMessage(OnWebSocketMessage);

    ledSetup();
}

unsigned long timer = millis();

void loop()
{
    if (timer + 1000 <= millis())
    {
#ifdef DEBUG_HEAP
        Serial.print("Avalible ram: ");
        Serial.print(ESP.getFreeHeap());
        Serial.println(" bytes");
#endif
        timer = millis();

        network.CleanUp();

        network.CheckStatus();

        ESP.resetHeap();
    }

    if (modeHandler.led_state)
    {
        modeHandler.update(leds);
        FastLED.show();
    }
}

void OnClientConnected(int id)
{
    String json;

    network.SentTextToClient(id, GetPreferences().c_str());
}

void OnWebSocketMessage(String data)
{
    if (data[0] != '{')
        return;

    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> doc;
    deserializeJson(doc, data.c_str());

    if (!doc.containsKey("event"))
    {
        doc.garbageCollect();

        modeHandler.ChangeMode(modeHandler.current_mode_id, data.c_str());

        SaveModeArgs(modeHandler.current_mode_id, data);
    }

    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> preferences;
    deserializeJson(preferences, GetPreferences());

    if (doc["event"] == MODE_SWITCH)
    {
        int mode_id = doc["value"].as<int>();
        preferences["mode"] = mode_id;
        network.SentTextToAll(GetModeArgs(mode_id).c_str());

        modeHandler.ChangeMode(mode_id, GetModeArgs(mode_id).c_str());
    }
    else if (doc["event"] == BRIGHTNESS)
    {
        int value = doc["value"].as<int>();
        FastLED.setBrightness(value);
        preferences[BRIGHTNESS] = value;
    }
    else if (doc["event"] == LIGHT_SWITCH)
    {
        bool value = doc["value"].as<bool>();
        modeHandler.LightSwitch(value);
        preferences[LIGHT_SWITCH] = value;
    }
    else if (doc["event"] == ARGS_REQUEST)
    {
        int mode_id = doc["value"].as<int>();
        network.SentTextToAll(GetModeArgs(mode_id).c_str());
    }

    String json;
    serializeJsonPretty(preferences, json);
    SavePreferences(json);

    preferences.garbageCollect();
    doc.garbageCollect();
    return;
}

void ChangeSettingsFromPreferences(String data)
{
    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> preferences;
    deserializeJson(preferences, data);

    modeHandler.LightSwitch(preferences["light_switch"].as<bool>());
    FastLED.setBrightness(preferences["brightness"].as<int>());

    int mode_id = preferences["mode"].as<int>();

    preferences.garbageCollect();

    modeHandler.ChangeMode(
        mode_id, GetModeArgs(mode_id).c_str());
}