/**
 * @file DebugServer.h
 * @author Tim Dietrich, Felix Herrling, Niclas Jost, Marius Busalt
 * @brief This component implements a webserver for debugging purposes.
 * @version 2.0
 * @date 2026-02
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef DEBUGSERVER_H
#define DEBUGSERVER_H
#include "MainPage.h"
#include "LoggingPage.h"
#include "LiveDataPage.h"
#include "SettingsPage.h"
#include "SwarmPage.h"
#include "Sensor.h"

class DebugServer {
private:
    WebServer server;
    MainPage* mainPage;
    LoggingPage* loggingPage;
    LiveDataPage* liveDataPage;
    SettingsPage* settingsPage;
    SwarmPage* swarmPage;
    bool serverActive = true;

    /**
     * @brief Periodically handle client requests to the webserver.
     * @param parameter
     * @return void
     */
    static void handleClientTask(void* parameter);
    std::vector<Sensor> sensors;
    static inline TaskHandle_t taskHandle;
public:
    DebugServer();

    /**
     * @brief Set up the DebugServer.
     * Initialize all webpage and sensor objects.
     * Configure dezibot as wi-fi accesspoint and set ip configuration and credentials.
     * @return void
     */
    void setup();

    /**
     * @brief Start the client handler on a separate thread to run independently of main program.
     * @return void
     */
    void beginClientHandle();

    /**
     * @brief Add a sensor to the list of sensors.
     * @param sensor
     * @return void
     */
    void addSensor(const Sensor& sensor);

    /**
     * @brief Get the list of sensors.
     * @return std::vector<Sensor>&
     */
    std::vector<Sensor>& getSensors();
};

#endif //DEBUGSERVER_H
