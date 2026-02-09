#include "LiveDataPage.h"
#include <Dezibot.h>
#include <ArduinoJson.h>
#include <logger/Logger.h>
#include <shared/SenderMap.h>

extern Dezibot dezibot;

LiveDataPage::LiveDataPage(WebServer *server) : serverPointer(server)
{
    server->on("/getEnabledSensorValues", [this]()
               { getEnabledSensorValues(); });
}

void LiveDataPage::handler()
{
    serveFileFromSpiffs(serverPointer, "/index.html", "text/html");
}

static void addSensorJson(JsonArray &arr, const char *name, const String &value)
{
    JsonObject obj = arr.add<JsonObject>();
    obj["name"] = name;
    obj["value"] = value;
}

void LiveDataPage::getRemoteSensorValues(const String &mac)
{
    JsonDocument jsonDoc;
    JsonArray sensorArray = jsonDoc.to<JsonArray>();

    auto &senderMap = getSenderMap();
    SemaphoreHandle_t mutex = getSenderMapMutex();

    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        auto it = senderMap.find(mac);
        if (it != senderMap.end())
        {
            const SensorMessage &m = it->second.msg;

            addSensorJson(sensorArray, "getAmbientLight()",
                          String(m.ambientLight));
            addSensorJson(sensorArray, "getRGB()",
                          "blue: " + String(m.colorB) + ", red: " + String(m.colorR) + ", green: " + String(m.colorG));
            addSensorJson(sensorArray, "getColorValue(RED)", String(m.colorR));
            addSensorJson(sensorArray, "getColorValue(GREEN)", String(m.colorG));
            addSensorJson(sensorArray, "getColorValue(BLUE)", String(m.colorB));
            addSensorJson(sensorArray, "getColorValue(WHITE)", String(m.colorW));

            addSensorJson(sensorArray, "getValue(IR_FRONT)", String(m.irFront));
            addSensorJson(sensorArray, "getValue(IR_LEFT)", String(m.irLeft));
            addSensorJson(sensorArray, "getValue(IR_RIGHT)", String(m.irRight));
            addSensorJson(sensorArray, "getValue(IR_BACK)", String(m.irBack));
            addSensorJson(sensorArray, "getValue(DL_BOTTOM)", String(m.dlBottom));
            addSensorJson(sensorArray, "getValue(DL_FRONT)", String(m.dlFront));

            addSensorJson(sensorArray, "left.getSpeed()", String(m.motorLeft));
            addSensorJson(sensorArray, "right.getSpeed()", String(m.motorRight));

            addSensorJson(sensorArray, "getAcceleration()",
                          "x: " + String(m.accelX) + ", y: " + String(m.accelY) + ", z: " + String(m.accelZ));
            addSensorJson(sensorArray, "getRotation()",
                          "x: " + String(m.gyroX) + ", y: " + String(m.gyroY) + ", z: " + String(m.gyroZ));
            addSensorJson(sensorArray, "getTemperature()", String(m.temperature));
            addSensorJson(sensorArray, "getWhoAmI()", String(m.whoAmI));
            addSensorJson(sensorArray, "getTilt()",
                          "x: " + String(m.tiltX) + ", y: " + String(m.tiltY));
            addSensorJson(sensorArray, "getTiltDirection()", String(m.tiltDirection));

            addSensorJson(sensorArray, "freeHeap", String(m.freeHeap));
            addSensorJson(sensorArray, "minFreeHeap", String(m.minFreeHeap));
            addSensorJson(sensorArray, "taskCount", String(m.taskCount));
            addSensorJson(sensorArray, "chipTemp", String(m.chipTemp));
            addSensorJson(sensorArray, "estimatedPower (mW)", String(m.estimatedPowerMw));
        }
        xSemaphoreGive(mutex);
    }

    String jsonResponse;
    serializeJson(jsonDoc, jsonResponse);
    serverPointer->send(200, "application/json", jsonResponse);
}

void LiveDataPage::getEnabledSensorValues()
{
    if (serverPointer->hasArg("mac"))
    {
        getRemoteSensorValues(serverPointer->arg("mac"));
        return;
    }

    JsonDocument jsonDoc;
    JsonArray sensorArray = jsonDoc.to<JsonArray>();

    Logger::getInstance().setLoggingEnabled(false);

    auto &sensors = dezibot.debugServer.getSensors();
    for (auto &sensor : sensors)
    {
        for (auto &sensorFunction : sensor.getSensorFunctions())
        {
            if (sensorFunction.getSensorState())
            {
                JsonObject sensorJson = sensorArray.add<JsonObject>();
                sensorJson["name"] = sensorFunction.getFunctionName();
                sensorJson["value"] = sensorFunction.getStringValue();
            }
        }
    }

    Logger::getInstance().setLoggingEnabled(true);

    String jsonResponse;
    serializeJson(jsonDoc, jsonResponse);
    serverPointer->send(200, "application/json", jsonResponse);
}
