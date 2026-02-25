/**
 * @file EspNowReceiverTransport.h
 * @author Niclas Jost, Marius Busalt
 * @brief ESP-NOW based receiver transport implementation.
 *        Uses ESP-NOW protocol to receive telemetry data from senders and send commands.
 * @version 1.0
 * @date 2026-02
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef ESPNOW_RECEIVER_TRANSPORT_H
#define ESPNOW_RECEIVER_TRANSPORT_H

#include "ReceiverTransport.h"

/**
 * @class EspNowReceiverTransport
 * @brief ESP-NOW implementation of ReceiverTransport for receiving telemetry via ESP-NOW.
 */
class EspNowReceiverTransport : public ReceiverTransport
{
public:
    /**
     * @brief Initialize ESP-NOW transport layer in receiver mode.
     * @return true if initialization successful, false otherwise.
     */
    bool begin() override;

    /**
     * @brief Send a command to a specific sender via ESP-NOW.
     * @param mac MAC address of the target device (6 bytes).
     * @param command Command byte to send.
     * @return true if send successful, false otherwise.
     */
    bool sendCommand(const uint8_t *mac, uint8_t command) override;

private:
    /**
     * @brief Singleton instance pointer for static callbacks.
     */
    static EspNowReceiverTransport *instance;

    /**
     * @brief Callback invoked when data is received.
     * @param mac MAC address of the sender.
     * @param data Pointer to received data.
     * @param dataLen Length of received data.
     */
    static void onRecv(const uint8_t *mac, const uint8_t *data, int dataLen);
};

#endif
