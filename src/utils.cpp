#include "utils.h"      // corresponding header
#include <algorithm>    // for std::isspace
#include <cctype>       // for std::isspace
#include <fstream>      // for std::ifstream
#include <iostream>     // for std::cerr
#include <sstream>      // for std::istringstream
#include <string>       // for std::string

// trim(): remove leading/trailing whitespace
std::string trim(const std::string& str) {
    size_t start = 0;
    // advance start until first non‐space
    while (start < str.length() && std::isspace(static_cast<unsigned char>(str[start]))) {
        ++start;
    }
    if (start == str.length()) {
        return "";  // string was all whitespace
    }
    size_t end = str.length() - 1;
    // move end backward until last non‐space
    while (end > start && std::isspace(static_cast<unsigned char>(str[end]))) {
        --end;
    }
    return str.substr(start, end - start + 1);
}

// read_config(): parse key=value pairs from `filename`
bool read_config(const std::string& filename, std::string& engine_out, int& samples_out) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error: Unable to open config file \"" << filename << "\"\n";
        return false;
    }

    std::string engine_temp;
    int samples_temp = -1;
    std::string line;

    while (std::getline(infile, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;  // skip blank/comment lines
        }
        size_t pos = trimmed.find('=');
        if (pos == std::string::npos) {
            continue;  // no '=' → skip
        }
        std::string key = trim(trimmed.substr(0, pos));
        std::string value = trim(trimmed.substr(pos + 1));

        if (key == "ENGINE") {
            engine_temp = value;
        }
        else if (key == "SAMPLES") {
            try {
                samples_temp = std::stoi(value);
            } catch (const std::exception&) {
                std::cerr << "Error: Unable to parse SAMPLES value: " << value << "\n";
                return false;
            }
        }
    }

    infile.close();
    if (engine_temp.empty() || samples_temp < 0) {
        std::cerr << "Error: Config file must contain ENGINE and SAMPLES entries.\n";
        return false;
    }
    engine_out = engine_temp;
    samples_out = samples_temp;
    return true;
}
