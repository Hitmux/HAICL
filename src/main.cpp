#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <sstream>

#include "ConfigManager.h"
#include "CLIParser.h"
#include "HttpClient.h"
#include "IAIModel.h"
#include "OpenAIModel.h"
#include "GoogleAIModel.h"
#include "HistoryManager.h"
#include "TerminalBeautifier.h"

// Function to get AI model based on type and config
std::unique_ptr<IAIModel> getAIModel(const ConfigManager& config, const std::string& model_type_arg, const std::string& model_name_arg) {
    std::string actual_model_type = model_type_arg.empty() ? config.getString("default_ai_model", "openai") : model_type_arg;

    if (actual_model_type == "openai") {
        std::string api_key = config.getString("openai.api_key");
        std::string base_url = config.getString("openai.base_url", "https://api.openai.com/v1");
        std::string model_name = model_name_arg.empty() ? config.getString("openai.model_name", "gpt-3.5-turbo") : model_name_arg;
        if (api_key.empty()) {
            std::cerr << TerminalBeautifier::red("Error: OpenAI API key not found. Please set OPENAI_API_KEY environment variable or in config.json.") << std::endl;
            return nullptr;
        }
        // Only create OpenAIModel if API key is present
        return std::make_unique<OpenAIModel>(api_key, base_url, model_name);
    } else if (actual_model_type == "google") {
        std::string api_key = config.getString("google.api_key");
        std::string base_url = config.getString("google.base_url", "https://generativelanguage.googleapis.com");
        std::string model_name = model_name_arg.empty() ? config.getString("google.model_name", "gemini-pro") : model_name_arg;
        if (api_key.empty()) {
            std::cerr << TerminalBeautifier::red("Error: Google AI API key not found. Please set GOOGLE_API_KEY environment variable or in config.json.") << std::endl;
            return nullptr;
        }
        // Only create GoogleAIModel if API key is present
        return std::make_unique<GoogleAIModel>(api_key, base_url, model_name);
    } else {
        std::cerr << TerminalBeautifier::red("Error: Unsupported AI model type: ") << actual_model_type << std::endl;
        return nullptr;
    }
}

// Function to handle quick question mode
void handleQuickQuestion(IAIModel* model, const std::string& prompt, const std::map<std::string, std::string>& model_params) {
    std::cout << TerminalBeautifier::bold(TerminalBeautifier::cyan("You: ")) << prompt << std::endl;
    if (!model) {
        std::cerr << TerminalBeautifier::red("Error: AI model not initialized. Cannot send message.") << std::endl;
        return;
    }
    std::vector<Message> messages = {{"user", prompt}};
    std::optional<Message> reply = model->sendMessage(messages, model_params);
    if (reply) {
        std::cout << TerminalBeautifier::bold(TerminalBeautifier::green("AI: ")) << reply->content << std::endl;
    } else {
        std::cerr << TerminalBeautifier::red("Failed to get a response from the AI. This might be due to network issues, invalid API key, or an issue with the AI service itself.") << std::endl;
    }
}

// Function to handle interactive mode
void handleInteractiveMode(IAIModel* model, HistoryManager& history_manager, const CommandLineArgs& args, const std::map<std::string, std::string>& initial_model_params) {
    std::vector<Message> conversation;

    if (!args.load_history_file.empty()) {
        std::optional<std::vector<Message>> loaded_conversation = history_manager.loadConversation(args.load_history_file);
        if (loaded_conversation) {
            conversation = *loaded_conversation;
            std::cout << TerminalBeautifier::yellow("Loaded conversation from: ") << args.load_history_file << std::endl;
            for (const auto& msg : conversation) {
                if (msg.role == "user") {
                    std::cout << TerminalBeautifier::bold(TerminalBeautifier::cyan("You: ")) << msg.content << std::endl;
                } else if (msg.role == "assistant" || msg.role == "model") {
                    std::cout << TerminalBeautifier::bold(TerminalBeautifier::green("AI: ")) << msg.content << std::endl;
                }
            }
        } else {
            std::cerr << TerminalBeautifier::red("Could not load conversation from: ") << args.load_history_file << std::endl;
        }
    }

    std::cout << TerminalBeautifier::yellow("Entering interactive mode. Type \\\"exit\\\" to quit, \\\"save\\\" to save, \\\"list\\\" to list history, \\\"show\\\" to display current conversation, \\\"load <filename>\\\" to load history, \\\"modify <index> <new_content>\\\" to modify message, \\\"help\\\" for commands.") << std::endl;
    if (!model) {
        std::cerr << TerminalBeautifier::yellow("Warning: AI model not initialized. AI interaction is disabled. Please ensure you have set a valid API key and selected a supported model type.") << std::endl;
    }

    std::string user_input;
    while (true) {
        std::cout << std::endl << TerminalBeautifier::bold(TerminalBeautifier::cyan("You: "));
        std::getline(std::cin, user_input);

        if (user_input == "exit") {
            break;
        } else if (user_input == "save") {
            if (!conversation.empty()) {
                history_manager.saveConversation(conversation);
            } else {
                std::cout << TerminalBeautifier::yellow("No conversation to save.") << std::endl;
            }
            continue;
        } else if (user_input == "list") {
            std::vector<std::string> history_files = history_manager.listConversations();
            if (history_files.empty()) {
                std::cout << TerminalBeautifier::yellow("No history files found.") << std::endl;
            } else {
                std::cout << TerminalBeautifier::yellow("Available history files:") << std::endl;
                for (const auto& file : history_files) {
                    std::cout << "- " << file << std::endl;
                }
            }
            continue;
        } else if (user_input == "show") {
            if (conversation.empty()) {
                std::cout << TerminalBeautifier::yellow("No active conversation.") << std::endl;
            } else {
                std::cout << TerminalBeautifier::yellow("Current conversation:") << std::endl;
                for (size_t i = 0; i < conversation.size(); ++i) {
                    const auto& msg = conversation[i];
                    if (msg.role == "user") {
                        std::cout << i << ": " << TerminalBeautifier::bold(TerminalBeautifier::cyan("You: ")) << msg.content << std::endl;
                    } else if (msg.role == "assistant" || msg.role == "model") {
                        std::cout << i << ": " << TerminalBeautifier::bold(TerminalBeautifier::green("AI: ")) << msg.content << std::endl;
                    }
                }
            }
            continue;
        } else if (user_input == "help") {
            std::cout << TerminalBeautifier::yellow("Available commands:") << std::endl;
            std::cout << TerminalBeautifier::yellow("  exit: Quit the interactive mode.") << std::endl;
            std::cout << TerminalBeautifier::yellow("  save: Save the current conversation to a new file.") << std::endl;
            std::cout << TerminalBeautifier::yellow("  list: List all saved history files.") << std::endl;
            std::cout << TerminalBeautifier::yellow("  show: Display the current conversation.") << std::endl;
            std::cout << TerminalBeautifier::yellow("  load <filename>: Load a conversation from a specified file.") << std::endl;
            std::cout << TerminalBeautifier::yellow("  modify <index> <new_content>: Modify a message at a specific index in the current conversation.") << std::endl;
            std::cout << TerminalBeautifier::yellow("  help: Display this help message.") << std::endl;
            continue;
        } else if (user_input.rfind("load ", 0) == 0) { // Starts with "load "
            std::string filename_to_load = user_input.substr(5);
            std::optional<std::vector<Message>> loaded_conv = history_manager.loadConversation(filename_to_load);
            if (loaded_conv) {
                conversation = *loaded_conv;
                std::cout << TerminalBeautifier::yellow("Loaded conversation from: ") << filename_to_load << std::endl;
                for (const auto& msg : conversation) {
                    if (msg.role == "user") {
                        std::cout << TerminalBeautifier::bold(TerminalBeautifier::cyan("You: ")) << msg.content << std::endl;
                    } else if (msg.role == "assistant" || msg.role == "model") {
                        std::cout << TerminalBeautifier::bold(TerminalBeautifier::green("AI: ")) << msg.content << std::endl;
                    }
                }
            } else {
                std::cerr << TerminalBeautifier::red("Failed to load conversation.") << std::endl;
            }
            continue;
        } else if (user_input.rfind("modify ", 0) == 0) { // Starts with "modify "
            std::istringstream iss(user_input.substr(7));
            size_t index;
            std::string new_content_str;
            iss >> index;
            std::getline(iss, new_content_str); // Read the rest of the line
            if (!new_content_str.empty() && new_content_str[0] == ' ') {
                new_content_str = new_content_str.substr(1); // Remove leading space
            }

            if (index < conversation.size()) {
                // Modify in memory first
                conversation[index].content = new_content_str;
                std::cout << TerminalBeautifier::yellow("Message at index ") << index << TerminalBeautifier::yellow(" modified in current session.") << std::endl;
                // Persist changes to file if a history file was loaded
                if (!args.load_history_file.empty()) {
                    history_manager.modifyMessage(args.load_history_file, index, new_content_str);
                }
            } else {
                std::cerr << TerminalBeautifier::red("Invalid message index.") << std::endl;
            }
            continue;
        }

        // Only attempt to send message to AI if model is initialized
        if (model) {
            conversation.push_back({"user", user_input});
            std::optional<Message> reply = model->sendMessage(conversation, initial_model_params);
            if (reply) {
                std::cout << TerminalBeautifier::bold(TerminalBeautifier::green("AI: ")) << reply->content << std::endl;
                conversation.push_back(*reply);
            } else {
                std::cerr << TerminalBeautifier::red("Failed to get a response from the AI. This might be due to network issues, invalid API key, or an issue with the AI service itself.") << std::endl;
                conversation.pop_back(); // Remove user message if AI failed to respond
            }
        } else {
            std::cerr << TerminalBeautifier::yellow("AI interaction is currently disabled. Your message was not sent to the AI. Please provide a valid API key and model type to enable AI responses.") << std::endl;
            // Do not add user_input to conversation if AI interaction is disabled
        }
    }

    if (!conversation.empty() && args.save_history_file.empty()) {
        // Auto-save if not manually saved and conversation exists
        history_manager.saveConversation(conversation);
    } else if (!conversation.empty() && !args.save_history_file.empty()) {
        // If a specific save file was requested, save to that file
        history_manager.saveConversation(conversation);
    }
}

int main(int argc, char** argv) {
    ConfigManager config;
    config.loadConfig(); // Load default config.json and environment variables

    // Debugging: Print loaded configuration
    std::cout << TerminalBeautifier::yellow("--- Loaded Configuration ---") << std::endl;
    std::cout << "Default AI Model: " << config.getString("default_ai_model", "N/A") << std::endl;
    std::cout << "OpenAI API Key: " << config.getString("openai.api_key", "N/A") << std::endl;
    std::cout << "OpenAI Base URL: " << config.getString("openai.base_url", "N/A") << std::endl;
    std::cout << "OpenAI Model Name: " << config.getString("openai.model_name", "N/A") << std::endl;
    std::cout << "Google API Key: " << config.getString("google.api_key", "N/A") << std::endl;
    std::cout << "Google Base URL: " << config.getString("google.base_url", "N/A") << std::endl;
    std::cout << "Google Model Name: " << config.getString("google.model_name", "N/A") << std::endl;
    std::cout << TerminalBeautifier::yellow("--------------------------") << std::endl;

    CLIParser parser(argc, argv);
    if (!parser.parse()) {
        return 1; // Error or help requested
    }

    const CommandLineArgs& args = parser.getArgs();

    // Determine model parameters, command line args override config
    std::map<std::string, std::string> model_params = config.getModelParams(args.model_type.empty() ? config.getString("default_ai_model", "openai") : args.model_type);
    for (const auto& param_str : args.model_params) {
        size_t eq_pos = param_str.find("=");
        if (eq_pos != std::string::npos) {
            std::string key = param_str.substr(0, eq_pos);
            std::string value = param_str.substr(eq_pos + 1);
            model_params[key] = value;
        } else {
            std::cerr << TerminalBeautifier::yellow("Warning: Invalid model parameter format: ") << param_str << ". Expected key=value." << std::endl;
        }
    }

    std::unique_ptr<IAIModel> ai_model = getAIModel(config, args.model_type, args.model_name);

    HistoryManager history_manager;

    if (!args.prompt.empty()) {
        // Quick question mode
        if (!ai_model) {
            std::cerr << TerminalBeautifier::red("Error: Cannot use quick question mode without an initialized AI model. Please ensure you have set a valid API key (e.g., OPENAI_API_KEY) and selected a supported model type (e.g., -t openai).") << std::endl;
            return 1;
        }
        handleQuickQuestion(ai_model.get(), args.prompt, model_params);
    } else if (args.interactive_mode || !args.load_history_file.empty()) {
        // Interactive mode or load history to continue
        handleInteractiveMode(ai_model.get(), history_manager, args, model_params);
    } else {
        std::cout << TerminalBeautifier::yellow("No prompt or interactive mode specified. Use -h for help.") << std::endl;
    }

    return 0;
}


