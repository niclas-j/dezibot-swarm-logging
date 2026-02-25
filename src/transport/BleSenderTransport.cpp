#include "BleSenderTransport.h"
#include <Arduino.h>
#include <esp_mac.h>

BleSenderTransport *BleSenderTransport::instance = nullptr;

void BleSenderTransport::ServerCallbacks::onConnect(BLEServer *pServer)
{
    if (instance)
        instance->deviceConnected = true;
    Serial.println("BLE: receiver connected");
}

void BleSenderTransport::ServerCallbacks::onDisconnect(BLEServer *pServer)
{
    if (instance)
        instance->deviceConnected = false;
    Serial.println("BLE: receiver disconnected");
    pServer->startAdvertising();
}

void BleSenderTransport::CommandCharCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{
    std::string value = pCharacteristic->getValue();
    if (value.length() != sizeof(CommandMessage))
        return;

    CommandMessage cmd;
    memcpy(&cmd, value.data(), sizeof(cmd));

    if (cmd.magic != CMD_MAGIC)
        return;

    if (instance && instance->commandCallback)
        instance->commandCallback(cmd);
}

bool BleSenderTransport::begin()
{
    instance = this;

    Serial.println("BLE: reading MAC...");
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);
    char nameBuf[20];
    snprintf(nameBuf, sizeof(nameBuf), "Dezibot_%02X%02X", mac[4], mac[5]);

    Serial.print("BLE: initializing as ");
    Serial.println(nameBuf);
    BLEDevice::init(nameBuf);

    Serial.println("BLE: creating server...");
    pServer = BLEDevice::createServer();
    if (!pServer)
    {
        Serial.println("BLE: createServer failed");
        return false;
    }
    pServer->setCallbacks(new ServerCallbacks());

    Serial.println("BLE: creating service...");
    BLEService *pService = pServer->createService(DEZIBOT_SERVICE_UUID);
    if (!pService)
    {
        Serial.println("BLE: createService failed");
        return false;
    }

    Serial.println("BLE: creating characteristics...");
    pSensorChar = pService->createCharacteristic(
        SENSOR_CHAR_UUID,
        BLECharacteristic::PROPERTY_NOTIFY);
    pSensorChar->addDescriptor(new BLE2902());

    pCommandChar = pService->createCharacteristic(
        COMMAND_CHAR_UUID,
        BLECharacteristic::PROPERTY_WRITE);
    pCommandChar->setCallbacks(new CommandCharCallbacks());

    Serial.println("BLE: starting service...");
    pService->start();

    BLEDevice::setMTU(100);

    Serial.println("BLE: starting advertising...");
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(DEZIBOT_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    BLEDevice::startAdvertising();

    Serial.println("BLE sender transport ready");
    return true;
}

bool BleSenderTransport::sendTelemetry(const SensorMessage &msg)
{
    if (!deviceConnected)
    {
        Serial.println("BLE: not connected, skipping telemetry");
        return false;
    }

    pSensorChar->setValue((uint8_t *)&msg, sizeof(msg));
    pSensorChar->notify();
    Serial.printf("BLE: notified %d bytes, counter=%lu\n", sizeof(msg), (unsigned long)msg.counter);
    return true;
}
