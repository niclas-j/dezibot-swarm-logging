#include <Arduino.h>
#include <Dezibot.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <shared/SensorMessage.h>

#define ESPNOW_CHANNEL 1

Dezibot dezibot;

static uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint32_t counter = 0;

void onSent(const uint8_t *macAddr, esp_now_send_status_t status) {
    Serial.print("Send status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

void setup() {
    dezibot.begin();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }

    esp_now_register_send_cb(onSent);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = ESPNOW_CHANNEL;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add broadcast peer");
        return;
    }

    Serial.println("ESP-NOW sender ready");
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
}

void loop() {
    SensorMessage msg = {};
    msg.magic = MSG_MAGIC;
    msg.counter = counter;
    msg.uptimeMs = millis();

    msg.ambientLight = dezibot.colorDetection.getAmbientLight();
    msg.colorR = dezibot.colorDetection.getColorValue(VEML_RED);
    msg.colorG = dezibot.colorDetection.getColorValue(VEML_GREEN);
    msg.colorB = dezibot.colorDetection.getColorValue(VEML_BLUE);
    msg.colorW = dezibot.colorDetection.getColorValue(VEML_WHITE);

    msg.irFront = LightDetection::getValue(IR_FRONT);
    msg.irLeft = LightDetection::getValue(IR_LEFT);
    msg.irRight = LightDetection::getValue(IR_RIGHT);
    msg.irBack = LightDetection::getValue(IR_BACK);
    msg.dlBottom = LightDetection::getValue(DL_BOTTOM);
    msg.dlFront = LightDetection::getValue(DL_FRONT);

    msg.motorLeft = Motion::left.getSpeed();
    msg.motorRight = Motion::right.getSpeed();

    IMUResult accel = Motion::detection.getAcceleration();
    msg.accelX = accel.x;
    msg.accelY = accel.y;
    msg.accelZ = accel.z;

    IMUResult gyro = Motion::detection.getRotation();
    msg.gyroX = gyro.x;
    msg.gyroY = gyro.y;
    msg.gyroZ = gyro.z;

    msg.temperature = Motion::detection.getTemperature();
    msg.whoAmI = Motion::detection.getWhoAmI();

    Orientation tilt = Motion::detection.getTilt();
    if (tilt.xRotation == INT_MAX && tilt.yRotation == INT_MAX) {
        tilt.xRotation = 0;
        tilt.yRotation = 0;
    }
    msg.tiltX = tilt.xRotation;
    msg.tiltY = tilt.yRotation;
    msg.tiltDirection = (uint8_t)Motion::detection.getTiltDirection();

    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&msg, sizeof(msg));

    Serial.printf("Sent #%lu uptime=%lu size=%u result=%s\n",
                  (unsigned long)counter,
                  (unsigned long)msg.uptimeMs,
                  (unsigned)sizeof(msg),
                  result == ESP_OK ? "OK" : "ERR");

    counter++;
    delay(1000);
}
