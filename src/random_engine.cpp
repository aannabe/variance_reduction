#include "random_engine.h"   // header for this class
#include <random>            // for std::random_device, std::uniform_real_distribution
#include <cmath>             // for std::sqrt (not strictly needed here)
#include <vector>            // for std::vector

// Constructor: seed the Mersenne Twister with a non‐deterministic seed
RandomEngine::RandomEngine() {
    std::random_device rd;        // obtain a random seed from the OS
    rng = std::mt19937(rd());     // seed the PRNG
}

// Destructor: nothing to clean up
RandomEngine::~RandomEngine() {}

// sample(): draw `samples` uniform points in [0,1]^2; fill outputs with Sample{x,y,value}
void RandomEngine::sample(int samples, std::vector<Sample>& outputs) {
    // Clear any existing contents and reserve space for exactly `samples`
    outputs.clear();
    outputs.reserve(static_cast<size_t>(samples));

    // Uniform distribution on [0,1)
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (int i = 0; i < samples; ++i) {
        // Draw a uniform x and y in [0,1)
        double x = dist(rng);
        double y = dist(rng);

        // Determine if (x,y) lies inside the unit quarter‐circle
        bool inside = (x * x + y * y) <= 1.0;

        // integrand = 4 if inside, else 0
        double val = inside ? 4.0 : 0.0;

        // Append this Sample to the outputs vector
        outputs.push_back(Sample{ x, y, val });
    }
}
