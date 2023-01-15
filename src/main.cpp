#pragma region fastled include

#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_ESP8266_D1_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0

#include <Arduino.h>
#include <FastLED.h>

#pragma endregion

#include <main.h>
#include <ModeHandler.h>

#include <ArduinoJson.h>

#include <wifi_settings.h>
#include <LittleFS.h>

#pragma region fastled setup

CRGB leds[NUMPIXELS];

ModeHandler modeHandler;

DynamicJsonDocument preferences(1024);

void ledSetup()
{
    modeHandler = ModeHandler();

    FastLED.addLeds<STRIP, STRIP_PIN, COLOR_ORDER>(leds, NUMPIXELS);
    FastLED.setBrightness(20);
}

#pragma endregion

#pragma region network

AsyncWebServer server(HTTP_PORT);
AsyncWebSocket ws("/ws");
WiFiClient client;

void onEvent(AsyncWebSocket *, AsyncWebSocketClient *, AwsEventType, void *, uint8_t *, size_t);
void handleWebSocketMessage(void *, uint8_t *, size_t);
void SentPreferences(int);
void SavePreferences();

void networkSetup()
{
    // get wifi parameters
    File file = LittleFS.open(WIFI_SETTINGS_PATH, "r");

    String ssid = file.readStringUntil('\n');
    String password = file.readString();

    ssid.trim();
    password.trim();

    file.close();

    // connection to wifi
    WiFi.begin(ssid.c_str(), password.c_str());

    Serial.print("[ESP] Connecting to ");
    Serial.print(ssid);

    while (WiFi.status() != WL_CONNECTED && millis() < INITIAL_DELAY + ATTEMPT_DURATION)
    {
        Serial.print(".");
        delay(500);
    }

    // if esp cant connect within 30 secs, reset and try again
    if (WiFi.status() != WL_CONNECTED)
    {
        ESP.restart();

        return;
    }

    Serial.println("success");

    // server setup
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(
                    request->beginResponse(LittleFS, INDEX_HTML_PATH, "text/html")); });

    server.begin();

    // websocket setup
    ws.onEvent(onEvent);
    server.addHandler(&ws);

    // print server url
    Serial.print("[ESP] HTTP server started at \"http://");
    Serial.print(WiFi.localIP());
    Serial.print(":");
    Serial.print(HTTP_PORT);
    Serial.println("\"");
    Serial.println("------------------------------------------------------------------");
}

// websocket stuff
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        SentPreferences(client->id());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) // check that its a valid arg
    {
        data[len] = 0;

        Serial.print("From websocket: ");
        Serial.println((char *)data);
        if (((char *)data)[0] != '{')
        {
            return;
        }

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, (char *)data);

        String event_type = doc["event"];

        if (event_type == LIGHT_SWITCH)
        {
            bool light_switch = doc["value"].as<bool>();
            modeHandler.LightSwitch(light_switch);
            preferences["light_switch"] = light_switch;
            SavePreferences();
        }
        else if (event_type == MODE_SWITCH)
        {
            int id = doc["value"].as<int>();

            if (!doc.containsKey("args"))
            {
                modeHandler.ChangeMode(id);
                preferences["mode"] = id;
                SavePreferences();
                return;
            }

            JsonArray args = doc["args"];

            modeHandler.ChangeMode(id, args);
            preferences["mode"] = id;
            preferences["args"][id] = args;
            SavePreferences();
        }
        else if (event_type == BRIGHTNESS)
        {
            int brightness = doc["value"].as<int>();
            FastLED.setBrightness(brightness);
            preferences["brightness"] = brightness;
            SavePreferences();
        }

        doc.garbageCollect();
    }
}

void SentPreferences(int id)
{
    String json;
    serializeJson(preferences, json);
    ws.text(id, json);
}

void SavePreferences()
{
    File file = LittleFS.open(PREFRENCES_PATH, "w");
    String json;
    serializeJson(preferences, json);
    Serial.println(json);
    file.write(json.c_str());
    file.close();
}

void notifyClients(String arg)
{
    ws.textAll(arg);
}

#pragma endregion

void setup()
{
    delay(INITIAL_DELAY);

    LittleFS.begin();

    Serial.begin(115200);
    Serial.println("[ESP] loaded");
    Serial.println("[ESP] loading preferences...");

    File file = LittleFS.open(PREFRENCES_PATH, "r");
    String file_contents = file.readString();
    deserializeJson(preferences, file_contents);

    file.close();
    networkSetup();

    ledSetup();

    String json;
    serializeJson(preferences, json);
    Serial.println(file_contents);

    modeHandler.LightSwitch(preferences["light_switch"].as<bool>());
    FastLED.setBrightness(preferences["brightness"].as<int>());

    int last_id = preferences["mode"].as<int>();
    JsonArray args = preferences["args"][last_id];

    modeHandler.ChangeMode(
        last_id, args);
}

unsigned long timer = millis();

void loop()
{
    if (timer + 10000 <= millis())
    {
        Serial.print("Avalible ram: ");
        Serial.print(ESP.getFreeHeap());
        Serial.println(" bytes");
        timer = millis();
        ESP.resetHeap();
    }

    ws.cleanupClients();

    if (modeHandler.led_state)
    {
        modeHandler.update(leds);
        FastLED.show();
    }
}
