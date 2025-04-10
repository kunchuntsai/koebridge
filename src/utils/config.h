/**
 * @file config.h
 * @brief Header file for configuration management
 */

#pragma once

#include <string>
#include <map>
#include <filesystem>

namespace fs = std::filesystem;

namespace koebridge {
namespace utils {

/**
 * @class Config
 * @brief Singleton configuration manager for application settings
 *
 * This class provides a thread-safe singleton configuration manager that can be used
 * throughout the application for consistent configuration management.
 */
class Config {
public:
    /**
     * @brief Get the singleton instance of the configuration manager
     * @return Config& Reference to the singleton config instance
     */
    static Config& getInstance();

    /**
     * @brief Load configuration from a file
     * @param filePath Path to the configuration file
     * @return bool True if configuration was loaded successfully, false otherwise
     */
    bool load(const std::string& filePath);

    /**
     * @brief Save current configuration to a file
     * @param filePath Path where the configuration file should be saved
     * @return bool True if configuration was saved successfully, false otherwise
     */
    bool save(const std::string& filePath);

    /**
     * @brief Get a string configuration value
     * @param key The configuration key
     * @param defaultValue Default value if key is not found
     * @return std::string The configuration value
     */
    std::string getString(const std::string& key, const std::string& defaultValue = "");

    /**
     * @brief Get a path configuration value with expansion
     * @param key The configuration key
     * @param defaultValue Default value if key is not found
     * @return std::string The expanded path value
     */
    std::string getPath(const std::string& key, const std::string& defaultValue = "");

    /**
     * @brief Get an integer configuration value
     * @param key The configuration key
     * @param defaultValue Default value if key is not found
     * @return int The configuration value
     */
    int getInt(const std::string& key, int defaultValue = 0);

    /**
     * @brief Get a float configuration value
     * @param key The configuration key
     * @param defaultValue Default value if key is not found
     * @return float The configuration value
     */
    float getFloat(const std::string& key, float defaultValue = 0.0f);

    /**
     * @brief Get a boolean configuration value
     * @param key The configuration key
     * @param defaultValue Default value if key is not found
     * @return bool The configuration value
     */
    bool getBool(const std::string& key, bool defaultValue = false);

    /**
     * @brief Set a string configuration value
     * @param key The configuration key
     * @param value The value to set
     */
    void setString(const std::string& key, const std::string& value);

    /**
     * @brief Set a path configuration value
     * @param key The configuration key
     * @param value The path value to set
     */
    void setPath(const std::string& key, const std::string& value);

    /**
     * @brief Set an integer configuration value
     * @param key The configuration key
     * @param value The value to set
     */
    void setInt(const std::string& key, int value);

    /**
     * @brief Set a float configuration value
     * @param key The configuration key
     * @param value The value to set
     */
    void setFloat(const std::string& key, float value);

    /**
     * @brief Set a boolean configuration value
     * @param key The configuration key
     * @param value The value to set
     */
    void setBool(const std::string& key, bool value);

private:
    /**
     * @brief Expand path with environment variables and home directory
     * @param path The path to expand
     * @return std::string The expanded path
     */
    std::string expandPath(const std::string& path) const;

    /**
     * @brief Private constructor for singleton pattern
     */
    Config() = default;

    /**
     * @brief Deleted copy constructor
     */
    Config(const Config&) = delete;

    /**
     * @brief Deleted assignment operator
     */
    Config& operator=(const Config&) = delete;

    std::map<std::string, std::string> configData; ///< Storage for configuration key-value pairs
};

} // namespace utils
} // namespace koebridge
