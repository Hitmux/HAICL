
#include "GoogleAIModel.h"
#include <iostream>

GoogleAIModel::GoogleAIModel(const std::string& api_key, const std::string& base_url, const std::string& model_name)
    : api_key_(api_key),
      base_url_(base_url),
      model_name_(model_name) {
}

std::optional<Message> GoogleAIModel::sendMessage(const std::vector<Message>& messages, const std::map<std::string, std::string>& model_params) {
    nlohmann::json request_body;
    
    // Google AI (Gemini) API typically uses a 'contents' array for messages
    // and 'generationConfig' for model parameters.
    // This is a simplified mapping and might need adjustment based on specific Gemini API version.
    nlohmann::json contents_array = nlohmann::json::array();
    for (const auto& msg : messages) {
        // Gemini API often expects 'user' and 'model' roles, not 'system'
        std::string role = msg.role;
        if (role == "assistant") {
            role = "model";
        }
        contents_array.push_back({{"role", role}, {"parts", {{{"text", msg.content}}}}});
    }
    request_body["contents"] = contents_array;

    nlohmann::json generation_config;
    for (const auto& param : model_params) {
        if (param.first == "temperature") {
            try {
                generation_config[param.first] = std::stod(param.second);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Could not parse temperature value for Google AI: " << e.what() << std::endl;
            }
        } else if (param.first == "max_tokens") {
            try {
                generation_config["maxOutputTokens"] = std::stoi(param.second);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Could not parse max_tokens value for Google AI: " << e.what() << std::endl;
            }
        } else {
            generation_config[param.first] = param.second;
        }
    }
    if (!generation_config.empty()) {
        request_body["generationConfig"] = generation_config;
    }

    std::map<std::string, std::string> headers;
    headers["Content-Type"] = "application/json";
    // Google AI API key is usually passed as a query parameter or in a specific header
    // For simplicity, we'll assume it's part of the base_url for now or handled by the client if it's a query param.
    // If it needs to be a header, it would be: headers["x-goog-api-key"] = api_key_;

    std::string url = base_url_ + "/v1/models/" + model_name_ + ":generateContent?key=" + api_key_;

    std::optional<nlohmann::json> response = http_client_.post(url, headers, request_body);

    if (response) {
        try {
            if (response->contains("candidates") && !(*response)["candidates"].empty()) {
                const auto& candidate = (*response)["candidates"][0];
                if (candidate.contains("content") && candidate["content"].contains("parts") && !candidate["content"]["parts"].empty()) {
                    Message reply;
                    reply.role = candidate["content"]["role"].get<std::string>();
                    reply.content = candidate["content"]["parts"][0]["text"].get<std::string>();
                    return reply;
                }
            }
            std::cerr << "Error: Unexpected Google AI API response format: " << response->dump(2) << std::endl;
        } catch (const nlohmann::json::exception& e) {
            std::cerr << "Error parsing Google AI API response: " << e.what() << std::endl;
            std::cerr << "Response was: " << response->dump(2) << std::endl;
        }
    }
    return std::nullopt;
}


