#include "config.h"
#include "logger.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <QDir>
#include <cctype>
#include <regex>

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

bool Config::load(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        Logger::getInstance().error("Failed to open config file: " + filePath);
        return false;
    }

    std::string line;
    std::string currentSection;
    std::regex sectionRegex("^\\[(.*)\\]$");
    std::regex keyValueRegex("^([^=]+)=(.*)$");
    std::smatch matches;

    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }

        // Check for section
        if (std::regex_match(line, matches, sectionRegex)) {
            currentSection = matches[1].str();
            continue;
        }

        // Check for key-value pair
        if (std::regex_match(line, matches, keyValueRegex)) {
            std::string key = matches[1].str();
            std::string value = matches[2].str();

            // Trim whitespace
            key = std::regex_replace(key, std::regex("^\\s+|\\s+$"), "");
            value = std::regex_replace(value, std::regex("^\\s+|\\s+$"), "");

            // Create full key with section
            std::string fullKey = currentSection.empty() ? key : currentSection + "." + key;

            // Store the value
            configData[fullKey] = value;

            // Debug output for model path
            if (fullKey == "translation.model_path") {
                Logger::getInstance().info("Model path from config: " + value);
                // Expand path if needed
                std::string expandedPath = expandPath(value);
                Logger::getInstance().info("Expanded model path: " + expandedPath);
            }
        }
    }

    Logger::getInstance().info("Loaded configuration from: " + filePath);
    return true;
}

bool Config::save(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        Logger::getInstance().error("Failed to open config file for writing: " + filePath);
        return false;
    }

    std::map<std::string, std::vector<std::pair<std::string, std::string>>> sections;

    // Group settings by section
    for (const auto& [key, value] : configData) {
        size_t dotPos = key.find('.');
        if (dotPos != std::string::npos) {
            std::string section = key.substr(0, dotPos);
            std::string setting = key.substr(dotPos + 1);
            sections[section].emplace_back(setting, value);
        } else {
            sections[""].emplace_back(key, value);
        }
    }

    // Write settings grouped by section
    for (const auto& [section, settings] : sections) {
        if (!section.empty()) {
            file << "[" << section << "]\n";
        }
        for (const auto& [key, value] : settings) {
            file << key << "=" << value << "\n";
        }
        file << "\n";
    }

    Logger::getInstance().info("Saved configuration to: " + filePath);
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

    // Validate path exists and is accessible
    QDir dir(QString::fromStdString(expandedPath));
    if (!dir.exists()) {
        LOG_ERROR("Path does not exist: " + expandedPath);
        throw std::runtime_error("Path does not exist: " + expandedPath);
    }

    if (!dir.isReadable()) {
        LOG_ERROR("Path is not readable: " + expandedPath);
        throw std::runtime_error("Path is not readable: " + expandedPath);
    }

    if (key.find("model") != std::string::npos) {
        std::string modelName = expandedPath.substr(expandedPath.find_last_of("/\\") + 1);
        LOG_DEBUG("Retrieved path for " + key + ": " + expandedPath + " (Model: " + modelName + ")");
    } else {
        LOG_DEBUG("Retrieved path for " + key + ": " + expandedPath);
    }
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
    std::string result = path;

    // Expand home directory
    if (result[0] == '~') {
        const char* home = std::getenv("HOME");
        if (home) {
            result.replace(0, 1, home);
        }
    }

    // Expand environment variables
    std::regex envVarRegex("\\$\\{([^}]+)\\}");
    std::smatch matches;
    std::string::const_iterator searchStart(result.cbegin());

    while (std::regex_search(searchStart, result.cend(), matches, envVarRegex)) {
        std::string envVar = matches[1].str();
        const char* envValue = std::getenv(envVar.c_str());
        if (envValue) {
            result.replace(matches[0].first - result.begin(),
                         matches[0].length(),
                         envValue);
            searchStart = result.cbegin() + (matches[0].first - result.begin() + strlen(envValue));
        } else {
            searchStart = matches[0].second;
        }
    }

    return result;
}

} // namespace utils
} // namespace koebridge
