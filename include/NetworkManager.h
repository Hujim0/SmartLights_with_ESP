#ifndef NETWORKMANAGER_H
#define NETWROKMANAGER_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <stdlib.h>

#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>

#define HTTP_PORT 10200

#define ATTEMPT_DURATION 30000

#define INDEX_HTML_PATH "/index.html"

typedef std::function<void(String)> OnNewMessageHandler;
typedef std::function<void(int)> OnNewClientHandler;

class NetworkManager
{
private:
    WiFiClient client = WiFiClient();
    AsyncWebServer server = AsyncWebServer(HTTP_PORT);
    AsyncWebSocket webSocket = AsyncWebSocket("/ws");

public:
    void handleWebSocketMessage(void *, uint8_t *, size_t);

    OnNewMessageHandler onNewMessageHandler;
    OnNewClientHandler onNewClientHandler;
    void OnNewMessage(OnNewMessageHandler);
    void OnNewClient(OnNewClientHandler);
    void CleanUp();
    void SentTextToClient(int, const char *);

    static NetworkManager *Instance;

    void Begin(const char *, const char *);
};

#endif