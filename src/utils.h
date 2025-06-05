#ifndef UTILS_H
#define UTILS_H

#include <string>   // for std::string

// Trim whitespace from both ends of `str`
std::string trim(const std::string& str);

// Read a simple key=value config file `filename`. Expects lines like:
//   ENGINE = Random
//   SAMPLES = 1000
// Returns true if successful; on success, sets `engine_out` and `samples_out`.
bool read_config(const std::string& filename, std::string& engine_out, int& samples_out);

#endif // UTILS_H
