#include "HistoryManager.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;

HistoryManager::HistoryManager(const std::string& history_dir)
    : history_dir_(history_dir) {
    if (!fs::exists(history_dir_)) {
        try {
            fs::create_directories(history_dir_);
            std::cout << "Created history directory: " << history_dir_ << std::endl;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error creating history directory " << history_dir_ << ": " << e.what() << std::endl;
        }
    }
}

std::string HistoryManager::getTimestampedFilename() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    return ss.str() + ".txt";
}

std::string HistoryManager::formatMessageLine(const Message& message) const {
    std::string formatted_content = message.content;
    // Replace actual newline characters with a placeholder string
    std::string::size_type pos = 0;
    while ((pos = formatted_content.find('\n', pos)) != std::string::npos) {
        formatted_content.replace(pos, 1, "[NEWLINE]");
        pos += strlen("[NEWLINE]"); // Advance past the inserted placeholder
    }
    return message.role + ": " + formatted_content;
}

// Helper to parse a single line into a Message.
std::optional<Message> HistoryManager::parseMessageLine(const std::string& line) const {
    size_t colon_pos = line.find(": ");
    if (colon_pos != std::string::npos) {
        Message msg;
        msg.role = line.substr(0, colon_pos);
        std::string parsed_content = line.substr(colon_pos + 2);
        // Replace placeholder string back to actual newline characters
        std::string::size_type pos = 0;
        while ((pos = parsed_content.find("[NEWLINE]", pos)) != std::string::npos) {
            parsed_content.replace(pos, strlen("[NEWLINE]"), "\n");
            pos += 1; // Advance past the inserted newline character
        }
        msg.content = parsed_content;
        return msg;
    }
    return std::nullopt;
}

std::string HistoryManager::saveConversation(const std::vector<Message>& conversation) {
    std::string filename = getTimestampedFilename();
    fs::path file_path = history_dir_ / filename;
    std::ofstream ofs(file_path);
    if (ofs.is_open()) {
        for (const auto& msg : conversation) {
            ofs << formatMessageLine(msg) << "\n";
        }
        ofs.close();
        std::cout << "Conversation saved to: " << file_path << std::endl;
        return filename;
    } else {
        std::cerr << "Error saving conversation to: " << file_path << std::endl;
        return "";
    }
}

std::optional<std::vector<Message>> HistoryManager::loadConversation(const std::string& filename) {
    fs::path file_path = history_dir_ / filename;
    if (!fs::exists(file_path)) {
        std::cerr << "Error: Conversation file not found: " << file_path << std::endl;
        return std::nullopt;
    }

    std::vector<Message> conversation;
    std::ifstream ifs(file_path);
    if (ifs.is_open()) {
        std::string line;
        while (std::getline(ifs, line)) {
            if (auto msg = parseMessageLine(line)) {
                conversation.push_back(*msg);
            } else {
                std::cerr << "Warning: Could not parse line in " << filename << ": " << line << std::endl;
            }
        }
        ifs.close();
        std::cout << "Conversation loaded from: " << file_path << std::endl;
        return conversation;
    } else {
        std::cerr << "Error loading conversation from: " << file_path << std::endl;
        return std::nullopt;
    }
}

std::vector<std::string> HistoryManager::listConversations() const {
    std::vector<std::string> filenames;
    try {
        for (const auto& entry : fs::directory_iterator(history_dir_)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                filenames.push_back(entry.path().filename().string());
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error listing conversations in " << history_dir_ << ": " << e.what() << std::endl;
    }
    std::sort(filenames.rbegin(), filenames.rend()); // Sort by newest first
    return filenames;
}

bool HistoryManager::modifyMessage(const std::string& filename, size_t message_index, const std::string& new_content) {
    fs::path file_path = history_dir_ / filename;
    if (!fs::exists(file_path)) {
        std::cerr << "Error: Conversation file not found for modification: " << file_path << std::endl;
        return false;
    }

    std::vector<std::string> lines;
    std::ifstream ifs(file_path);
    if (ifs.is_open()) {
        std::string line;
        while (std::getline(ifs, line)) {
            lines.push_back(line);
        }
        ifs.close();
    } else {
        std::cerr << "Error opening conversation file for modification: " << file_path << std::endl;
        return false;
    }

    if (message_index >= lines.size()) {
        std::cerr << "Error: Message index out of bounds for modification: " << message_index << std::endl;
        return false;
    }

    // Parse the original message to get the role, then reconstruct the line
    if (auto original_msg = parseMessageLine(lines[message_index])) {
        Message updated_msg = *original_msg;
        updated_msg.content = new_content;
        lines[message_index] = formatMessageLine(updated_msg);
    } else {
        std::cerr << "Error: Could not parse original message at index " << message_index << " for modification." << std::endl;
        return false;
    }

    std::ofstream ofs(file_path, std::ios::trunc); // Truncate to overwrite
    if (ofs.is_open()) {
        for (const auto& line : lines) {
            ofs << line << "\n";
        }
        ofs.close();
        std::cout << "Message at index " << message_index << " in " << filename << " modified successfully." << std::endl;
        return true;
    } else {
        std::cerr << "Error writing modified conversation to: " << file_path << std::endl;
        return false;
    }
}


