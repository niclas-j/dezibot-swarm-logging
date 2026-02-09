#ifndef SENDER_MAP_H
#define SENDER_MAP_H

#include <Arduino.h>
#include <map>
#include <shared/SensorMessage.h>

struct SenderInfo
{
    SensorMessage msg;
    unsigned long lastSeenMs;
};

std::map<String, SenderInfo> &getSenderMap();
SemaphoreHandle_t getSenderMapMutex();

#endif
