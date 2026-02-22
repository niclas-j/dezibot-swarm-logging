#ifndef SENDER_TRANSPORT_H
#define SENDER_TRANSPORT_H

#include <functional>
#include <shared/SensorMessage.h>
#include <shared/CommandMessage.h>

using CommandCallback = std::function<void(const CommandMessage &cmd)>;

class SenderTransport
{
public:
    virtual ~SenderTransport() = default;
    virtual bool begin() = 0;
    virtual bool sendTelemetry(const SensorMessage &msg) = 0;

    void setCommandCallback(CommandCallback cb) { commandCallback = cb; }

protected:
    CommandCallback commandCallback;
};

#endif
