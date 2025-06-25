#ifndef HAICL_CONFIG_MANAGER_H
#define HAICL_CONFIG_MANAGER_H

#include <string>
#include <map>
#include "json.hpp"

class ConfigManager {
public:
    ConfigManager();
    void loadConfig(const std::string& config_file_path = "config.json");
    std::string getString(const std::string& key, const std::string& default_value = "") const;
    int getInt(const std::string& key, int default_value = 0) const;
    bool getBool(const std::string& key, bool default_value = false) const;
    std::map<std::string, std::string> getModelParams(const std::string& model_type) const;

private:
    nlohmann::json config_;
    void loadEnvironmentVariables();
    void loadConfigFile(const std::string& config_file_path);
    void mergeConfig(nlohmann::json& target, const nlohmann::json& source); // New helper for merging with priority
};

#endif // HAICL_CONFIG_MANAGER_H

