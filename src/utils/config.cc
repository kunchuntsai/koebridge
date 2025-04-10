#include "config.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <stdexcept>

namespace koebridge {
namespace utils {

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

bool Config::load(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            configData[key] = value;
        }
    }

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

std::string Config::getPath(const std::string& key, const std::string& defaultValue) {
    return expandPath(getString(key, defaultValue));
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
