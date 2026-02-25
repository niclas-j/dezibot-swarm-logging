#include "EspNowSenderTransport.h"
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#define ESPNOW_CHANNEL 1

EspNowSenderTransport *EspNowSenderTransport::instance = nullptr;

void EspNowSenderTransport::onSent(const uint8_t *macAddr, esp_now_send_status_t status)
{
    Serial.print("Send status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

void EspNowSenderTransport::onRecv(const uint8_t *mac, const uint8_t *data, int dataLen)
{
    if (dataLen != sizeof(CommandMessage))
        return;

    CommandMessage cmd;
    memcpy(&cmd, data, sizeof(cmd));

    if (cmd.magic != CMD_MAGIC)
        return;

    if (instance && instance->commandCallback)
        instance->commandCallback(cmd);
}

bool EspNowSenderTransport::begin()
{
    instance = this;

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW init failed");
        return false;
    }

    esp_now_register_send_cb(onSent);
    esp_now_register_recv_cb(onRecv);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = ESPNOW_CHANNEL;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to add broadcast peer");
        return false;
    }

    Serial.println("ESP-NOW sender transport ready");
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
    return true;
}

bool EspNowSenderTransport::sendTelemetry(const SensorMessage &msg)
{
    esp_err_t result = esp_now_send(broadcastAddress, (const uint8_t *)&msg, sizeof(msg));
    return result == ESP_OK;
}
