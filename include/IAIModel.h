#ifndef HAICL_IAI_MODEL_H
#define HAICL_IAI_MODEL_H

#include <optional>
#include <string>
#include <vector>
#include <map>
#include "json.hpp"

struct Message {
    std::string role;
    std::string content;

    // Helper for JSON serialization/deserialization
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Message, role, content)
};

class IAIModel {
public:
    virtual ~IAIModel() = default;

    // Sends a message to the AI model and returns the response.
    // messages: A vector of Message objects representing the conversation history.
    // model_params: A map of model-specific parameters (e.g., "temperature", "max_tokens").
    // Returns an optional Message object representing the AI's reply, or empty if an error occurs.
    virtual std::optional<Message> sendMessage(const std::vector<Message>& messages, const std::map<std::string, std::string>& model_params) = 0;
};

#endif // HAICL_IAI_MODEL_H


