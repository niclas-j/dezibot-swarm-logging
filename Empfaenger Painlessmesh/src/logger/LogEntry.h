/**
* @file LogEntry.h
 * @author Tim Dietrich, Felix Herrling
 * @brief This struct serves as the basic structure of every log entry, where
 * every log entry contains an enum based log level, a timestamp and a message.
 * @version 1.0
 * @date 2025-03-30
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef LOGENTRY_H
#define LOGENTRY_H

#include <string>

namespace LogEntry {
    enum Level {
        INFO,
        WARNING,
        ERROR,
        DEBUG,
        TRACE
    };

    struct Entry {
        Level level;
        std::string timestamp;
        std::string message;
    };
}

#endif //LOGENTRY_H
