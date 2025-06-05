#include "stratified_engine.h"   // header for this class
#include <cmath>                 // for std::sqrt, std::floor
#include <random>                // for std::uniform_real_distribution
#include <iostream>              // for std::cerr
#include <vector>                // for std::vector

// Constructor: seed RNG with std::random_device
StratifiedEngine::StratifiedEngine() {
    std::random_device rd;      
    rng = std::mt19937(rd());  
}

// Destructor: nothing special
StratifiedEngine::~StratifiedEngine() {}

// sample(): perform stratified sampling in [0,1]^2
void StratifiedEngine::sample(int samples, std::vector<Sample>& outputs) {
    // Compute m = floor(sqrt(samples)), so total actual draws = m*m
    int m = static_cast<int>(std::floor(std::sqrt(static_cast<double>(samples))));
    int total = m * m;

    // If total != requested, warn user
    if (total != samples) {
        std::cerr << "Warning: StratifiedEngine requires a perfect square. "
                  << "Using " << total << " samples instead of " << samples << ".\n";
    }

    // Clear outputs and reserve exactly total
    outputs.clear();
    outputs.reserve(static_cast<size_t>(total));

    // Uniform offset within [0,1)
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    // Loop over each stratum cell (i, j)
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < m; ++j) {
            // Draw a uniform sub‐point (u,v) ∈ [0,1)²
            double u = dist(rng);
            double v = dist(rng);

            // Map into the (i,j)-th stratum in [0,1]
            double x = (static_cast<double>(i) + u) / static_cast<double>(m);
            double y = (static_cast<double>(j) + v) / static_cast<double>(m);

            // Check if inside quarter‐circle
            bool inside = (x * x + y * y) <= 1.0;

            // Integrand value = 4 or 0
            double val = inside ? 4.0 : 0.0;

            // Append this Sample to outputs
            outputs.push_back(Sample{ x, y, val });
        }
    }
}
