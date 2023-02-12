#pragma region fastled include

#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_ESP8266_D1_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0

#include <Arduino.h>
#include <FastLED.h>

#pragma endregion

// #define DEBUG_HEAP
// #define DEBUG_PREFERENCES
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

    FastLED.addLeds<STRIP, STRIP_PIN, COLOR_ORDER>(leds, NUMPIXELS);

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

bool checkPreferences(DynamicJsonDocument &pref)
{
    for (int i = 0; i < (int)pref["args"].size(); i++)
    {
        if (pref["args"][i].size() == 0 || pref["args"][i] == NULL)
        {
            Serial.println(i);
            return false;
        }
    }

    return true;
}
void OnWebSocketMessage(String data)
{
    // if (data[0] != '{')
    //     return;
    // serializeJson(preferences, Serial);
    // Serial.print("--endln ");
    // modeHandler.ChangeModeFromJson(data, preferences);
    // bool isValid = checkPreferences(preferences);
    // Serial.print("preferences is valid = ");
    // Serial.println(isValid);
    // if (!isValid)
    // {
    //     serializeJson(preferences, Serial);
    //     return;
    // }
    // String json;
    // serializeJsonPretty(preferences, json);
    // SavePreferences(json);

    if (data[0] != '{')
        return;

    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> preferences;
    deserializeJson(preferences, GetPreferences());

    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> doc;
    deserializeJson(doc, data.c_str());

    modeHandler.ChangeModeFromJson(doc);

    serializeJson(doc, Serial);

    if (doc["event"] == MODE_SWITCH)
    {
        int mode_id = doc["value"].as<int>();
        preferences["mode"] = mode_id;
        preferences["args"][mode_id] = doc["args"];
        // JsonObject obj = preferences["args"][mode_id];
        // JsonObject args = doc["args"];
        // // MemoryPool pool =
        // obj.clear();
        // for (JsonPair kv : args)
        // {
        //     obj[kv.key().c_str()] = args[kv.key().c_str()];
        // }
    }

    if (doc["event"] == BRIGHTNESS)
        preferences[BRIGHTNESS] = doc["value"].as<int>();
    if (doc["event"] == LIGHT_SWITCH)
        preferences[LIGHT_SWITCH] = doc["value"].as<bool>();

    String json;
    serializeJsonPretty(preferences, json);
    SavePreferences(json);
    // doc.garbageCollect();
    preferences.garbageCollect();
}

void ChangeSettingsFromPreferences(String data)
{
    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> doc;
    deserializeJson(doc, data);

    modeHandler.LightSwitch(doc["light_switch"].as<bool>());
    FastLED.setBrightness(doc["brightness"].as<int>());

    int last_id = doc["mode"].as<int>();
    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> args = doc["args"][last_id];

    modeHandler.ChangeMode(
        last_id, args);

    args.garbageCollect();
    doc.garbageCollect();
}