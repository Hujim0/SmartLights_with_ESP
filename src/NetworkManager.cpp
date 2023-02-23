#include <NetworkManager.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// const IPAddress ip(192, 168, 1, 17); // статический IP
// const IPAddress gateway(192, 168, 1, 1);
// const IPAddress subnet(255, 255, 255, 0);

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

    Serial.print("[Websocket] Got: \"");
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
void NetworkManager::SentTextToAll(const char *data)
{
    Serial.print("Texted to all: ");
    Serial.println(data);
    webSocket.textAll(data);
}

bool NetworkManager::Begin(const char *ssid, const char *password)
{
    Instance = this;

    // connection to wifi
    WiFi.begin(ssid, password);

    Serial.print("[ESP] Connecting to ");
    Serial.print(ssid);
    Serial.print("...");

    if (WiFi.waitForConnectResult(ATTEMPT_DURATION) != WL_CONNECTED)
    {
        // ESP.restart();
        return false;
    }

    // WiFi.config(ip, gateway, subnet);

    Serial.println("success");
    // server setup
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(
                    request->beginResponse(LittleFS, INDEX_HTML_PATH, "text/html")); });

    server.begin();

    // websocket setup
    server.addHandler(&webSocket);
    webSocket.onEvent(onNewEvent);
    webSocket.closeAll();

    // print server url
    Serial.print("[ESP] HTTP server started at \"http://");
    Serial.print(WiFi.localIP());
    Serial.print(":");
    Serial.print(HTTP_PORT);
    Serial.println("\"");
    Serial.println("------------------------------------------------------------------");

    return true;
}

void NetworkManager::CheckStatus()
{
    wl_status_t status = WiFi.status();
    if (status == WL_CONNECTION_LOST || status == WL_DISCONNECTED)
    {
        WiFi.reconnect();
    }
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