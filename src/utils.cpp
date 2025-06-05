#include "utils.h"      // corresponding header
#include <algorithm>    // for std::isspace
#include <cctype>       // for std::isspace
#include <fstream>      // for std::ifstream
#include <iostream>     // for std::cerr
#include <sstream>      // for std::istringstream
#include <string>       // for std::string

// trim(): remove leading/trailing whitespace from str
std::string trim(const std::string& str) {
    // Find first non‐space character from the left
    size_t start = 0;
    while (start < str.length() && std::isspace(static_cast<unsigned char>(str[start]))) {
        ++start;
    }
    // If entire string is whitespace, return empty
    if (start == str.length()) {
        return "";
    }
    // Find last non‐space character from the right
    size_t end = str.length() - 1;
    while (end > start && std::isspace(static_cast<unsigned char>(str[end]))) {
        --end;
    }
    // Return the substring without leading/trailing whitespace
    return str.substr(start, end - start + 1);
}

// read_config(): parse key=value pairs from filename.
bool read_config(const std::string& filename,
                 std::string& engine_out,
                 int& samples_out){
    // Open the file for reading
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error: Unable to open config file \"" << filename << "\"\n";
        return false;
    }

    // Temporary storage for parsed values
    std::string engine_temp;
    int samples_temp = -1;

    std::string line;
    while (std::getline(infile, line)) {
        // Trim whitespace from both ends
        std::string trimmed = trim(line);

        // Skip blank lines or lines starting with '#' (comments)
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        // Look for the '=' separating key and value
        size_t pos = trimmed.find('=');
        if (pos == std::string::npos) {
            // No '=' on this line: skip it
            continue;
        }

        // Extract key and value substrings, trimming both
        std::string key = trim(trimmed.substr(0, pos));
        std::string value = trim(trimmed.substr(pos + 1));

        // Check which key we have
        if (key == "ENGINE") {
            engine_temp = value;
        }
        else if (key == "SAMPLES") {
            try {
                samples_temp = std::stoi(value);
            } catch (const std::exception&) {
                std::cerr << "Error: Unable to parse SAMPLES value: \"" << value << "\"\n";
                infile.close();
                return false;
            }
        }
        // Other keys are ignored
    }

    infile.close();  // close the file

    // Verify that ENGINE and SAMPLES were provided
    if (engine_temp.empty() || samples_temp < 0) {
        std::cerr << "Error: Config file must contain ENGINE and SAMPLES entries.\n";
        return false;
    }

    // Assign output parameters
    engine_out = engine_temp;
    samples_out = samples_temp;
    return true;
}
