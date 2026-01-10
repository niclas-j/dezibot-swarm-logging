/**
 * @file Logger.h
 * @author Tim Dietrich, Felix Herrling
 * @brief The logger class follows the singleton pattern and implements basic logging
 * functions that can be used by developers. Logging functions then make calls to the
 * log database to store the logs.
 * @version 1.0
 * @date 2025-03-30
 * @copyright Copyright (c) 2025
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include "LogEntry.h"

class Logger {
public:
    /**
     * @brief Starts the internal timer used for log entry timestamps.
     * @note This should be called once before any logging operations.
     */
    void startTimer();

    /**
     * @brief Returns the singleton instance of the Logger.
     * @return Logger& Reference to the Logger instance.
     */
    static Logger& getInstance();

    /**
     * @brief Logs an informational message.
     * @param message The message to be logged.
     */
    void logInfo(const std::string& message) const;

    /**
     * @brief Logs a warning message.
     * @param message The warning message to be logged.
     */
    void logWarning(const std::string& message) const;

    /**
     * @brief Logs an error message.
     * @param message The error message to be logged.
     */
    void logError(const std::string& message) const;

    /**
     * @brief Logs a debug message.
     * @param message The debug message to be logged.
     */
    void logDebug(const std::string& message) const;

    /**
     * @brief Logs a trace message.
     * @param message The trace message to be logged.
     */
    void logTrace(const std::string& message) const;

    /**
     * @brief Enables or disables logging globally.
     * @param enabled If true, enables logging; if false, disables all logging.
     */
    void setLoggingEnabled(bool enabled);

private:
    /**
     * @brief Private constructor to enforce singleton pattern.
     */
    Logger() = default;

    /**
     * @brief Default destructor.
     */
    ~Logger() = default;

    /**
     * @brief Internal logging function that handles all log entries.
     * @param level The severity level of the log entry.
     * @param message The message to be logged.
     */
    void log(LogEntry::Level level, const std::string& message) const;

    // Delete copy constructor and assignment operator to prevent copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    /**
     * @brief Generates a timestamp string based on elapsed time since startTimer() was called.
     * @return std::string Formatted timestamp in HH:MM:SS.mm format.
     */
    std::string getCurrentTimestamp() const;

    bool loggingEnabled = true; ///< Flag controlling whether logging is enabled
};

#endif // LOGGER_H