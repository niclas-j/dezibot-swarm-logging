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
#include "WifiCredentials.h"
#include <WebServer.h>
#include <WiFi.h>
#include <SPIFFS.h>

WebServer server;
extern Dezibot dezibot;

DebugServer::DebugServer():server(80) {
    mainPage = new MainPage(&server);
    loggingPage = new LoggingPage(&server);
    settingsPage = new SettingsPage(&server);
    liveDataPage = new LiveDataPage(&server);
}

void DebugServer::setup() {
    // initialize SPIFFS for file access
    // changes in html files require "pio run -t uploadfs" or "Upload Filesystem Image" in plugin to take effect
    SPIFFS.begin();

    // connect to existing WiFi network (Station mode)
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    Serial.print("Connecting to WiFi");
    const unsigned long timeout = 20000;
    const unsigned long startAttempt = millis();
    
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < timeout) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi connection failed! Debug server will not be available.");
        return;
    }

    // set uri and handler for each page
    // Main Page
    server.on("/", [this] {
        mainPage->handler();
    });

    server.on("/css/mainPageStyle.css", [this] {
       mainPage->cssHandler();
    });

    // Logging
    server.on("/logging", [this] {
        loggingPage->handler();
    });

    server.on("/css/loggingPageStyle.css", [this] {
        loggingPage->cssHandler();
    });

    server.on("/js/loggingPageScript.js", [this] {
        loggingPage->jsHandler();
    });

    // Live Data
    server.on("/livedata", [this] {
        liveDataPage->handler();
    });

    server.on("/lib/canvasjs.min.js", [this] {
        liveDataPage->canvasjsHandler();
    });

    server.on("/js/liveDataPageScript.js", [this] {
        liveDataPage->jsHandler();
    });

    server.on("/css/liveDataPageStyle.css", [this] {
       liveDataPage->cssHandler();
    });

    // Settings
    server.on("/settings", [this] {
        settingsPage->handler();
    });

    server.on("/js/settingsPageScript.js", [this] {
        settingsPage->jsHandler();
    });

    server.on("/css/settingsPageStyle.css", [this] {
       settingsPage->cssHandler();
    });

    server.onNotFound([this] {
        mainPage->errorPageHandler();
    });

    // initialize color sensor
    Sensor colorSensor("Color Sensor", "ColorDetection");
    SensorFunction getAmbientLight("getAmbientLight()",
        [] { return std::to_string(dezibot.colorDetection.getAmbientLight()); });
    SensorFunction getRGB("getRGB()",
        [] { const uint16_t red = dezibot.colorDetection.getColorValue(VEML_RED);
            const uint16_t green = dezibot.colorDetection.getColorValue(VEML_GREEN);
            const uint16_t blue = dezibot.colorDetection.getColorValue(VEML_BLUE);
            // changed order of colour values to match graph colour in liveDataPage
            return "blue: " + std::to_string(blue) + ", red: " + std::to_string(red) +
                ", green: " + std::to_string(green);});
    SensorFunction getColorValueRed("getColorValue(RED)",
        [] { return std::to_string(dezibot.colorDetection.getColorValue(VEML_RED)); });
    SensorFunction getColorValueGreen("getColorValue(GREEN)",
        [] { return std::to_string(dezibot.colorDetection.getColorValue(VEML_GREEN)); });
    SensorFunction getColorValueBlue("getColorValue(BLUE)",
        [] { return std::to_string(dezibot.colorDetection.getColorValue(VEML_BLUE)); }) ;
    SensorFunction getColorValueWhite("getColorValue(WHITE)",
        [] { return std::to_string(dezibot.colorDetection.getColorValue(VEML_WHITE)); });
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
        [] { return std::to_string(LightDetection::getValue(IR_FRONT)); });
    SensorFunction getValueIrLeft("getValue(IR_LEFT)",
        [] { return std::to_string(LightDetection::getValue(IR_LEFT)); });
    SensorFunction getValueIrRight("getValue(IR_RIGHT)",
        [] { return std::to_string(LightDetection::getValue(IR_RIGHT)); });
    SensorFunction getValueIrBack("getValue(IR_BACK)",
        [] { return std::to_string(LightDetection::getValue(IR_BACK)); });
    SensorFunction getValueDlBottom("getValue(DL_BOTTOM)",
        [] { return std::to_string(LightDetection::getValue(DL_BOTTOM)); });
    SensorFunction getValueDlFront("getValue(DL_FRONT)",
        [] { return std::to_string(LightDetection::getValue(DL_FRONT)); });
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
        [] { return std::to_string(Motion::left.getSpeed()); });
    SensorFunction getSpeedRight("right.getSpeed()",
        [] { return std::to_string(Motion::right.getSpeed()); });
    motor.addFunction(getSpeedLeft);
    motor.addFunction(getSpeedRight);
    addSensor(motor);

    // initialize motion sensor
    Sensor motionSensor("Motion Sensor", "MotionDetection");
    SensorFunction getAcceleration("getAcceleration()",
        [] {
            IMUResult result = Motion::detection.getAcceleration();
            return "x: " + std::to_string(result.x) + ", y: " + std::to_string(result.y) + ", z: " +
                std::to_string(result.z);
    });
    SensorFunction getRotation("getRotation()",
        [] { IMUResult result = Motion::detection.getRotation();
            return "x: " + std::to_string(result.x) + ", y: " + std::to_string(result.y) + ", z: " +
                std::to_string(result.z); });
    SensorFunction getTemperature("getTemperature()",
        [] { return std::to_string(Motion::detection.getTemperature()); });
    SensorFunction getWhoAmI("getWhoAmI()",
        [] { return std::to_string(Motion::detection.getWhoAmI()); });
    SensorFunction getTilt("getTilt()",
        [] {
            Orientation result = Motion::detection.getTilt();
            // Because an INT_MAX makes the graph practically unusable, instead send 0
            if (result.xRotation == INT_MAX && result.yRotation == INT_MAX) {
                result.xRotation = 0;
                result.yRotation = 0;
            }
            return "x: " + std::to_string(result.xRotation) + ", y: " + std::to_string(result.yRotation);
    });
    SensorFunction getTiltDirection("getTiltDirection()",
        [] { Direction result = Motion::detection.getTiltDirection();
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

void DebugServer::addSensor(const Sensor& sensor) {
    sensors.push_back(sensor);
}

std::vector<Sensor>& DebugServer::getSensors() {
    return sensors;
}

// create a FreeRTOS task to handle client requests
void DebugServer::beginClientHandle() {
    xTaskCreatePinnedToCore(
        handleClientTask,
        "DebugServerTask",
        4096,
        this,
        11,
        &taskHandle,
        1
    );
}

// handle client requests
void DebugServer::handleClientTask(void* parameter) {
    const auto debugServer = static_cast<DebugServer*>(parameter);
    TickType_t xLastWakeTime;

     // continuously handle client requests
    while (debugServer->serverActive) {
        constexpr TickType_t xFrequency = 25;
        debugServer->server.handleClient();
        xLastWakeTime = xTaskGetTickCount();
        xTaskDelayUntil(&xLastWakeTime, xFrequency);
    }

    // Delete task when server is no longer active
    vTaskDelete(taskHandle);
}
