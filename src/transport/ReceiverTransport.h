#ifndef RECEIVER_TRANSPORT_H
#define RECEIVER_TRANSPORT_H

#include <functional>
#include <shared/SensorMessage.h>
#include <shared/CommandMessage.h>

using TelemetryCallback = std::function<void(const uint8_t *mac, const SensorMessage &msg)>;

class ReceiverTransport
{
public:
    virtual ~ReceiverTransport() = default;
    virtual bool begin() = 0;
    virtual bool sendCommand(const uint8_t *mac, uint8_t command) = 0;

    void setTelemetryCallback(TelemetryCallback cb) { telemetryCallback = cb; }

protected:
    TelemetryCallback telemetryCallback;
};

#endif
