#ifndef RANDOM_ENGINE_H
#define RANDOM_ENGINE_H

#include "engine.h"     // base Engine + Sample struct
#include <random>       // for std::mt19937 and std::uniform_real_distribution

// RandomEngine: plain Monte Carlo in [0,1]^2 to estimate π via 4·I[(x,y) inside quarter‐circle]
class RandomEngine : public Engine {
public:
    // Constructor: seed the RNG
    RandomEngine();

    // Destructor: nothing special
    ~RandomEngine();

    // Override: generate up to `samples` uniform points in [0,1]^2 and compute value=4·I[in‐circle].
    void sample(int samples, std::vector<Sample>& outputs) override;

private:
    std::mt19937 rng;  // Mersenne Twister PRNG
};

#endif // RANDOM_ENGINE_H
