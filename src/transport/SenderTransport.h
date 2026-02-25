/**
 * @file SenderTransport.h
 * @author Niclas Jost, Marius Busalt
 * @brief Abstract base class for sender transport implementations.
 *        Defines the interface for sending telemetry data and receiving commands.
 * @version 1.0
 * @date 2026-02
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef SENDER_TRANSPORT_H
#define SENDER_TRANSPORT_H

#include <functional>
#include <shared/SensorMessage.h>
#include <shared/CommandMessage.h>

using CommandCallback = std::function<void(const CommandMessage &cmd)>;

/**
 * @class SenderTransport
 * @brief Abstract base class for all sender transport implementations.
 */
class SenderTransport
{
public:
    /**
     * @brief Virtual destructor for proper cleanup of derived classes.
     */
    virtual ~SenderTransport() = default;

    /**
     * @brief Initialize the transport layer.
     * @return true if initialization successful, false otherwise.
     */
    virtual bool begin() = 0;

    /**
     * @brief Send telemetry data to the receiver.
     * @param msg The sensor message containing telemetry data.
     * @return true if send successful, false otherwise.
     */
    virtual bool sendTelemetry(const SensorMessage &msg) = 0;

    /**
     * @brief Set the callback function for incoming commands.
     * @param cb Callback function to handle received commands.
     * @return void
     */
    void setCommandCallback(CommandCallback cb) { commandCallback = cb; }

protected:
    /**
     * @brief Callback function for incoming commands.
     */
    CommandCallback commandCallback;
};

#endif
