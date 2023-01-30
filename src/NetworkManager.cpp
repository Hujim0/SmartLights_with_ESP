#include <NetworkManager.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// singleton initializer
NetworkManager *NetworkManager::Instance = 0;

static void onNewEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                       void *arg, uint8_t *data, size_t len);

// websocket stuff
void NetworkManager::handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (!info->final || info->index != 0 || info->len != len || info->opcode != WS_TEXT) // check that its a valid arg
    {
        return;
    }
    data[len] = 0;

    Serial.print("From websocket: \"");
    Serial.print((char *)data);
    Serial.println("\" --endln");

    if (onNewClientHandler != NULL)
    {
        onNewMessageHandler((String)(char *)data);
    }
}

void NetworkManager::SentTextToClient(int id, const char *data)
{
    webSocket.text(id, data);
}

void NetworkManager::Begin(const char *ssid, const char *password)
{
    Instance = this;

    // connection to wifi
    WiFi.begin(ssid, password);

    Serial.print("[ESP] Connecting to ");
    Serial.print(ssid);

    while (WiFi.status() != WL_CONNECTED && millis() < ATTEMPT_DURATION)
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
    server.addHandler(&webSocket);
    webSocket.onEvent(onNewEvent);

    // print server url
    Serial.print("[ESP] HTTP server started at \"http://");
    Serial.print(WiFi.localIP());
    Serial.print(":");
    Serial.print(HTTP_PORT);
    Serial.println("\"");
    Serial.println("------------------------------------------------------------------");
}

void NetworkManager::OnNewClient(OnNewClientHandler handler)
{
    onNewClientHandler = handler;
}

void NetworkManager::OnNewMessage(OnNewMessageHandler handler)
{
    onNewMessageHandler = handler;
}

void NetworkManager::CleanUp()
{
    webSocket.cleanupClients();
}

static void onNewEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                       void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.print("websocket client #");
        Serial.print(client->id());
        Serial.print(" connected from ");
        Serial.println(client->remoteIP().toString().c_str());

        if (NetworkManager::Instance->onNewMessageHandler != NULL)
        {
            NetworkManager::Instance->onNewClientHandler(client->id());
        }

        break;
    case WS_EVT_DISCONNECT:
        Serial.print("websocket client #");
        Serial.print(client->id());
        Serial.print(" disconnected");
        break;
    case WS_EVT_DATA:
        NetworkManager::Instance->handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}