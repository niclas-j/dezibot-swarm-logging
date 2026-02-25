#include "EspNowReceiverTransport.h"
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

EspNowReceiverTransport *EspNowReceiverTransport::instance = nullptr;

void EspNowReceiverTransport::onRecv(const uint8_t *mac, const uint8_t *data, int dataLen)
{
    if (dataLen != sizeof(SensorMessage))
        return;

    SensorMessage msg;
    memcpy(&msg, data, sizeof(msg));

    if (msg.magic != MSG_MAGIC)
        return;

    if (instance && instance->telemetryCallback)
        instance->telemetryCallback(mac, msg);
}

bool EspNowReceiverTransport::begin()
{
    instance = this;

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW init failed");
        return false;
    }

    esp_now_register_recv_cb(onRecv);

    uint8_t primaryChan = 0;
    wifi_second_chan_t secondChan;
    esp_wifi_get_channel(&primaryChan, &secondChan);
    Serial.printf("ESP-NOW receiver transport ready on channel %d\n", primaryChan);
    return true;
}

bool EspNowReceiverTransport::sendCommand(const uint8_t *mac, uint8_t command)
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
    }

    CommandMessage msg = {};
    msg.magic = CMD_MAGIC;
    msg.command = command;

    esp_err_t result = esp_now_send(mac, (uint8_t *)&msg, sizeof(msg));
    return result == ESP_OK;
}
