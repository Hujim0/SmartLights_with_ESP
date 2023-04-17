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

#include <main.h>
#include <ModeHandler.h>
#include <NetworkManager.h>
#include <FileSystem.h>
#include <TimeManager.h>

#include <ArduinoJson.h>

CRGB leds[NUMPIXELS];

ModeHandler modeHandler;
NetworkManager network = NetworkManager();
TimeManager timeManager;
String lang = "en";
#define INITIAL_DELAY 1500

void ledSetup()
{
    pinMode(STRIP_PIN, OUTPUT);

    FastLED.addLeds<STRIP, STRIP_PIN, COLOR_ORDER>(leds, NUMPIXELS).setCorrection(TypicalLEDStrip);
    FastLED.setMaxRefreshRate(60);

    FastLED.clearData();
    FastLED.clear();
    FastLED.show();
}

void networkSetup()
{
    network.OnNewClient(OnClientConnected);
    network.OnNewMessage(OnWebSocketMessage);
    network.OnConnectionLost(networkSetup);

    String wifi_data[2];
    GetWifiSettings(wifi_data);

    if (!network.Begin(wifi_data[0].c_str(), wifi_data[1].c_str()))
    {
        networkSetup();
        return;
    }

    network.ServeStatic("/data", LittleFS, "/", "max-age=600");

    // global
    network.AddWebPageHandler("http://local_lumify/", [](AsyncWebServerRequest *request)
                              { request->redirect(network.getUrl() + lang + "/home"); });

    network.AddWebPageHandler("/", [](AsyncWebServerRequest *request)
                              { request->redirect("/" + lang + "/home"); });

    network.AddWebPageHandler("/mode", [](AsyncWebServerRequest *request)
                              {
                                int id = request->arg("id").toInt();

                                String args = GetModeArgs(id);

                                if (!request->hasArg("request"))
                                {
                                    __try
                                    {
                                      modeHandler.ChangeMode(id, args.c_str());

                                      SaveModeArgs(id, args);
                                    }
                                    __catch (const std::exception& ex) {
                                        Serial.println("[error]" + String(ex.what()));
                                        request->send(404);
                                        return;
                                    }
                                    String json = GetPreferences();
                                    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> preferences;
                                    deserializeJson(preferences, json);
                                    preferences["mode"] = id;
                                    String json_save;
                                    serializeJsonPretty(preferences, json_save);
                                    SavePreferences(json_save);
                                    preferences.garbageCollect();
                                } 

                                request->send(
                                    request->beginResponse(HTTP_POST, "text/json", args)); });

    network.AddWebPageHandler("/elements", [](AsyncWebServerRequest *request)
                              { request->send(
                                    request->beginResponse(LittleFS, "modes/elements/" + lang + "/elements" + request->arg("id") + ".json", "text/json")); });

    network.AddWebPageHandler("/changelang", [](AsyncWebServerRequest *request)
                              {
        ChangeLanguage(request->arg("lang"));
        request->redirect("/" + request->arg("lang") + "/home"); });

    //====================================================

    // pages
    network.AddWebPageHandler("/en/home", [](AsyncWebServerRequest *request)
                              { request->send(
                                    request->beginResponse(LittleFS, "web/home/home.html", "text/html")); });
    network.AddWebPageHandler("/ru/home", [](AsyncWebServerRequest *request)
                              { request->send(
                                    request->beginResponse(LittleFS, "web/home/home_ru.html", "text/html")); });

    //------------------------------------------------------
    network.AddWebPageHandler("/en/schedule", [](AsyncWebServerRequest *request)
                              { request->send(
                                    request->beginResponse(LittleFS, "web/schedule/schedule.html", "text/html")); });
    network.AddWebPageHandler("/ru/schedule", [](AsyncWebServerRequest *request)
                              { request->send(
                                    request->beginResponse(LittleFS, "web/schedule/schedule_ru.html", "text/html")); });
}

// void AddWebResponce(const char *page_name, const char *file_path, const char *content_type)
// {
//     network.AddWebPageHandler(page_name, [](AsyncWebServerRequest *request)
//                               { request->send(
//                                     request->beginResponse(LittleFS, file_path, content_type)); });
// }
void ChangeLanguage(String _lang)
{
    lang = _lang;

    String json = GetPreferences();

    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> preferences;
    deserializeJson(preferences, json);

    preferences["lang"] = _lang;

    String json_save;
    serializeJsonPretty(preferences, json_save);
    SavePreferences(json_save);

    preferences.garbageCollect();
}

void setup()
{
    delay(INITIAL_DELAY);

    Serial.begin(115200);
    Serial.println("[ESP] loaded");

    FSBegin();

    LoadFromPreferences(GetPreferences());

    networkSetup();

    ledSetup();

    modeHandler.ChangeMode(
        modeHandler.current_mode_id,
        GetModeArgs(modeHandler.current_mode_id).c_str());
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
    }
    FastLED.show();

    timeManager.Update();
}

void OnClientConnected(int id)
{
    network.SentTextToClient(id, GetPreferences().c_str());
}

String current_stream = "";

void OnWebSocketMessage(String data)
{
    if (current_stream != "")
    {
        if (data.endsWith("]"))
        {
            current_stream = "";
            return;
        }

        if (current_stream == BRIGHTNESS)
            FastLED.setBrightness(data.toInt());
        else
            modeHandler.PushArg(current_stream, data);

        return;
    }

    if (data[0] != '{')
        return;

    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> doc;
    deserializeJson(doc, data.c_str());

    if (!doc.containsKey("event"))
    {
        doc.garbageCollect();

        modeHandler.UpdateArgs(data.c_str());

        SaveModeArgs(modeHandler.current_mode_id, data);

        return;
    }

    // for events that dont need preferences
    if (doc["event"] == ARGS_REQUEST)
    {
        int mode_id = doc["value"].as<int>();
        network.SentTextToAll(GetModeArgs(mode_id).c_str());

        network.SentTextToAll(GetElements(mode_id).c_str());
        return;
    }
    else if (doc["event"] == STREAM_OPEN)
    {
        current_stream = doc["value"].as<String>();
        return;
    }

    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> preferences;
    deserializeJson(preferences, GetPreferences());

    if (doc["event"] == MODE_SWITCH)
    {
        int mode_id = doc["value"].as<int>();
        preferences["mode"] = mode_id;
        network.SentTextToAll(GetModeArgs(mode_id).c_str());

        network.SentTextToAll(GetElements(mode_id).c_str());

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
    else if (doc["event"] == EPOCH_TIME)
    {
        timeManager.Setup(doc["value"].as<uint16_t>(), doc["dayOfTheWeek"].as<int>());
    }

    String json;
    serializeJsonPretty(preferences, json);
    SavePreferences(json);

    preferences.garbageCollect();
    doc.garbageCollect();
}

void LoadFromPreferences(String data)
{
    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> preferences;
    deserializeJson(preferences, data);

#ifdef DEBUG_PREFERENCES
    Serial.println("[ESP] loading preferences...");

    Serial.print("Loaded settings: ");
    Serial.println(data);

    Serial.println("------------------------------------------------------------------");

#endif

    modeHandler.LightSwitch(preferences["light_switch"].as<bool>());
    FastLED.setBrightness(preferences["brightness"].as<int>());

    modeHandler.current_mode_id = preferences["mode"].as<int>();

    preferences.garbageCollect();
}