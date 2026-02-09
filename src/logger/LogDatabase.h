/**
* @file LogDatabase.h
 * @author Tim Dietrich, Felix Herrling
 * @brief The log database class saves all logs in a vector which can then be accessed
 * to read out all existing/new logs. It should not be directly accessed, as logs should
 * be written via the logger class.
 * @version 1.0
 * @date 2025-03-30
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef LOGDATABASE_H
#define LOGDATABASE_H

#include "LogEntry.h"
#include <vector>
#include <mutex>

class LogDatabase {
public:
    /**
     * @brief Returns a reference to the instance of the LogDatabase, used to call functions
     * @return LogDatabase
     */
    static LogDatabase& getInstance();

    /**
     * @brief Adds a log to the vector stored in the instance
     * @param entry a log entry created by the logger class.
     * @return void
     */
    void addLog(const LogEntry::Entry& entry);

    /**
     * @brief Retrieves all log entries stored in the database.
     * @note This marks all logs as "sent" by updating the lastSentIndex_ to the current size.
     * @return const std::vector<LogEntry::Entry>& A reference to the vector containing all log entries.
     */
    const std::vector<LogEntry::Entry>& getLogs();

    /**
     * @brief Retrieves only new log entries that haven't been sent before.
     * @details This function returns all log entries added since the last call to either
     * getLogs() or getNewLogs(). It updates the lastSentIndex_ to the current size after retrieval.
     * @return std::vector<LogEntry::Entry> A vector containing only the new log entries.
     */
    std::vector<LogEntry::Entry> getNewLogs();

private:
    /**
     * @brief Private constructor to enforce singleton pattern.
     * Initializes lastSentIndex_ to 0.
     */
    LogDatabase() : lastSentIndex_(0) {};

    /**
     * @brief Default destructor.
     */
    ~LogDatabase() = default;

    // Delete copy constructor and assignment operator to prevent copying
    LogDatabase(const LogDatabase&) = delete;
    LogDatabase& operator=(const LogDatabase&) = delete;

    static constexpr size_t MAX_LOG_ENTRIES = 500;
    std::vector<LogEntry::Entry> logEntries_; ///< Vector storing all log entries
    std::mutex mutex_; ///< Mutex for thread-safe operations
    size_t lastSentIndex_; ///< Index tracking the last sent log entry
};

#endif // LOGDATABASE_H