#ifndef NETWORKMANAGER_H
#define NETWROKMANAGER_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>

#define PORT 10200

#define ATTEMPT_DURATION 30000

#define DEBUG_WIFI_SETTINGS

typedef std::function<void(String)> OnNewMessageHandler;
typedef std::function<void(int)> OnNewClientHandler;
typedef std::function<void()> OnConnectionLostHandler;

class NetworkManager
{
private:
    AsyncWebServer server = AsyncWebServer(PORT);
    AsyncWebSocket webSocket = AsyncWebSocket("/ws");

    String buffer;
    uint64_t buffer_size;

public:
    void handleWebSocketMessage(void *, uint8_t *, size_t);

    OnNewMessageHandler onNewMessageHandler;
    OnNewClientHandler onNewClientHandler;
    OnConnectionLostHandler onConnectionLostHandler;

    void OnNewMessage(OnNewMessageHandler);
    void OnNewClient(OnNewClientHandler);
    void OnConnectionLost(OnConnectionLostHandler);
    void AddWebPageHandler(String uri, ArRequestHandlerFunction function);
    void AddWebPageHandler(const char *uri, ArRequestHandlerFunction function);
    void CleanUp();
    void CheckStatus();
    void SentTextToClient(int, const char *);
    void SentTextToAll(const char *);

    static NetworkManager *Instance;

    bool Begin(const char *, const char *);
};

#endif