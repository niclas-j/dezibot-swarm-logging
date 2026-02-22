#include <Arduino.h>
#include <Dezibot.h>
#include <WiFi.h>
#include <shared/SenderMap.h>
#include <shared/CommandMessage.h>
#include <shared/CommandSender.h>
#include <logger/Logger.h>
#include <transport/EspNowReceiverTransport.h>
#include <transport/BleReceiverTransport.h>

Dezibot dezibot;

static std::map<String, SenderInfo> senderMap;
static SemaphoreHandle_t senderMapMutex = xSemaphoreCreateMutex();

std::map<String, SenderInfo> &getSenderMap() { return senderMap; }
SemaphoreHandle_t getSenderMapMutex() { return senderMapMutex; }

static EspNowReceiverTransport espNowTransport;
static BleReceiverTransport bleTransport;

static void storeTelemetry(const uint8_t *mac, const SensorMessage &msg, TransportType transport)
{
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    if (xSemaphoreTake(senderMapMutex, pdMS_TO_TICKS(10)) == pdTRUE)
    {
        SenderInfo &info = senderMap[String(macStr)];
        info.msg = msg;
        info.lastSeenMs = millis();
        info.transport = transport;
        xSemaphoreGive(senderMapMutex);
    }

    char logBuf[128];
    snprintf(logBuf, sizeof(logBuf), "Telemetry from %s [%s]: counter=%lu uptime=%lu",
             macStr, transport == TRANSPORT_BLE ? "BLE" : "ESP-NOW",
             (unsigned long)msg.counter, (unsigned long)msg.uptimeMs);
    Logger::getInstance().logInfo(std::string(logBuf));
}

static void onEspNowTelemetry(const uint8_t *mac, const SensorMessage &msg)
{
    storeTelemetry(mac, msg, TRANSPORT_ESPNOW);
}

static void onBleTelemetry(const uint8_t *mac, const SensorMessage &msg)
{
    storeTelemetry(mac, msg, TRANSPORT_BLE);
}

bool sendCommandToDevice(const uint8_t *mac, uint8_t command)
{
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    TransportType transport = TRANSPORT_ESPNOW;
    if (xSemaphoreTake(senderMapMutex, pdMS_TO_TICKS(10)) == pdTRUE)
    {
        auto it = senderMap.find(String(macStr));
        if (it != senderMap.end())
            transport = it->second.transport;
        xSemaphoreGive(senderMapMutex);
    }

    Serial.printf("sendCommand: %s via %s\n", macStr, transport == TRANSPORT_BLE ? "BLE" : "ESP-NOW");

    if (transport == TRANSPORT_BLE)
        return bleTransport.sendCommand(mac, command);
    else
        return espNowTransport.sendCommand(mac, command);
}

int i = 0;

void setup()
{
    dezibot.begin();
    dezibot.debugServer.setup();

    espNowTransport.setTelemetryCallback(onEspNowTelemetry);
    espNowTransport.begin();

    bleTransport.setTelemetryCallback(onBleTelemetry);
    bleTransport.begin();

    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
}

void loop()
{
    Serial.println(i);
    i++;
    delay(1000);
}
