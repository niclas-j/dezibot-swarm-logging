#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#define ESPNOW_CHANNEL 1
#define MSG_MAGIC 0xDE21

typedef struct {
    uint16_t magic;
    uint32_t counter;
    uint32_t uptimeMs;
} __attribute__((packed)) SensorMessage;

static uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint32_t counter = 0;

void onSent(const uint8_t *macAddr, esp_now_send_status_t status) {
    Serial.print("Send status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }

    esp_now_register_send_cb(onSent);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = ESPNOW_CHANNEL;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add broadcast peer");
        return;
    }

    Serial.println("ESP-NOW sender ready");
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
}

void loop() {
    SensorMessage msg;
    msg.magic = MSG_MAGIC;
    msg.counter = counter;
    msg.uptimeMs = millis();

    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&msg, sizeof(msg));

    Serial.printf("Sent #%lu uptime=%lu result=%s\n",
                  (unsigned long)counter,
                  (unsigned long)msg.uptimeMs,
                  result == ESP_OK ? "OK" : "ERR");

    counter++;
    delay(1000);
}
