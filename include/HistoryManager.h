#ifndef HAICL_HISTORY_MANAGER_H
#define HAICL_HISTORY_MANAGER_H

#include <string>
#include <vector>
#include <filesystem>
#include <optional>
#include <cstdlib> // For std::getenv
#include "IAIModel.h" // For Message struct
#include "json.hpp"

namespace fs = std::filesystem;

class HistoryManager {
public:
    HistoryManager(const std::string& history_dir = std::string(std::getenv("HOME")) + "/.history");

    // Saves a conversation to a new file.
    // conversation: Vector of Message objects representing the conversation.
    // Returns the filename of the saved conversation, or empty string on failure.
    std::string saveConversation(const std::vector<Message>& conversation);

    // Loads a conversation from a specified file.
    // filename: The name of the history file to load.
    // Returns an optional vector of Message objects, or empty if file not found or parsing fails.
    std::optional<std::vector<Message>> loadConversation(const std::string& filename);

    // Lists all available conversation history files.
    // Returns a vector of filenames.
    std::vector<std::string> listConversations() const;

    // Modifies a specific message in a conversation file.
    // filename: The name of the history file to modify.
    // message_index: The 0-based index of the message to modify.
    // new_content: The new content for the message.
    // Returns true on success, false on failure.
    bool modifyMessage(const std::string& filename, size_t message_index, const std::string& new_content);

private:
    fs::path history_dir_;

    // Helper to get a timestamped filename.
    std::string getTimestampedFilename() const;

    // Helper to parse a single line into a Message.
    std::optional<Message> parseMessageLine(const std::string& line) const;

    // Helper to format a Message into a string line.
    std::string formatMessageLine(const Message& message) const;
};

#endif // HAICL_HISTORY_MANAGER_H
