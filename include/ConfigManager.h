#ifndef HAICL_CONFIG_MANAGER_H
#define HAICL_CONFIG_MANAGER_H

#include <string>
#include <map>
#include <filesystem> // Required for std::filesystem
#include "json.hpp"

class ConfigManager {
public:
    ConfigManager();
    
    // Loads configuration. Automatically handles path and file creation.
    void loadConfig(); 
    
    std::string getString(const std::string& key, const std::string& default_value = "") const;
    int getInt(const std::string& key, int default_value = 0) const;
    bool getBool(const std::string& key, bool default_value = false) const;
    std::map<std::string, std::string> getModelParams(const std::string& model_type) const;

private:
    nlohmann::json config_;
    
    // Helper to get the path to the configuration directory.
    std::filesystem::path getConfigPath() const;

    void loadEnvironmentVariables();
    void loadConfigFile(const std::string& config_file_path);
    void mergeConfig(nlohmann::json& target, const nlohmann::json& source);
};

#endif // HAICL_CONFIG_MANAGER_H