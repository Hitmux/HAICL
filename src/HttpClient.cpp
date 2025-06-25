
#include "HttpClient.h"
#include <curl/curl.h>
#include <iostream>

// Callback function to write received data into a string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::optional<nlohmann::json> HttpClient::performRequest(const std::string& url, const std::map<std::string, std::string>& headers, const std::optional<std::string>& post_fields, const std::string& method) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        struct curl_slist* chunk = NULL;
        for (const auto& header : headers) {
            std::string header_str = header.first + ": " + header.second;
            chunk = curl_slist_append(chunk, header_str.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

        if (method == "POST") {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            if (post_fields) {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields->c_str());
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post_fields->length());
            }
        } else if (method == "GET") {
            // GET is default, no specific option needed unless custom request type is set
        } else {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
        }

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_slist_free_all(chunk);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return std::nullopt;
        }

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code != 200) {
            std::cerr << "HTTP request failed with code: " << http_code << ", Response: " << readBuffer << std::endl;
            curl_slist_free_all(chunk);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return std::nullopt;
        }

        curl_slist_free_all(chunk);
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        try {
            return nlohmann::json::parse(readBuffer);
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << ", Response: " << readBuffer << std::endl;
            return std::nullopt;
        }
    }
    return std::nullopt;
}

std::optional<nlohmann::json> HttpClient::post(const std::string& url, const std::map<std::string, std::string>& headers, const nlohmann::json& body) {
    return performRequest(url, headers, body.dump(), "POST");
}

std::optional<nlohmann::json> HttpClient::get(const std::string& url, const std::map<std::string, std::string>& headers) {
    return performRequest(url, headers, std::nullopt, "GET");
}


