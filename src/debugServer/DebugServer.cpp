/**
 * @file DebugServer.cpp
 * @author Tim Dietrich, Felix Herrling
 * @brief Implementation of the DebugServer class.
 * @version 1.0
 * @date 2025-03-23
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "DebugServer.h"
#include <Dezibot.h>

#include "PageProvider.h"
#include "MainPage.h"
#include <WebServer.h>
#include <WiFi.h>
#include <SPIFFS.h>

WebServer server;
extern Dezibot dezibot;

DebugServer::DebugServer() : server(80)
{
    mainPage = new MainPage(&server);
    loggingPage = new LoggingPage(&server);
    settingsPage = new SettingsPage(&server);
    liveDataPage = new LiveDataPage(&server);
    swarmPage = new SwarmPage(&server);
}

void DebugServer::setup()
{
    SPIFFS.begin();

    const char *SSID = "Debug-Server";
    const char *PSK = "PW4studProj";

    const IPAddress local_ip(192, 168, 1, 1);
    const IPAddress gateway(192, 168, 1, 1);
    const IPAddress subnet(255, 255, 255, 0);

    // setup as wi-fi access point
    WiFi.softAP(SSID, PSK);
    WiFi.softAPConfig(local_ip, gateway, subnet);

    Serial.print("Debug server AP started. Connect to '");
    Serial.println(WiFi.softAPIP());

    server.on("/", [this]
              { mainPage->handler(); });

    server.onNotFound([this]
                      {
        String uri = server.uri();
        if (uri.startsWith("/assets/"))
        {
            String contentType = "application/octet-stream";
            if (uri.endsWith(".js"))
                contentType = "application/javascript";
            else if (uri.endsWith(".css"))
                contentType = "text/css";
            PageProvider::serveFileFromSpiffs(&server, uri.c_str(), contentType.c_str());
        }
        else
        {
            mainPage->handler();
        } });

    // initialize color sensor
    Sensor colorSensor("Color Sensor", "ColorDetection");
    SensorFunction getAmbientLight("getAmbientLight()",
                                   []
                                   { return std::to_string(dezibot.colorDetection.getAmbientLight()); });
    SensorFunction getRGB("getRGB()",
                          []
                          { const uint16_t red = dezibot.colorDetection.getColorValue(VEML_RED);
            const uint16_t green = dezibot.colorDetection.getColorValue(VEML_GREEN);
            const uint16_t blue = dezibot.colorDetection.getColorValue(VEML_BLUE);
            // changed order of colour values to match graph colour in liveDataPage
            return "blue: " + std::to_string(blue) + ", red: " + std::to_string(red) +
                ", green: " + std::to_string(green); });
    SensorFunction getColorValueRed("getColorValue(RED)",
                                    []
                                    { return std::to_string(dezibot.colorDetection.getColorValue(VEML_RED)); });
    SensorFunction getColorValueGreen("getColorValue(GREEN)",
                                      []
                                      { return std::to_string(dezibot.colorDetection.getColorValue(VEML_GREEN)); });
    SensorFunction getColorValueBlue("getColorValue(BLUE)",
                                     []
                                     { return std::to_string(dezibot.colorDetection.getColorValue(VEML_BLUE)); });
    SensorFunction getColorValueWhite("getColorValue(WHITE)",
                                      []
                                      { return std::to_string(dezibot.colorDetection.getColorValue(VEML_WHITE)); });
    colorSensor.addFunction(getAmbientLight);
    colorSensor.addFunction(getRGB);
    colorSensor.addFunction(getColorValueRed);
    colorSensor.addFunction(getColorValueGreen);
    colorSensor.addFunction(getColorValueBlue);
    colorSensor.addFunction(getColorValueWhite);
    addSensor(colorSensor);

    // initialize light sensor
    Sensor lightSensor("Light Sensor", "LightDetection");
    SensorFunction getValueIrFront("getValue(IR_FRONT)",
                                   []
                                   { return std::to_string(LightDetection::getValue(IR_FRONT)); });
    SensorFunction getValueIrLeft("getValue(IR_LEFT)",
                                  []
                                  { return std::to_string(LightDetection::getValue(IR_LEFT)); });
    SensorFunction getValueIrRight("getValue(IR_RIGHT)",
                                   []
                                   { return std::to_string(LightDetection::getValue(IR_RIGHT)); });
    SensorFunction getValueIrBack("getValue(IR_BACK)",
                                  []
                                  { return std::to_string(LightDetection::getValue(IR_BACK)); });
    SensorFunction getValueDlBottom("getValue(DL_BOTTOM)",
                                    []
                                    { return std::to_string(LightDetection::getValue(DL_BOTTOM)); });
    SensorFunction getValueDlFront("getValue(DL_FRONT)",
                                   []
                                   { return std::to_string(LightDetection::getValue(DL_FRONT)); });
    lightSensor.addFunction(getValueIrFront);
    lightSensor.addFunction(getValueIrLeft);
    lightSensor.addFunction(getValueIrRight);
    lightSensor.addFunction(getValueIrBack);
    lightSensor.addFunction(getValueDlBottom);
    lightSensor.addFunction(getValueDlFront);
    addSensor(lightSensor);

    // initialize motor
    Sensor motor("Motor", "Motion");
    SensorFunction getSpeedLeft("left.getSpeed()",
                                []
                                { return std::to_string(Motion::left.getSpeed()); });
    SensorFunction getSpeedRight("right.getSpeed()",
                                 []
                                 { return std::to_string(Motion::right.getSpeed()); });
    motor.addFunction(getSpeedLeft);
    motor.addFunction(getSpeedRight);
    addSensor(motor);

    // initialize motion sensor
    Sensor motionSensor("Motion Sensor", "MotionDetection");
    SensorFunction getAcceleration("getAcceleration()",
                                   []
                                   {
                                       IMUResult result = Motion::detection.getAcceleration();
                                       return "x: " + std::to_string(result.x) + ", y: " + std::to_string(result.y) + ", z: " +
                                              std::to_string(result.z);
                                   });
    SensorFunction getRotation("getRotation()",
                               []
                               { IMUResult result = Motion::detection.getRotation();
            return "x: " + std::to_string(result.x) + ", y: " + std::to_string(result.y) + ", z: " +
                std::to_string(result.z); });
    SensorFunction getTemperature("getTemperature()",
                                  []
                                  { return std::to_string(Motion::detection.getTemperature()); });
    SensorFunction getWhoAmI("getWhoAmI()",
                             []
                             { return std::to_string(Motion::detection.getWhoAmI()); });
    SensorFunction getTilt("getTilt()",
                           []
                           {
                               Orientation result = Motion::detection.getTilt();
                               // Because an INT_MAX makes the graph practically unusable, instead send 0
                               if (result.xRotation == INT_MAX && result.yRotation == INT_MAX)
                               {
                                   result.xRotation = 0;
                                   result.yRotation = 0;
                               }
                               return "x: " + std::to_string(result.xRotation) + ", y: " + std::to_string(result.yRotation);
                           });
    SensorFunction getTiltDirection("getTiltDirection()",
                                    []
                                    { Direction result = Motion::detection.getTiltDirection();
            return std::to_string(result); });
    motionSensor.addFunction(getAcceleration);
    motionSensor.addFunction(getRotation);
    motionSensor.addFunction(getTemperature);
    motionSensor.addFunction(getWhoAmI);
    motionSensor.addFunction(getTilt);
    motionSensor.addFunction(getTiltDirection);
    addSensor(motionSensor);

    // start webserver and run client handler on separate thread
    server.begin();
    beginClientHandle();
};

void DebugServer::addSensor(const Sensor &sensor)
{
    sensors.push_back(sensor);
}

std::vector<Sensor> &DebugServer::getSensors()
{
    return sensors;
}

// create a FreeRTOS task to handle client requests
void DebugServer::beginClientHandle()
{
    xTaskCreatePinnedToCore(
        handleClientTask,
        "DebugServerTask",
        4096,
        this,
        11,
        &taskHandle,
        1);
}

// handle client requests
void DebugServer::handleClientTask(void *parameter)
{
    const auto debugServer = static_cast<DebugServer *>(parameter);
    TickType_t xLastWakeTime;

    // continuously handle client requests
    while (debugServer->serverActive)
    {
        constexpr TickType_t xFrequency = 25;
        debugServer->server.handleClient();
        xLastWakeTime = xTaskGetTickCount();
        xTaskDelayUntil(&xLastWakeTime, xFrequency);
    }

    // Delete task when server is no longer active
    vTaskDelete(taskHandle);
}
