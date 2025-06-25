
#include "OpenAIModel.h"
#include <iostream>

OpenAIModel::OpenAIModel(const std::string& api_key, const std::string& base_url, const std::string& model_name)
    : api_key_(api_key),
      base_url_(base_url),
      model_name_(model_name) {
}

std::optional<Message> OpenAIModel::sendMessage(const std::vector<Message>& messages, const std::map<std::string, std::string>& model_params) {
    nlohmann::json request_body;
    request_body["model"] = model_name_;

    nlohmann::json messages_array = nlohmann::json::array();
    for (const auto& msg : messages) {
        messages_array.push_back({{"role", msg.role}, {"content", msg.content}});
    }
    request_body["messages"] = messages_array;

    // Add model parameters if provided
    for (const auto& param : model_params) {
        // Basic type handling for common parameters. More robust parsing might be needed for complex types.
        if (param.first == "temperature") {
            try {
                request_body[param.first] = std::stod(param.second);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Could not parse temperature value ";
            }
        } else if (param.first == "max_tokens") {
            try {
                request_body[param.first] = std::stoi(param.second);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Could not parse max_tokens value ";
            }
        } else {
            request_body[param.first] = param.second;
        }
    }

    std::map<std::string, std::string> headers;
    headers["Content-Type"] = "application/json";
    headers["Authorization"] = "Bearer " + api_key_;

    std::string url = base_url_ + "/chat/completions";

    std::optional<nlohmann::json> response = http_client_.post(url, headers, request_body);

    if (response) {
        try {
            if (response->contains("choices") && !(*response)["choices"].empty()) {
                const auto& choice = (*response)["choices"][0];
                if (choice.contains("message")) {
                    Message reply;
                    reply.role = choice["message"]["role"].get<std::string>();
                    reply.content = choice["message"]["content"].get<std::string>();
                    return reply;
                }
            }
            std::cerr << "Error: Unexpected API response format: " << response->dump(2) << std::endl;
        } catch (const nlohmann::json::exception& e) {
            std::cerr << "Error parsing OpenAI API response: " << e.what() << std::endl;
            std::cerr << "Response was: " << response->dump(2) << std::endl;
        }
    }
    return std::nullopt;
}


