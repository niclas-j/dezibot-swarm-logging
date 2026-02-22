#ifndef BLE_SENDER_TRANSPORT_H
#define BLE_SENDER_TRANSPORT_H

#include "SenderTransport.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define DEZIBOT_SERVICE_UUID "DE210001-0000-1000-8000-00805F9B34FB"
#define SENSOR_CHAR_UUID     "DE210002-0000-1000-8000-00805F9B34FB"
#define COMMAND_CHAR_UUID    "DE210003-0000-1000-8000-00805F9B34FB"

class BleSenderTransport : public SenderTransport
{
public:
    bool begin() override;
    bool sendTelemetry(const SensorMessage &msg) override;

private:
    BLEServer *pServer = nullptr;
    BLECharacteristic *pSensorChar = nullptr;
    BLECharacteristic *pCommandChar = nullptr;
    bool deviceConnected = false;

    static BleSenderTransport *instance;

    class ServerCallbacks : public BLEServerCallbacks
    {
        void onConnect(BLEServer *pServer) override;
        void onDisconnect(BLEServer *pServer) override;
    };

    class CommandCharCallbacks : public BLECharacteristicCallbacks
    {
        void onWrite(BLECharacteristic *pCharacteristic) override;
    };
};

#endif
