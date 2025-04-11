/**
 * @file logger.h
 * @brief Header file for logging functionality
 */

#pragma once

#include <string>
#include <QObject>

namespace koebridge {
namespace utils {

/**
 * @enum LogLevel
 * @brief Enumeration of logging levels
 */
enum class LogLevel {
    DEBUG,   ///< Debug level messages
    INFO,    ///< Informational messages
    WARNING, ///< Warning messages
    ERROR,   ///< Error messages
    FATAL    ///< Fatal error messages
};

/**
 * @class Logger
 * @brief Singleton logger class for application-wide logging
 *
 * This class provides a thread-safe singleton logger that can be used
 * throughout the application for consistent logging.
 */
class Logger : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Get the singleton instance of the logger
     * @return Logger& Reference to the singleton logger instance
     */
    static Logger& getInstance();

    /**
     * @brief Set the current logging level
     * @param level The new logging level to set
     */
    void setLevel(LogLevel level);

    /**
     * @brief Log a message at the specified level
     * @param level The logging level for the message
     * @param message The message to log
     */
    void log(LogLevel level, const std::string& message);

    /**
     * @brief Log a debug message
     * @param message The debug message to log
     */
    void debug(const std::string& message);

    /**
     * @brief Log an informational message
     * @param message The info message to log
     */
    void info(const std::string& message);

    /**
     * @brief Log a warning message
     * @param message The warning message to log
     */
    void warning(const std::string& message);

    /**
     * @brief Log an error message
     * @param message The error message to log
     */
    void error(const std::string& message);

    /**
     * @brief Log a fatal error message
     * @param message The fatal error message to log
     */
    void fatal(const std::string& message);

signals:
    /**
     * @brief Signal emitted when a message is logged
     * @param level The logging level of the message
     * @param message The logged message
     */
    void logMessage(LogLevel level, const std::string& message);

private:
    /**
     * @brief Private constructor for singleton pattern
     */
    Logger();

    /**
     * @brief Deleted copy constructor
     */
    Logger(const Logger&) = delete;

    /**
     * @brief Deleted assignment operator
     */
    Logger& operator=(const Logger&) = delete;

    LogLevel currentLevel; ///< Current logging level
};

} // namespace utils
} // namespace koebridge

/**
 * @def LOG_DEBUG(msg)
 * @brief Macro for logging debug messages
 * @param msg The debug message to log
 */
#define LOG_DEBUG(msg) koebridge::utils::Logger::getInstance().debug(msg)

/**
 * @def LOG_INFO(msg)
 * @brief Macro for logging informational messages
 * @param msg The info message to log
 */
#define LOG_INFO(msg) koebridge::utils::Logger::getInstance().info(msg)

/**
 * @def LOG_WARNING(msg)
 * @brief Macro for logging warning messages
 * @param msg The warning message to log
 */
#define LOG_WARNING(msg) koebridge::utils::Logger::getInstance().warning(msg)

/**
 * @def LOG_ERROR(msg)
 * @brief Macro for logging error messages
 * @param msg The error message to log
 */
#define LOG_ERROR(msg) koebridge::utils::Logger::getInstance().error(msg)

/**
 * @def LOG_FATAL(msg)
 * @brief Macro for logging fatal error messages
 * @param msg The fatal error message to log
 */
#define LOG_FATAL(msg) koebridge::utils::Logger::getInstance().fatal(msg)
