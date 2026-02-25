/**
 * @file BleSenderTransport.h
 * @author Niclas Jost, Marius Busalt
 * @brief BLE (Bluetooth Low Energy) based sender transport implementation.
 *        Uses BLE GATT server to send telemetry data and receive commands.
 * @version 1.0
 * @date 2026-02
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef BLE_SENDER_TRANSPORT_H
#define BLE_SENDER_TRANSPORT_H

#include "SenderTransport.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>

/**
 * @brief Service UUID for Dezibot BLE communication.
 */
#define DEZIBOT_SERVICE_UUID "DE210001-0000-1000-8000-00805F9B34FB"
/**
 * @brief Characteristic UUID for sensor data.
 */
#define SENSOR_CHAR_UUID     "DE210002-0000-1000-8000-00805F9B34FB"
/**
 * @brief Characteristic UUID for command data.
 */
#define COMMAND_CHAR_UUID    "DE210003-0000-1000-8000-00805F9B34FB"

/**
 * @class BleSenderTransport
 * @brief BLE GATT server implementation of SenderTransport.
 */
class BleSenderTransport : public SenderTransport
{
public:
    /**
     * @brief Initialize BLE transport layer and start advertising.
     * @return true if initialization successful, false otherwise.
     */
    bool begin() override;

    /**
     * @brief Send telemetry data via BLE notification.
     * @param msg The sensor message containing telemetry data.
     * @return true if send successful, false otherwise.
     */
    bool sendTelemetry(const SensorMessage &msg) override;

private:
    /**
     * @brief Pointer to the BLE server instance.
     */
    BLEServer *pServer = nullptr;

    /**
     * @brief Pointer to the sensor characteristic.
     */
    BLECharacteristic *pSensorChar = nullptr;

    /**
     * @brief Pointer to the command characteristic.
     */
    BLECharacteristic *pCommandChar = nullptr;

    /**
     * @brief Flag indicating if a device is currently connected.
     */
    bool deviceConnected = false;

    /**
     * @brief Singleton instance pointer for static callbacks.
     */
    static BleSenderTransport *instance;

    /**
     * @class ServerCallbacks
     * @brief Callback handler for BLE server events.
     */
    class ServerCallbacks : public BLEServerCallbacks
    {
        /**
         * @brief Called when a client connects.
         * @param pServer Pointer to the BLE server.
         */
        void onConnect(BLEServer *pServer) override;

        /**
         * @brief Called when a client disconnects.
         * @param pServer Pointer to the BLE server.
         */
        void onDisconnect(BLEServer *pServer) override;
    };

    /**
     * @class CommandCharCallbacks
     * @brief Callback handler for command characteristic write events.
     */
    class CommandCharCallbacks : public BLECharacteristicCallbacks
    {
        /**
         * @brief Called when command data is written to the characteristic.
         * @param pCharacteristic Pointer to the characteristic.
         */
        void onWrite(BLECharacteristic *pCharacteristic) override;
    };
};

#endif
