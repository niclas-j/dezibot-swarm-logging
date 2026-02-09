/**
 * @file LiveDataPage.h
 * @author Tim Dietrich, Felix Herrling
 * @brief This component implements the livedata page for the webserver.
 * Displays current values for sensors that are enabled for live data.
 * @version 1.0
 * @date 2025-03-23
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef LIVEDATAPAGE_H
#define LIVEDATAPAGE_H

#include <ArduinoJson.h>
#include "PageProvider.h"

class LiveDataPage: public PageProvider {
private:
    WebServer* serverPointer;
public:
    explicit LiveDataPage(WebServer* server);
    void handler() override;
    void getEnabledSensorValues();
    void getRemoteSensorValues(const String &mac);
};

#endif //LIVEDATAPAGE_H
