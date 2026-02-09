#include "SwarmPage.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <shared/SenderMap.h>

SwarmPage::SwarmPage(WebServer *server) : serverPointer(server)
{
    server->on("/getSwarmData", [this]()
               { getSwarmData(); });
}

void SwarmPage::handler()
{
    serveFileFromSpiffs(serverPointer, "/index.html", "text/html");
}

void SwarmPage::getSwarmData()
{
    JsonDocument jsonDoc;
    JsonArray arr = jsonDoc.to<JsonArray>();

    auto &senderMap = getSenderMap();
    SemaphoreHandle_t mutex = getSenderMapMutex();

    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        unsigned long now = millis();
        for (auto &entry : senderMap)
        {
            JsonObject obj = arr.add<JsonObject>();
            obj["mac"] = entry.first;
            obj["counter"] = entry.second.msg.counter;
            obj["uptime"] = entry.second.msg.uptimeMs;
            obj["lastSeen"] = now - entry.second.lastSeenMs;
            obj["online"] = (now - entry.second.lastSeenMs) < 5000;
        }
        xSemaphoreGive(mutex);
    }

    String response;
    serializeJson(jsonDoc, response);
    serverPointer->send(200, "application/json", response);
}
