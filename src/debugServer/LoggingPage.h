/**
 * @file LoggingPage.h
 * @author Tim Dietrich, Felix Herrling
 * @brief This component implements the logging page for the webserver.
 * Log entries are read from the logdatabase and sent to the client.
 * @version 1.0
 * @date 2025-03-23
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef LOGGINGPAGE_H
#define LOGGINGPAGE_H

#include "PageProvider.h"
#include "logger/LogEntry.h"
#include "logger/LogDatabase.h"

class LoggingPage : public PageProvider {
private:
    WebServer* serverPointer;

    void sendLogs() const;
    void sendNewLogs() const;
    void processLogs(const std::vector<LogEntry::Entry>& logs, const String& logLevel) const;

public:
    explicit LoggingPage(WebServer* server);
    void handler() override;
};

#endif //LOGGINGPAGE_H
