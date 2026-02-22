#ifndef ESPNOW_RECEIVER_TRANSPORT_H
#define ESPNOW_RECEIVER_TRANSPORT_H

#include "ReceiverTransport.h"

class EspNowReceiverTransport : public ReceiverTransport
{
public:
    bool begin() override;
    bool sendCommand(const uint8_t *mac, uint8_t command) override;

private:
    static EspNowReceiverTransport *instance;
    static void onRecv(const uint8_t *mac, const uint8_t *data, int dataLen);
};

#endif
