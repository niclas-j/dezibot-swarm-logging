#ifndef BLE_RECEIVER_TRANSPORT_H
#define BLE_RECEIVER_TRANSPORT_H

#include "ReceiverTransport.h"
#include <BLEDevice.h>
#include <BLEClient.h>
#include <map>
#include <Arduino.h>

#define DEZIBOT_SERVICE_UUID "DE210001-0000-1000-8000-00805F9B34FB"
#define SENSOR_CHAR_UUID     "DE210002-0000-1000-8000-00805F9B34FB"
#define COMMAND_CHAR_UUID    "DE210003-0000-1000-8000-00805F9B34FB"

struct BleDeviceEntry
{
    BLEClient *client;
    BLERemoteCharacteristic *commandChar;
    uint8_t mac[6];
};

class BleReceiverTransport : public ReceiverTransport
{
public:
    bool begin() override;
    bool sendCommand(const uint8_t *mac, uint8_t command) override;

private:
    static BleReceiverTransport *instance;
    std::map<std::string, BleDeviceEntry> connectedDevices;
    SemaphoreHandle_t devicesMutex;
    bool scanning = false;

    static void scanTask(void *param);
    void connectToDevice(BLEAdvertisedDevice *device);
    static void notifyCallback(BLERemoteCharacteristic *pChar, uint8_t *pData, size_t length, bool isNotify);

    class ScanCallbacks : public BLEAdvertisedDeviceCallbacks
    {
        void onResult(BLEAdvertisedDevice advertisedDevice) override;
    };

    class ClientCallbacks : public BLEClientCallbacks
    {
        void onConnect(BLEClient *pClient) override;
        void onDisconnect(BLEClient *pClient) override;
    };
};

#endif
