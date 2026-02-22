#ifndef ESPNOW_SENDER_TRANSPORT_H
#define ESPNOW_SENDER_TRANSPORT_H

#include "SenderTransport.h"
#include <esp_now.h>

class EspNowSenderTransport : public SenderTransport
{
public:
    bool begin() override;
    bool sendTelemetry(const SensorMessage &msg) override;

private:
    static EspNowSenderTransport *instance;
    static void onSent(const uint8_t *macAddr, esp_now_send_status_t status);
    static void onRecv(const uint8_t *mac, const uint8_t *data, int dataLen);

    uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
};

#endif
