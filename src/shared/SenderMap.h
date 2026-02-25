#ifndef SENDER_MAP_H
#define SENDER_MAP_H

#include <Arduino.h>
#include <map>
#include <shared/SensorMessage.h>

enum TransportType : uint8_t
{
    TRANSPORT_ESPNOW = 0,
    TRANSPORT_BLE = 1,
};

struct SenderInfo
{
    SensorMessage msg;
    unsigned long lastSeenMs;
    TransportType transport = TRANSPORT_ESPNOW;
};

std::map<String, SenderInfo> &getSenderMap();
SemaphoreHandle_t getSenderMapMutex();

#endif
