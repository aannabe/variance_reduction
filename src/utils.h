#ifndef UTILS_H
#define UTILS_H

#include <string>   // for std::string

// Trim whitespace from both ends of `str`
std::string trim(const std::string& str);

// Read a simple key=value config file named `filename`.
// It expects lines like:
//   SAMPLES =  1000
//   ENGINE  = Random
//
// Returns true if the file was read successfully. On success:
//   - engine_out  is set to the ENGINE string
//   - samples_out is set to the parsed integer
//
// If ENGINE or SAMPLES is missing, or if a parse error occurs, returns false.
bool read_config(const std::string& filename,
                 std::string& engine_out,
                 int& samples_out);

#endif // UTILS_H
