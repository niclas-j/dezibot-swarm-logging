#include "BleReceiverTransport.h"
#include <Arduino.h>
#include <shared/CommandMessage.h>

BleReceiverTransport *BleReceiverTransport::instance = nullptr;

static std::vector<BLEAdvertisedDevice *> pendingDevices;
static SemaphoreHandle_t pendingMutex = xSemaphoreCreateMutex();

void BleReceiverTransport::ScanCallbacks::onResult(BLEAdvertisedDevice advertisedDevice)
{
    if (!advertisedDevice.haveServiceUUID() ||
        !advertisedDevice.isAdvertisingService(BLEUUID(DEZIBOT_SERVICE_UUID)))
        return;

    Serial.printf("BLE: found dezibot: %s\n", advertisedDevice.getAddress().toString().c_str());

    BLEScan *pScan = BLEDevice::getScan();
    pScan->stop();

    if (xSemaphoreTake(pendingMutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        pendingDevices.push_back(new BLEAdvertisedDevice(advertisedDevice));
        xSemaphoreGive(pendingMutex);
    }
}

void BleReceiverTransport::ClientCallbacks::onConnect(BLEClient *pClient)
{
    Serial.printf("BLE: connected to %s\n", pClient->getPeerAddress().toString().c_str());
}

void BleReceiverTransport::ClientCallbacks::onDisconnect(BLEClient *pClient)
{
    std::string addr = pClient->getPeerAddress().toString();
    Serial.printf("BLE: disconnected from %s\n", addr.c_str());

    if (instance && xSemaphoreTake(instance->devicesMutex, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        instance->connectedDevices.erase(addr);
        xSemaphoreGive(instance->devicesMutex);
    }
}

static void macFromBleAddress(BLEAddress &addr, uint8_t *outMac)
{
    std::string str = addr.toString();
    sscanf(str.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &outMac[0], &outMac[1], &outMac[2], &outMac[3], &outMac[4], &outMac[5]);
}

void BleReceiverTransport::notifyCallback(BLERemoteCharacteristic *pChar,
                                           uint8_t *pData, size_t length, bool isNotify)
{
    Serial.printf("BLE: notify received, len=%d\n", length);

    if (length != sizeof(SensorMessage) || !instance)
    {
        Serial.printf("BLE: notify size mismatch (got %d, expected %d)\n", length, sizeof(SensorMessage));
        return;
    }

    SensorMessage msg;
    memcpy(&msg, pData, sizeof(msg));

    if (msg.magic != MSG_MAGIC)
    {
        Serial.printf("BLE: bad magic 0x%04X\n", msg.magic);
        return;
    }

    BLEClient *pClient = pChar->getRemoteService()->getClient();
    BLEAddress addr = pClient->getPeerAddress();

    uint8_t mac[6];
    macFromBleAddress(addr, mac);

    Serial.printf("BLE: telemetry from %02X:%02X:%02X:%02X:%02X:%02X counter=%lu\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
                  (unsigned long)msg.counter);

    if (instance->telemetryCallback)
        instance->telemetryCallback(mac, msg);
}

void BleReceiverTransport::connectToDevice(BLEAdvertisedDevice *device)
{
    std::string addr = device->getAddress().toString();

    if (xSemaphoreTake(devicesMutex, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        if (connectedDevices.count(addr))
        {
            xSemaphoreGive(devicesMutex);
            return;
        }
        xSemaphoreGive(devicesMutex);
    }

    BLEClient *pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new ClientCallbacks());

    if (!pClient->connect(device))
    {
        Serial.printf("BLE: failed to connect to %s\n", addr.c_str());
        delete pClient;
        return;
    }

    // Request larger MTU — SensorMessage is 79 bytes, need at least 82 (79 + 3 ATT header)
    pClient->setMTU(100);
    Serial.printf("BLE: negotiated MTU with %s\n", addr.c_str());

    BLERemoteService *pService = pClient->getService(DEZIBOT_SERVICE_UUID);
    if (!pService)
    {
        Serial.printf("BLE: service not found on %s\n", addr.c_str());
        pClient->disconnect();
        delete pClient;
        return;
    }

    BLERemoteCharacteristic *pSensorChar = pService->getCharacteristic(SENSOR_CHAR_UUID);
    if (!pSensorChar)
    {
        Serial.printf("BLE: sensor characteristic not found on %s\n", addr.c_str());
        pClient->disconnect();
        delete pClient;
        return;
    }

    if (pSensorChar->canNotify())
    {
        Serial.printf("BLE: registering for notifications from %s\n", addr.c_str());
        pSensorChar->registerForNotify(notifyCallback);

        // Manually write CCCD to enable notifications in case registerForNotify didn't
        BLERemoteDescriptor *pDesc = pSensorChar->getDescriptor(BLEUUID((uint16_t)0x2902));
        if (pDesc)
        {
            uint8_t enableNotify[] = {0x01, 0x00};
            pDesc->writeValue(enableNotify, 2);
            Serial.println("BLE: CCCD written to enable notifications");
        }
        else
        {
            Serial.println("BLE: CCCD descriptor not found, notifications may not work");
        }
    }
    else
    {
        Serial.printf("BLE: sensor characteristic does not support notify on %s\n", addr.c_str());
    }

    BLERemoteCharacteristic *pCommandChar = pService->getCharacteristic(COMMAND_CHAR_UUID);

    BLEAddress bleAddr = device->getAddress();
    uint8_t mac[6];
    macFromBleAddress(bleAddr, mac);

    BleDeviceEntry entry;
    entry.client = pClient;
    entry.commandChar = pCommandChar;
    memcpy(entry.mac, mac, 6);

    if (xSemaphoreTake(devicesMutex, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        connectedDevices[addr] = entry;
        xSemaphoreGive(devicesMutex);
    }

    Serial.printf("BLE: subscribed to %s (MAC %02X:%02X:%02X:%02X:%02X:%02X)\n",
                  addr.c_str(), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void BleReceiverTransport::scanTask(void *param)
{
    BleReceiverTransport *self = (BleReceiverTransport *)param;

    while (true)
    {
        BLEScan *pScan = BLEDevice::getScan();
        pScan->setActiveScan(true);
        pScan->setInterval(100);
        pScan->setWindow(99);
        pScan->start(5, false);

        if (xSemaphoreTake(pendingMutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            std::vector<BLEAdvertisedDevice *> toConnect;
            toConnect.swap(pendingDevices);
            xSemaphoreGive(pendingMutex);

            for (auto *dev : toConnect)
            {
                self->connectToDevice(dev);
                delete dev;
            }
        }

        pScan->clearResults();
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

bool BleReceiverTransport::begin()
{
    instance = this;
    devicesMutex = xSemaphoreCreateMutex();

    BLEDevice::init("Dezibot_Receiver");

    BLEScan *pScan = BLEDevice::getScan();
    pScan->setAdvertisedDeviceCallbacks(new ScanCallbacks());

    xTaskCreatePinnedToCore(scanTask, "ble_scan", 8192, this, 3, NULL, 0);

    Serial.println("BLE receiver transport ready");
    return true;
}

bool BleReceiverTransport::sendCommand(const uint8_t *mac, uint8_t command)
{
    if (xSemaphoreTake(devicesMutex, pdMS_TO_TICKS(100)) != pdTRUE)
        return false;

    bool sent = false;
    for (auto &pair : connectedDevices)
    {
        BleDeviceEntry &entry = pair.second;
        if (memcmp(entry.mac, mac, 6) == 0 && entry.commandChar)
        {
            CommandMessage msg = {};
            msg.magic = CMD_MAGIC;
            msg.command = command;
            entry.commandChar->writeValue((uint8_t *)&msg, sizeof(msg));
            sent = true;
            break;
        }
    }

    xSemaphoreGive(devicesMutex);
    return sent;
}
