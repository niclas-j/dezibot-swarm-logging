/**
 * @file LoggingPage.cpp
 * @author Tim Dietrich, Felix Herrling
 * @brief Implementation of the LoggingPage class.
 * @version 1.0
 * @date 2025-03-23
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "LoggingPage.h"
#include "logger/LogDatabase.h"
#include <ArduinoJson.h>
#include "Utility.h"

LoggingPage::LoggingPage(WebServer* server): serverPointer(server) {
    serverPointer->on("/logging/getLogs", [this]() {
        sendLogs();
    });

    serverPointer->on("/logging/getNewLogs", [this]() {
        sendNewLogs();
    });
}

void LoggingPage::handler() {
    serveFileFromSpiffs(serverPointer, "/index.html", "text/html");
}

void LoggingPage::sendLogs() const {
    String logLevel = serverPointer->arg("level");
    String macFilter = serverPointer->arg("mac");
    auto& logs = LogDatabase::getInstance().getLogs();
    processLogs(logs, logLevel, macFilter);
}

void LoggingPage::sendNewLogs() const {
    String logLevel = serverPointer->arg("level");
    String macFilter = serverPointer->arg("mac");
    auto logs = LogDatabase::getInstance().getNewLogs();
    processLogs(logs, logLevel, macFilter);
}

void LoggingPage::processLogs(const std::vector<LogEntry::Entry>& logs, const String& logLevel, const String& macFilter) const {
    JsonDocument jsonDocument;
    JsonArray logsJson = jsonDocument.to<JsonArray>();

    for (const auto& log : logs) {
        const bool matchesLevel = logLevel == "ALL" || logLevel == Utility::logLevelToString(log.level);
        const bool matchesMac = macFilter.length() == 0 || macFilter == String(log.sourceMac.c_str());

        if (matchesLevel && matchesMac) {
            JsonObject logJson = logsJson.add<JsonObject>();
            logJson["level"] = Utility::logLevelToString(log.level);
            logJson["timestamp"] = log.timestamp;
            logJson["message"] = log.message;
            logJson["mac"] = log.sourceMac;
        }
    }

    String jsonResponse;
    serializeJson(jsonDocument, jsonResponse);
    serverPointer->send(200, "application/json", jsonResponse);
}
