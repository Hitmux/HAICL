#ifndef HAICL_HTTP_CLIENT_H
#define HAICL_HTTP_CLIENT_H

#include <string>
#include <map>
#include <optional>
#include "json.hpp"

// Callback function for libcurl to write received data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

class HttpClient {
public:
    // Performs an HTTP POST request
    // url: The URL to send the request to
    // headers: A map of HTTP headers (e.g., {"Content-Type", "application/json"})
    // body: The request body as a JSON object
    // Returns an optional JSON object representing the response, or empty if an error occurs
    std::optional<nlohmann::json> post(const std::string& url, const std::map<std::string, std::string>& headers, const nlohmann::json& body);

    // Performs an HTTP GET request
    // url: The URL to send the request to
    // headers: A map of HTTP headers
    // Returns an optional JSON object representing the response, or empty if an error occurs
    std::optional<nlohmann::json> get(const std::string& url, const std::map<std::string, std::string>& headers);

private:
    // Helper function to perform a generic HTTP request
    std::optional<nlohmann::json> performRequest(const std::string& url, const std::map<std::string, std::string>& headers, const std::optional<std::string>& post_fields, const std::string& method);
};

#endif // HAICL_HTTP_CLIENT_H


