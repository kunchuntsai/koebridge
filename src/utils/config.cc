#include "config.h"
#include "logger.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <algorithm>

namespace koebridge {
namespace utils {

// Helper function to trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char c) {
        return std::isspace(c);
    });
    auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char c) {
        return std::isspace(c);
    }).base();
    return (start < end) ? std::string(start, end) : std::string();
}

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

bool Config::load(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open config file: " + configPath);
        return false;
    }

    LOG_INFO("Loading configuration from: " + configPath);
    std::string line;
    int lineNumber = 0;

    // Clear existing configuration
    configData.clear();
    LOG_DEBUG("Cleared existing configuration");

    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            LOG_DEBUG("Line " + std::to_string(lineNumber) + ": Skipping " +
                     (line.empty() ? "empty line" : "comment"));
            continue;
        }

        // Parse key-value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));

            if (!key.empty()) {
                configData[key] = value;
                LOG_DEBUG("Line " + std::to_string(lineNumber) + ": Loaded config - " + key + " = " + value);
            } else {
                LOG_WARNING("Line " + std::to_string(lineNumber) + ": Empty key found, skipping");
            }
        } else {
            LOG_WARNING("Line " + std::to_string(lineNumber) + ": Invalid format (no '=' found): " + line);
        }
    }

    LOG_INFO("Configuration loaded successfully with " + std::to_string(configData.size()) + " entries");

    // Log all loaded configuration entries
    LOG_INFO("=== Configuration Content ===");
    for (const auto& [key, value] : configData) {
        LOG_INFO(key + " = " + value);
    }
    LOG_INFO("==========================");

    return true;
}

bool Config::save(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& pair : configData) {
        file << pair.first << " = " << pair.second << std::endl;
    }

    return true;
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) {
    auto it = configData.find(key);
    return (it != configData.end()) ? it->second : defaultValue;
}

std::string Config::getPath(const std::string& key) {
    LOG_DEBUG("Attempting to get path for key: " + key);
    auto it = configData.find(key);
    if (it == configData.end()) {
        LOG_ERROR("Config key not found: " + key);
        throw std::runtime_error("Config key not found: " + key);
    }

    std::string expandedPath = expandPath(it->second);
    LOG_DEBUG("Retrieved path for " + key + ": " + expandedPath);
    return expandedPath;
}

int Config::getInt(const std::string& key, int defaultValue) {
    auto it = configData.find(key);
    if (it != configData.end()) {
        try {
            return std::stoi(it->second);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

float Config::getFloat(const std::string& key, float defaultValue) {
    auto it = configData.find(key);
    if (it != configData.end()) {
        try {
            return std::stof(it->second);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue) {
    auto it = configData.find(key);
    if (it != configData.end()) {
        return (it->second == "true" || it->second == "1" || it->second == "yes");
    }
    return defaultValue;
}

void Config::setString(const std::string& key, const std::string& value) {
    configData[key] = value;
}

void Config::setPath(const std::string& key, const std::string& value) {
    // Store the path as-is, expansion happens on retrieval
    configData[key] = value;
}

void Config::setInt(const std::string& key, int value) {
    configData[key] = std::to_string(value);
}

void Config::setFloat(const std::string& key, float value) {
    configData[key] = std::to_string(value);
}

void Config::setBool(const std::string& key, bool value) {
    configData[key] = value ? "true" : "false";
}

std::string Config::expandPath(const std::string& path) const {
    if (path.empty()) {
        return path;
    }

    std::string result = path;

    // Expand home directory
    if (result[0] == '~') {
        const char* home = std::getenv("HOME");
        if (home) {
            result.replace(0, 1, home);
        } else {
            throw std::runtime_error("HOME environment variable not set");
        }
    }

    // Expand environment variables
    size_t pos = 0;
    while ((pos = result.find("${", pos)) != std::string::npos) {
        size_t end = result.find("}", pos);
        if (end == std::string::npos) {
            break;
        }

        std::string var = result.substr(pos + 2, end - pos - 2);
        const char* value = std::getenv(var.c_str());

        if (value) {
            result.replace(pos, end - pos + 1, value);
        } else {
            throw std::runtime_error("Environment variable not set: " + var);
        }
    }

    return result;
}

} // namespace utils
} // namespace koebridge
