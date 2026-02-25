/**
 * @file BleReceiverTransport.h
 * @author Niclas Jost, Marius Busalt
 * @brief BLE (Bluetooth Low Energy) based receiver transport implementation.
 *        Uses BLE GATT client to receive telemetry from senders and send commands.
 * @version 1.0
 * @date 2026-02
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef BLE_RECEIVER_TRANSPORT_H
#define BLE_RECEIVER_TRANSPORT_H

#include "ReceiverTransport.h"
#include <BLEDevice.h>
#include <BLEClient.h>
#include <map>
#include <Arduino.h>

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
 * @struct BleDeviceEntry
 * @brief Stores connection info and characteristics for a discovered BLE device.
 */
struct BleDeviceEntry
{
    /**
     * @brief Pointer to the BLE client for this device.
     */
    BLEClient *client;

    /**
     * @brief Pointer to the command characteristic.
     */
    BLERemoteCharacteristic *commandChar;

    /**
     * @brief MAC address of the device (6 bytes).
     */
    uint8_t mac[6];
};

/**
 * @class BleReceiverTransport
 * @brief BLE GATT client implementation of ReceiverTransport.
 *        Scans for and connects to Dezibot sender devices.
 */
class BleReceiverTransport : public ReceiverTransport
{
public:
    /**
     * @brief Initialize BLE transport layer and start scanning for devices.
     * @return true if initialization successful, false otherwise.
     */
    bool begin() override;

    /**
     * @brief Send a command to a specific connected device.
     * @param mac MAC address of the target device (6 bytes).
     * @param command Command byte to send.
     * @return true if send successful, false otherwise.
     */
    bool sendCommand(const uint8_t *mac, uint8_t command) override;

private:
    /**
     * @brief Singleton instance pointer for static callbacks.
     */
    static BleReceiverTransport *instance;

    /**
     * @brief Map of connected devices indexed by MAC address.
     */
    std::map<std::string, BleDeviceEntry> connectedDevices;

    /**
     * @brief Mutex for thread-safe access to connectedDevices.
     */
    SemaphoreHandle_t devicesMutex;

    /**
     * @brief Flag indicating if scanning is active.
     */
    bool scanning = false;

    /**
     * @brief Task function that performs BLE scanning.
     * @param param Pointer to task parameters.
     */
    static void scanTask(void *param);

    /**
     * @brief Connect to a discovered BLE device.
     * @param device Pointer to the advertised device to connect to.
     */
    void connectToDevice(BLEAdvertisedDevice *device);

    /**
     * @brief Callback for handling notifications from remote characteristics.
     * @param pChar Pointer to the characteristic that sent the notification.
     * @param pData Pointer to the notification data.
     * @param length Length of the notification data.
     * @param isNotify True if this is a notification (vs indication).
     */
    static void notifyCallback(BLERemoteCharacteristic *pChar, uint8_t *pData, size_t length, bool isNotify);

    /**
     * @class ScanCallbacks
     * @brief Callback handler for BLE scan events.
     */
    class ScanCallbacks : public BLEAdvertisedDeviceCallbacks
    {
        /**
         * @brief Called when a new device is discovered during scan.
         * @param advertisedDevice The discovered device.
         */
        void onResult(BLEAdvertisedDevice advertisedDevice) override;
    };

    /**
     * @class ClientCallbacks
     * @brief Callback handler for BLE client connection events.
     */
    class ClientCallbacks : public BLEClientCallbacks
    {
        /**
         * @brief Called when a client connects to a server.
         * @param pClient Pointer to the client.
         */
        void onConnect(BLEClient *pClient) override;

        /**
         * @brief Called when a client disconnects from a server.
         * @param pClient Pointer to the client.
         */
        void onDisconnect(BLEClient *pClient) override;
    };
};

#endif
