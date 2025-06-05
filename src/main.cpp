#include <iostream>             // for std::cout, std::cerr
#include <vector>               // for std::vector
#include <memory>               // for std::unique_ptr, std::make_unique
#include <string>               // for std::string
#include <cmath>                // for std::sqrt
#include <fstream>              // for std::ofstream
#include <iomanip>              // for std::fixed, std::setprecision

#include "engine.h"             // base Engine + Sample
#include "random_engine.h"      // RandomEngine
#include "stratified_engine.h"  // StratifiedEngine
#include "exponential_engine.h" // ExponentialEngine
#include "antithetic_engine.h"  // AntitheticEngine
#include "control_variate_engine.h" // ControlVariateEngine
#include "utils.h"              // read_config, trim

int main() {
    // 1) Read configuration from "input.in"
    std::string engine_name;     // Will hold e.g. "Random" or "Stratified"
    int requested_samples = 0;   // Will hold the integer that user wants

    // Call updated read_config that also fills `lambda`
    if (!read_config("input.in", engine_name, requested_samples)) {
        // If it fails (missing ENGINE or SAMPLES, or parse error), exit with error
        return 1;
    }

    // 2) Instantiate the chosen engine (as a unique_ptr to base class)
    std::unique_ptr<Engine> engine_ptr;

    if (engine_name == "Random") {
        // Now using std::make_unique (C++14)
        engine_ptr = std::make_unique<RandomEngine>();
    }
    else if (engine_name == "Stratified") {
        engine_ptr = std::make_unique<StratifiedEngine>();
    }
    else if (engine_name == "Exponential") {
        double lambda = 0.6;   // hard-coded value
        engine_ptr = std::make_unique<ExponentialEngine>(lambda);
    }
    else if (engine_name == "Antithetic") {
        engine_ptr = std::make_unique<AntitheticEngine>();
    }
    else if (engine_name == "ControlVariate") {
        engine_ptr = std::make_unique<ControlVariateEngine>();
    }
    else {
        std::cerr << "Error: Unknown ENGINE \"" << engine_name << "\" in config.\n";
        return 1;
    }

    // 3) Collect all samples into a vector<Sample>
    std::vector<Sample> samples;           // Each Sample holds (x,y,value)
    engine_ptr->sample(requested_samples, samples);
    int actual_samples = static_cast<int>(samples.size());  // May differ if stratified adjusted

    // 4) Open results.log for appending so we can log per-sample info
    std::ofstream logfile("results.log", std::ios::app);
    if (!logfile.is_open()) {
        std::cerr << "Warning: Could not open results.log for writing.\n";
    } else {
        // Write a header for this run
        logfile << "# Engine: " << engine_name
                << "  Requested: " << requested_samples
                << "  Actual: " << actual_samples << "\n";
        logfile << "# n  x       y       value    mean     var      stderr\n";
        logfile << std::fixed << std::setprecision(6);
    }

    // 5) Initialize Welford’s algorithm for running mean & M2
    // This is a one-pass, on-the-fly computation of the mean and variances.
    // It gives a similar numerical accuracy compared to the usual two-pass version, calculated as:
    //    i) mean = sum(x_i) / N
    //    ii) var = sum(x - mean)^2 / (N-1)

    double mean = 0.0;     // running mean of the integrand values
    double M2 = 0.0;       // running sum of squared deviations
    // At step n: variance = M2/(n-1) for n>1; for n=1 we set var=0

    // 6) Loop over each sample index and update online stats + log
    for (int i = 0; i < actual_samples; ++i) {
        // Current Sample struct
        double x     = samples[i].x;      // x-coordinate
        double y     = samples[i].y;      // y-coordinate
        double value = samples[i].value;  // integrand = 4 or 0

        int n = i + 1;  // current sample count

        // 6a) Welford update for mean & M2
        double delta = value - mean;
        mean += delta / static_cast<double>(n);   // new running mean
        double delta2 = value - mean;
        M2 += delta * delta2;                     // accumulate sum of squares

        // 6b) Compute sample variance (unbiased) for n>1; else 0.0
        double variance = 0.0;
        if (n > 1) {
            variance = M2 / static_cast<double>(n - 1);
        }

        // 6c) Compute standard error = sqrt(variance / n) if n>1; else 0
        double std_error = 0.0;
        if (n > 1) {
            std_error = std::sqrt(variance / static_cast<double>(n));
        }

        // 6d) Append to logfile if it’s open, all with 6 decimal places
        if (logfile.is_open()) {
            logfile << n << "  "
                    << x        << "  "
                    << y        << "  "
                    << value    << "  "
                    << mean     << "  "
                    << variance << "  "
                    << std_error<< "\n";
        }
    }

    // 7) Close logfile
    if (logfile.is_open()) {
        logfile.close();
    }

    // 8) Compute final variance & std_error for console output
    double final_variance = 0.0;
    double final_std_error = 0.0;
    if (actual_samples > 1) {
        final_variance = M2 / static_cast<double>(actual_samples - 1);
        final_std_error = std::sqrt(final_variance / static_cast<double>(actual_samples));
    }

    // 9) Print summary to console with fixed precision (six decimals)
    std::cout << std::fixed << std::setprecision(8);
    std::cout << "Engine:            " << engine_name          << "\n";
    std::cout << "Requested Samples: " << requested_samples    << "\n";
    std::cout << "Actual Samples:    " << actual_samples       << "\n";
    std::cout << "Final Estimate π:  " << mean                  << "\n";
    std::cout << "Final Variance:    " << final_variance        << "\n";
    std::cout << "Final Std. Error:  " << final_std_error       << "\n";

    return 0;
}
