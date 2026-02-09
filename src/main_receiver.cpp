#include <Arduino.h>
#include <Dezibot.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <shared/SenderMap.h>
#include <shared/CommandMessage.h>
#include <shared/CommandSender.h>
#include <logger/Logger.h>

#define ESPNOW_CHANNEL 1

Dezibot dezibot;

static std::map<String, SenderInfo> senderMap;
static SemaphoreHandle_t senderMapMutex = xSemaphoreCreateMutex();

std::map<String, SenderInfo> &getSenderMap() { return senderMap; }
SemaphoreHandle_t getSenderMapMutex() { return senderMapMutex; }

static bool parseMac(const char *str, uint8_t *out)
{
    return sscanf(str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                  &out[0], &out[1], &out[2], &out[3], &out[4], &out[5]) == 6;
}

bool sendCommandToDevice(const uint8_t *mac, uint8_t command)
{
    Serial.printf("sendCommand: to %02X:%02X:%02X:%02X:%02X:%02X cmd=0x%02X\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], command);

    if (!esp_now_is_peer_exist(mac))
    {
        esp_now_peer_info_t peer = {};
        memcpy(peer.peer_addr, mac, 6);
        peer.channel = 0;
        peer.encrypt = false;
        peer.ifidx = WIFI_IF_AP;
        esp_err_t addResult = esp_now_add_peer(&peer);
        if (addResult != ESP_OK)
        {
            Serial.printf("sendCommand: add_peer failed err=0x%x\n", addResult);
            return false;
        }
        Serial.println("sendCommand: peer added");
    }

    CommandMessage msg = {};
    msg.magic = CMD_MAGIC;
    msg.command = command;

    esp_err_t result = esp_now_send(mac, (uint8_t *)&msg, sizeof(msg));
    Serial.printf("sendCommand: send result=0x%x\n", result);
    return result == ESP_OK;
}

void onReceive(const uint8_t *mac, const uint8_t *data, int dataLen)
{
    if (dataLen != sizeof(SensorMessage))
        return;

    SensorMessage msg;
    memcpy(&msg, data, sizeof(msg));

    if (msg.magic != MSG_MAGIC)
        return;

    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    if (xSemaphoreTake(senderMapMutex, pdMS_TO_TICKS(10)) == pdTRUE)
    {
        SenderInfo &info = senderMap[String(macStr)];
        info.msg = msg;
        info.lastSeenMs = millis();
        xSemaphoreGive(senderMapMutex);
    }

    char logBuf[128];
    snprintf(logBuf, sizeof(logBuf), "ESP-NOW from %s: counter=%lu uptime=%lu",
             macStr, (unsigned long)msg.counter, (unsigned long)msg.uptimeMs);
    Logger::getInstance().logInfo(std::string(logBuf));
}

int i = 0;

void setup()
{
    dezibot.begin();
    dezibot.debugServer.setup();

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW init failed");
        return;
    }

    esp_now_register_recv_cb(onReceive);

    uint8_t primaryChan = 0;
    wifi_second_chan_t secondChan;
    esp_wifi_get_channel(&primaryChan, &secondChan);
    Serial.printf("ESP-NOW receiver ready on channel %d\n", primaryChan);
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
}

void loop()
{
    Serial.println(i);
    i++;
    delay(1000);
}
