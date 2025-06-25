#ifndef HAICL_GOOGLE_AI_MODEL_H
#define HAICL_GOOGLE_AI_MODEL_H

#include "IAIModel.h"
#include "HttpClient.h"
#include "json.hpp"
#include <string>
#include <vector>
#include <map>

class GoogleAIModel : public IAIModel {
public:
    GoogleAIModel(const std::string& api_key, const std::string& base_url, const std::string& model_name);

    std::optional<Message> sendMessage(const std::vector<Message>& messages, const std::map<std::string, std::string>& model_params) override;

private:
    std::string api_key_;
    std::string base_url_;
    std::string model_name_;
    HttpClient http_client_;
};

#endif // HAICL_GOOGLE_AI_MODEL_H


