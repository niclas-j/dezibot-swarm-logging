/**
 * @file EspNowSenderTransport.h
 * @author Niclas Jost, Marius Busalt
 * @brief ESP-NOW based sender transport implementation.
 *        Uses ESP-NOW protocol to send telemetry data and receive commands.
 * @version 1.0
 * @date 2026-02
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef ESPNOW_SENDER_TRANSPORT_H
#define ESPNOW_SENDER_TRANSPORT_H

#include "SenderTransport.h"
#include <esp_now.h>

/**
 * @class EspNowSenderTransport
 * @brief ESP-NOW implementation of SenderTransport for sending telemetry via ESP-NOW.
 */
class EspNowSenderTransport : public SenderTransport
{
public:
    /**
     * @brief Initialize ESP-NOW transport layer.
     * @return true if initialization successful, false otherwise.
     */
    bool begin() override;

    /**
     * @brief Send telemetry data via ESP-NOW broadcast.
     * @param msg The sensor message containing telemetry data.
     * @return true if send successful, false otherwise.
     */
    bool sendTelemetry(const SensorMessage &msg) override;

private:
    /**
     * @brief Singleton instance pointer for static callbacks.
     */
    static EspNowSenderTransport *instance;

    /**
     * @brief Callback invoked when data is sent.
     * @param macAddr MAC address of the recipient.
     * @param status Send status (ESP_NOW_SEND_SUCCESS or ESP_NOW_SEND_FAIL).
     */
    static void onSent(const uint8_t *macAddr, esp_now_send_status_t status);

    /**
     * @brief Callback invoked when data is received.
     * @param mac MAC address of the sender.
     * @param data Pointer to received data.
     * @param dataLen Length of received data.
     */
    static void onRecv(const uint8_t *mac, const uint8_t *data, int dataLen);

    /**
     * @brief Broadcast MAC address (FF:FF:FF:FF:FF:FF).
     */
    uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
};

#endif
