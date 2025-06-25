
#include "ConfigManager.h"
#include <iostream>
#include <fstream>
#include <cstdlib> // For std::getenv
#include <filesystem> // For std::filesystem::exists

namespace fs = std::filesystem;

ConfigManager::ConfigManager() {
    // Initialize with empty JSON object
    config_ = nlohmann::json::object();
}

void ConfigManager::loadEnvironmentVariables() {
    // Load API keys and URLs from environment variables
    const char* openai_api_key = std::getenv("OPENAI_API_KEY");
    if (openai_api_key) {
        config_["openai"]["api_key"] = openai_api_key;
    }
    const char* openai_base_url = std::getenv("OPENAI_BASE_URL");
    if (openai_base_url) {
        std::string url_str = openai_base_url;
        // Remove surrounding quotes if present
        if (url_str.length() >= 2 && url_str.front() == '"' && url_str.back() == '"') {
            url_str = url_str.substr(1, url_str.length() - 2);
        }
        config_["openai"]["base_url"] = url_str;
    }
    const char* openai_model_name = std::getenv("OPENAI_MODEL_NAME");
    if (openai_model_name) {
        config_["openai"]["model_name"] = openai_model_name;
    }

    const char* google_api_key = std::getenv("GOOGLE_API_KEY");
    if (google_api_key) {
        config_["google"]["api_key"] = google_api_key;
    }
    const char* google_base_url = std::getenv("GOOGLE_BASE_URL");
    if (google_base_url) {
        config_["google"]["base_url"] = google_base_url;
    }
    const char* google_model_name = std::getenv("GOOGLE_MODEL_NAME");
    if (google_model_name) {
        config_["google"]["model_name"] = google_model_name;
    }

    const char* default_ai_model = std::getenv("DEFAULT_AI_MODEL");
    if (default_ai_model) {
        config_["default_ai_model"] = default_ai_model;
    }
}

void ConfigManager::loadConfigFile(const std::string& config_file_path) {
    if (fs::exists(config_file_path)) {
        std::ifstream ifs(config_file_path);
        if (ifs.is_open()) {
            try {
                nlohmann::json file_config = nlohmann::json::parse(ifs);
                // Merge file_config into config_, prioritizing existing values (from env vars)
                mergeConfig(config_, file_config);
            } catch (const nlohmann::json::parse_error& e) {
                std::cerr << "Error parsing config file " << config_file_path << ": " << e.what() << std::endl;
            }
        } else {
            std::cerr << "Warning: Could not open config file: " << config_file_path << std::endl;
        }
    } else {
        std::cerr << "Warning: Config file not found: " << config_file_path << std::endl;
    }
}

void ConfigManager::loadConfig(const std::string& config_file_path) {
    // Load environment variables first
    loadEnvironmentVariables();
    // Then load from file, merging into existing config (env vars have higher priority)
    loadConfigFile(config_file_path);
}

void ConfigManager::mergeConfig(nlohmann::json& target, const nlohmann::json& source) {
    for (auto it = source.begin(); it != source.end(); ++it) {
        if (it.value().is_object()) {
            if (target.contains(it.key()) && target[it.key()].is_object()) {
                mergeConfig(target[it.key()], it.value());
            } else {
                target[it.key()] = it.value();
            }
        } else {
            if (!target.contains(it.key())) { // Only add if not already present (from env vars)
                target[it.key()] = it.value();
            }
        }
    }
}

std::string ConfigManager::getString(const std::string& key, const std::string& default_value) const {
    try {
        // Split key by "." for nested access
        nlohmann::json current_node = config_;
        size_t start = 0;
        size_t end = key.find(".");
        while (end != std::string::npos) {
            std::string sub_key = key.substr(start, end - start);
            if (current_node.contains(sub_key)) {
                current_node = current_node[sub_key];
            } else {
                return default_value;
            }
            start = end + 1;
            end = key.find(".", start);
        }
        std::string last_key = key.substr(start);
        if (current_node.contains(last_key) && current_node[last_key].is_string()) {
            return current_node[last_key].get<std::string>();
        }
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "Error getting string config for key " << key << ": " << e.what() << std::endl;
    }
    return default_value;
}

int ConfigManager::getInt(const std::string& key, int default_value) const {
    try {
        nlohmann::json current_node = config_;
        size_t start = 0;
        size_t end = key.find(".");
        while (end != std::string::npos) {
            std::string sub_key = key.substr(start, end - start);
            if (current_node.contains(sub_key)) {
                current_node = current_node[sub_key];
            } else {
                return default_value;
            }
            start = end + 1;
            end = key.find(".", start);
        }
        std::string last_key = key.substr(start);
        if (current_node.contains(last_key) && current_node[last_key].is_number_integer()) {
            return current_node[last_key].get<int>();
        }
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "Error getting int config for key " << key << ": " << e.what() << std::endl;
    }
    return default_value;
}

bool ConfigManager::getBool(const std::string& key, bool default_value) const {
    try {
        nlohmann::json current_node = config_;
        size_t start = 0;
        size_t end = key.find(".");
        while (end != std::string::npos) {
            std::string sub_key = key.substr(start, end - start);
            if (current_node.contains(sub_key)) {
                current_node = current_node[sub_key];
            } else {
                return default_value;
            }
            start = end + 1;
            end = key.find(".", start);
        }
        std::string last_key = key.substr(start);
        if (current_node.contains(last_key) && current_node[last_key].is_boolean()) {
            return current_node[last_key].get<bool>();
        }
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "Error getting bool config for key " << key << ": " << e.what() << std::endl;
    }
    return default_value;
}

std::map<std::string, std::string> ConfigManager::getModelParams(const std::string& model_type) const {
    std::map<std::string, std::string> params;
    try {
        if (config_.contains(model_type) && config_[model_type].contains("model_params") && config_[model_type]["model_params"].is_object()) {
            for (auto it = config_[model_type]["model_params"].begin(); it != config_[model_type]["model_params"].end(); ++it) {
                params[it.key()] = it.value().dump(); // Store as string, convert later if needed
            }
        }
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "Error getting model parameters for type " << model_type << ": " << e.what() << std::endl;
    }
    return params;
}


