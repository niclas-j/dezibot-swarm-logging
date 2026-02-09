#include <Arduino.h>
#include <Dezibot.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <logger/Logger.h>

#define ESPNOW_CHANNEL 1
#define MSG_MAGIC 0xDE21

Dezibot dezibot;

typedef struct
{
    uint16_t magic;
    uint32_t counter;
    uint32_t uptimeMs;
} __attribute__((packed)) SensorMessage;

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

    Serial.printf("ESP-NOW from %s: counter=%lu uptime=%lu\n",
                  macStr, (unsigned long)msg.counter, (unsigned long)msg.uptimeMs);

    dezibot.display.clear();
    char displayBuf[64];
    snprintf(displayBuf, sizeof(displayBuf), "From: %s\n#%lu t=%lu",
             macStr, (unsigned long)msg.counter, (unsigned long)msg.uptimeMs);
    dezibot.display.print(displayBuf);
}

int i = 0;

void setup()
{
    dezibot.begin();
    dezibot.debugServer.setup();

    // if (esp_now_init() != ESP_OK)
    // {
    //     Serial.println("ESP-NOW init failed");
    //     return;
    // }

    // esp_now_register_recv_cb(onReceive);

    // uint8_t primaryChan = 0;
    // wifi_second_chan_t secondChan;
    // esp_wifi_get_channel(&primaryChan, &secondChan);
    // Serial.printf("ESP-NOW receiver ready on channel %d\n", primaryChan);
    // Serial.print("MAC: ");
    // Serial.println(WiFi.macAddress());
}

void loop()
{
    dezibot.colorDetection.getColorValue(VEML_RED);
    dezibot.display.println(i);
    Serial.println(i);
    i++;
    delay(1000);
}
