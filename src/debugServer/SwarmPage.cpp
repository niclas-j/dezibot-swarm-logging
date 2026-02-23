#include "SwarmPage.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <shared/SenderMap.h>
#include <shared/CommandSender.h>
#include <shared/CommandMessage.h>

SwarmPage::SwarmPage(WebServer *server) : serverPointer(server)
{
    server->on("/getSwarmData", [this]()
               { getSwarmData(); });
    server->on("/command/locate", HTTP_POST, [this]()
               { locateDevice(); });
    server->on("/command/forward", HTTP_POST, [this]()
               { forwardDevice(); });
    server->on("/command/stop", HTTP_POST, [this]()
               { stopDevice(); });
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
            obj["powerMw"] = entry.second.msg.estimatedPowerMw;
        }
        xSemaphoreGive(mutex);
    }

    String response;
    serializeJson(jsonDoc, response);
    serverPointer->send(200, "application/json", response);
}

void SwarmPage::locateDevice()
{
    String mac = serverPointer->arg("mac");
    if (mac.length() == 0)
    {
        serverPointer->send(400, "application/json", "{\"error\":\"missing mac parameter\"}");
        return;
    }

    uint8_t macBytes[6];
    if (sscanf(mac.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
               &macBytes[0], &macBytes[1], &macBytes[2],
               &macBytes[3], &macBytes[4], &macBytes[5]) != 6)
    {
        serverPointer->send(400, "application/json", "{\"error\":\"invalid mac format\"}");
        return;
    }

    bool ok = sendCommandToDevice(macBytes, CMD_LOCATE);
    if (ok)
        serverPointer->send(200, "application/json", "{\"status\":\"sent\"}");
    else
        serverPointer->send(500, "application/json", "{\"error\":\"send failed\"}");
}

void SwarmPage::forwardDevice()
{
    sendCommand(CMD_FORWARD);
}

void SwarmPage::stopDevice()
{
    sendCommand(CMD_STOP);
}

void SwarmPage::sendCommand(uint8_t cmd)
{
    String mac = serverPointer->arg("mac");
    if (mac.length() == 0)
    {
        serverPointer->send(400, "application/json", "{\"error\":\"missing mac parameter\"}");
        return;
    }

    uint8_t macBytes[6];
    if (sscanf(mac.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
               &macBytes[0], &macBytes[1], &macBytes[2],
               &macBytes[3], &macBytes[4], &macBytes[5]) != 6)
    {
        serverPointer->send(400, "application/json", "{\"error\":\"invalid mac format\"}");
        return;
    }

    bool ok = sendCommandToDevice(macBytes, cmd);
    if (ok)
        serverPointer->send(200, "application/json", "{\"status\":\"sent\"}");
    else
        serverPointer->send(500, "application/json", "{\"error\":\"send failed\"}");
}
