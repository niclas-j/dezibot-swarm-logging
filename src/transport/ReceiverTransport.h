/**
 * @file ReceiverTransport.h
 * @author Niclas Jost, Marius Busalt
 * @brief Abstract base class for receiver transport implementations.
 *        Defines the interface for receiving telemetry data and sending commands.
 * @version 1.0
 * @date 2026-02
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef RECEIVER_TRANSPORT_H
#define RECEIVER_TRANSPORT_H

#include <functional>
#include <shared/SensorMessage.h>
#include <shared/CommandMessage.h>

using TelemetryCallback = std::function<void(const uint8_t *mac, const SensorMessage &msg)>;

/**
 * @class ReceiverTransport
 * @brief Abstract base class for all receiver transport implementations.
 */
class ReceiverTransport
{
public:
    /**
     * @brief Virtual destructor for proper cleanup of derived classes.
     */
    virtual ~ReceiverTransport() = default;

    /**
     * @brief Initialize the transport layer.
     * @return true if initialization successful, false otherwise.
     */
    virtual bool begin() = 0;

    /**
     * @brief Send a command to a specific device.
     * @param mac MAC address of the target device (6 bytes).
     * @param command Command byte to send.
     * @return true if send successful, false otherwise.
     */
    virtual bool sendCommand(const uint8_t *mac, uint8_t command) = 0;

    /**
     * @brief Set the callback function for incoming telemetry data.
     * @param cb Callback function to handle received telemetry.
     * @return void
     */
    void setTelemetryCallback(TelemetryCallback cb) { telemetryCallback = cb; }

protected:
    /**
     * @brief Callback function for incoming telemetry data.
     */
    TelemetryCallback telemetryCallback;
};

#endif
