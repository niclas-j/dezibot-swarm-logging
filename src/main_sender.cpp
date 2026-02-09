#include <Arduino.h>
#include <Dezibot.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_system.h>
#include <driver/temp_sensor.h>
#include <shared/SensorMessage.h>
#include <shared/CommandMessage.h>

#define ESPNOW_CHANNEL 1

Dezibot dezibot;

static uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint32_t counter = 0;

static uint16_t estimatePowerMw(const SensorMessage &m)
{
    const uint16_t PW_CPU_WIFI = 500;
    const uint16_t PW_ESPNOW_TX = 165;
    const uint16_t PW_IMU = 5;
    const uint16_t PW_COLOR_SENSE = 3;
    const uint16_t PW_DISPLAY = 50;

    uint16_t total = PW_CPU_WIFI + PW_ESPNOW_TX + PW_IMU + PW_COLOR_SENSE + PW_DISPLAY;

    total += (uint16_t)((uint32_t)m.motorLeft * 990 / 8192);
    total += (uint16_t)((uint32_t)m.motorRight * 990 / 8192);

    return total;
}

void onSent(const uint8_t *macAddr, esp_now_send_status_t status)
{
    Serial.print("Send status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

void onCommand(const uint8_t *mac, const uint8_t *data, int dataLen)
{
    if (dataLen != sizeof(CommandMessage))
        return;

    CommandMessage cmd;
    memcpy(&cmd, data, sizeof(cmd));

    if (cmd.magic != CMD_MAGIC)
        return;

    switch (cmd.command)
    {
    case CMD_LOCATE:
        Serial.println("LOCATE command received, blinking LEDs");
        dezibot.multiColorLight.blink(5, 0x00006400, ALL, 500);
        break;
    default:
        Serial.printf("Unknown command: 0x%02X\n", cmd.command);
        break;
    }
}

static void telemetryTask(void *param)
{
    while (true)
    {
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
        if (tilt.xRotation == INT_MAX && tilt.yRotation == INT_MAX)
        {
            tilt.xRotation = 0;
            tilt.yRotation = 0;
        }
        msg.tiltX = tilt.xRotation;
        msg.tiltY = tilt.yRotation;
        msg.tiltDirection = (uint8_t)Motion::detection.getTiltDirection();

        msg.freeHeap = esp_get_free_heap_size();
        msg.minFreeHeap = esp_get_minimum_free_heap_size();
        msg.taskCount = (uint8_t)uxTaskGetNumberOfTasks();
        float chipTemp = 0.0f;
        temp_sensor_read_celsius(&chipTemp);
        msg.chipTemp = chipTemp;

        msg.estimatedPowerMw = estimatePowerMw(msg);

        esp_now_send(broadcastAddress, (uint8_t *)&msg, sizeof(msg));
        counter++;

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void setup()
{
    dezibot.begin();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW init failed");
        return;
    }

    esp_now_register_send_cb(onSent);
    esp_now_register_recv_cb(onCommand);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = ESPNOW_CHANNEL;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to add broadcast peer");
        return;
    }

    temp_sensor_config_t tempCfg = TSENS_CONFIG_DEFAULT();
    temp_sensor_set_config(tempCfg);
    temp_sensor_start();

    xTaskCreatePinnedToCore(telemetryTask, "telemetry", 4096, NULL, 5, NULL, 0);

    Serial.println("ESP-NOW sender ready");
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
}

void loop()
{
    dezibot.motion.rotateClockwise(500);
    delay(2000);
    dezibot.motion.stop();
}
