#include <Arduino.h>
#include <Dezibot.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <shared/SenderMap.h>

#define ESPNOW_CHANNEL 1

Dezibot dezibot;

static std::map<String, SenderInfo> senderMap;
static SemaphoreHandle_t senderMapMutex = xSemaphoreCreateMutex();

std::map<String, SenderInfo> &getSenderMap() { return senderMap; }
SemaphoreHandle_t getSenderMapMutex() { return senderMapMutex; }

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

    Serial.printf("ESP-NOW from %s: counter=%lu uptime=%lu\n",
                  macStr, (unsigned long)msg.counter, (unsigned long)msg.uptimeMs);
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
