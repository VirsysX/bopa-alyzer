#include <iostream>
#include <curl/curl.h>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

// Structure to hold technology details, including detection location
struct Technology {
    std::string name;
    std::vector<std::pair<std::regex, std::string>> patterns; // Regex and where to search (html, headers, files)
};

// Callback function to capture the data from cURL response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    if (s == nullptr) return 0;
    s->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

// Function to fetch content using cURL (reusable)
std::string fetchContent(const std::string& url, bool getHeaders = false, std::string* headers = nullptr) {
    CURL* curl = curl_easy_init();
    std::string content;
    CURLcode res;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        std::string header_string;
        if (getHeaders) {
            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);
        }

        res = curl_easy_perform(curl);

        if (res == CURLE_OK && getHeaders && headers != nullptr) {
            *headers = header_string;
        } else if (res != CURLE_OK) {
            std::cerr << "cURL request failed for " << url << ": " << curl_easy_strerror(res) << std::endl;
            content = ""; // Return empty string on failure
        }
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize cURL." << std::endl;
    }
    return content;
}

// Function to parse HTTP headers into a map
std::unordered_map<std::string, std::string> parseHeaders(const std::string& headerString) {
    std::unordered_map<std::string, std::string> headers;
    std::istringstream headerStream(headerString);
    std::string line;
    while (std::getline(headerStream, line)) {
        if (line.empty() || line == "\r") continue;
        size_t separator = line.find(':');
        if (separator != std::string::npos) {
            std::string key = line.substr(0, separator);
            std::string value = line.substr(separator + 1);
            // Trim leading/trailing whitespace from key and value
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            headers[key] = value;
        }
    }
    return headers;
}

// Function to check for the existence of specific files (e.g., robots.txt)
bool checkFileExists(const std::string& url) {
    CURL* curl = curl_easy_init();
    CURLcode res = CURLE_FAILED_INIT;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); // Perform a HEAD request
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        return res == CURLE_OK;
    }
    return false;
}

// Function to identify technologies based on various methods
void identifyTechnologies(const std::string& url, const std::string& htmlContent, const std::unordered_map<std::string, std::string>& headers) {
    std::vector<Technology> technologies = {
        {"WordPress", {
            {std::regex("wp-(content|includes)", std::regex::icase), "html"},
            {std::regex("wordpress", std::regex::icase), "headers:x-powered-by"},
            {std::regex("/wp-login\\.php", std::regex::icase), "files"}
        }},
        {"jQuery", {
            {std::regex("jquery[-.\\d]*(?:\\.min)?\\.js", std::regex::icase), "html"}
        }},
        {"React", {
            {std::regex("react(-dom)?\\.production\\.min\\.js|ReactDOM", std::regex::icase), "html"}
        }},
        {"Angular", {
            {std::regex("angular(?:\\.min)?\\.js", std::regex::icase), "html"}
        }},
        {"Node.js", {
            {std::regex("Powered by Node\\.js", std::regex::icase), "html"},
            {std::regex("node", std::regex::icase), "headers:x-powered-by"}
        }},
        {"Bootstrap", {
            {std::regex("bootstrap(?:\\.min)?\\.css", std::regex::icase), "html"}
        }},
        {"Vue.js", {
            {std::regex("vue(?:\\.min)?\\.js", std::regex::icase), "html"}
        }},
        {"PHP", {
            {std::regex("<\?php", std::regex::icase), "html"},
            {std::regex("PHP/", std::regex::icase), "headers:x-powered-by"},
            {std::regex("\\.php$", std::regex::icase), "files"} // Check for PHP file extensions
        }},
        {"Nginx", {
            {std::regex("nginx", std::regex::icase), "headers:server"}
        }},
        {"Apache", {
            {std::regex("apache", std::regex::icase), "headers:server"}
        }},
        {"Laravel", {
            {std::regex("laravel", std::regex::icase), "html"}, // Often in comments or meta tags
            {std::regex("laravel_session", std::regex::icase), "headers:set-cookie"}
        }},
        {"Django", {
            {std::regex("django", std::regex::icase), "html"},
            {std::regex("csrftoken", std::regex::icase), "headers:set-cookie"}
        }},
        // Add more technologies and refined patterns here!
    };

    std::cout << "Detected technologies for " << url << ":" << std::endl;
    bool detected = false;
    for (const auto& tech : technologies) {
        for (const auto& patternInfo : tech.patterns) {
            const std::regex& pattern = patternInfo.first;
            const std::string& location = patternInfo.second;

            if (location == "html" && std::regex_search(htmlContent, pattern)) {
                std::cout << "- " << tech.name << " (found in HTML)" << std::endl;
                detected = true;
                break; // Move to the next technology once found
            } else if (location.rfind("headers:", 0) == 0) {
                std::string headerKey = location.substr(8);
                if (headers.count(headerKey) && std::regex_search(headers.at(headerKey), pattern)) {
                    std::cout << "- " << tech.name << " (found in headers - " << headerKey << ")" << std::endl;
                    detected = true;
                    break;
                }
            } else if (location == "files") {
                // Heuristic check for common files
                std::vector<std::string> filePaths;
                if (tech.name == "WordPress") filePaths = {"/wp-login.php", "/wp-admin/", "/wp-content/"};
                else if (tech.name == "PHP") filePaths = {"/index.php", "/info.php"}; // Example, add more
                // Add file paths for other technologies as needed

                for (const auto& filePath : filePaths) {
                    if (checkFileExists(url + filePath)) {
                        std::cout << "- " << tech.name << " (common files found)" << std::endl;
                        detected = true;
                        break;
                    }
                }
                if (detected) break;
            }
        }
    }

    if (!detected) {
        std::cout << "No specific technologies detected based on current patterns." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <URL>" << std::endl;
        return 1;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);

    std::string url = argv[1];
    std::string headers;
    std::string htmlContent = fetchContent(url, true, &headers);

    if (!htmlContent.empty()) {
        std::unordered_map<std::string, std::string> parsedHeaders = parseHeaders(headers);
        identifyTechnologies(url, htmlContent, parsedHeaders);
    }

    curl_global_cleanup();
    return 0;
}
