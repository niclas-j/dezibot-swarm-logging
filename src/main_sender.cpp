#include <Arduino.h>
#include <Dezibot.h>
#include <esp_system.h>
#include <driver/temp_sensor.h>
#include <shared/SensorMessage.h>
#include <shared/CommandMessage.h>
#include <transport/SenderTransport.h>
#include <transport/EspNowSenderTransport.h>
#include <transport/BleSenderTransport.h>

#define TRANSPORT_PROTOCOL "bluetooth" // "esp_now" or "bluetooth"

Dezibot dezibot;

static SenderTransport *transport = nullptr;
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

        transport->sendTelemetry(msg);
        counter++;

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void setup()
{
    dezibot.begin();
    delay(1000);

    Serial.printf("Setup: protocol=%s, free heap=%u\n", TRANSPORT_PROTOCOL, esp_get_free_heap_size());

    if (strcmp(TRANSPORT_PROTOCOL, "bluetooth") == 0)
        transport = new BleSenderTransport();
    else
        transport = new EspNowSenderTransport();

    Serial.println("Setup: transport created, setting callback...");

    transport->setCommandCallback([](const CommandMessage &cmd)
                                  {
        switch (cmd.command)
        {
        case CMD_LOCATE:
            Serial.println("LOCATE command received, blinking LEDs");
            dezibot.multiColorLight.blink(5, 0x00006400, ALL, 500);
            break;
        default:
            Serial.printf("Unknown command: 0x%02X\n", cmd.command);
            break;
        } });

    Serial.println("Setup: calling transport->begin()...");

    if (!transport->begin())
    {
        Serial.println("Transport init failed");
        return;
    }

    Serial.printf("Setup: transport ready, free heap=%u\n", esp_get_free_heap_size());

    temp_sensor_config_t tempCfg = TSENS_CONFIG_DEFAULT();
    temp_sensor_set_config(tempCfg);
    temp_sensor_start();

    xTaskCreatePinnedToCore(telemetryTask, "telemetry", 4096, NULL, 5, NULL, 0);
    Serial.println("Setup: complete");
}

void loop()
{
    delay(2000);
}
