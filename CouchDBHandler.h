#pragma once

#include <curl/curl.h>
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class CouchDBHandler {
private:
    std::string baseUrl;
    std::string username;
    std::string password;

public:
    CouchDBHandler(const std::string& baseUrl, const std::string& username, const std::string& password)
        : baseUrl(baseUrl), username(username), password(password) {
        curl_global_init(CURL_GLOBAL_ALL);
    }

    ~CouchDBHandler() {
        curl_global_cleanup();
    }

    std::string createDatabase(const std::string& dbName) {
        return sendRequest(dbName, "PUT");
    }

    std::string deleteDatabase(const std::string& dbName) {
        return sendRequest(dbName, "DELETE");
    }

    std::string createDocument(const std::string& dbName, const std::string& docId, const json& jsonData) {
        std::string jsonDataStr = jsonData.dump();

        //This cout is only for testing purposes
        std::cout << jsonDataStr << std::endl;

        std::string endpoint = dbName + "/" + docId;
        return sendRequest(endpoint, "PUT", jsonDataStr);
    }

    json getDocument(const std::string& dbName, const std::string& docId) {
        std::string endpoint = dbName + "/" + docId;
        std::string response = sendRequest(endpoint, "GET");
        json jsonData = json::parse(response);
        return jsonData;
    }

    std::string updateDocument(const std::string& dbName, const std::string& docId, const json& jsonData) {
        json currentDoc = getDocument(dbName, docId);

        if (currentDoc.empty()) {
            return "Document not found";
        }

        std::string currentRev = currentDoc["_rev"].get<std::string>();

        json newData = jsonData;
        newData["_rev"] = currentRev;

        std::string updatedJson = newData.dump();

        //This cout is only for testing purposes
        std::cout << updatedJson << std::endl;

        std::string endpoint = dbName + "/" + docId;
        return sendRequest(endpoint, "PUT", updatedJson);
    }

    std::string deleteDocument(const std::string& dbName, const std::string& docId) {
        json currentDoc = getDocument(dbName, docId);

        if (currentDoc.empty()) {
            return "Document not found";
        }

        std::string currentRev = currentDoc["_rev"].get<std::string>();

        std::string endpoint = dbName + "/" + docId + "?rev=" + currentRev;
        return sendRequest(endpoint, "DELETE");
    }

private:
    std::string sendRequest(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        std::string response;

        CURL* curl = curl_easy_init();
        if (curl) {
            std::string url = baseUrl + endpoint;

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
            curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

            if (method == "PUT" || method == "POST") {
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            }
            else if (method == "DELETE") {
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
            }

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "Curl error: " << curl_easy_strerror(res) << std::endl;
            }

            curl_easy_cleanup(curl);
        }
        return response;
    }

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
        size_t total_size = size * nmemb;
        response->append((char*)contents, total_size);
        return total_size;
    }
};
