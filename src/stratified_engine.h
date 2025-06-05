#ifndef STRATIFIED_ENGINE_H
#define STRATIFIED_ENGINE_H

#include "engine.h"     // base Engine + Sample
#include <random>       // for std::mt19937 and uniform_real_distribution

// StratifiedEngine: subdivide [0,1]^2 into m×m strata (m = floor(sqrt(samples))) and draw one point per cell
class StratifiedEngine : public Engine {
public:
    // Constructor: seed the RNG
    StratifiedEngine();

    // Destructor: nothing special
    ~StratifiedEngine();

    // Override: if samples is not a perfect square, take m = floor(sqrt(samples)), total = m*m
    // Fill outputs with exactly total Sample structs.
    void sample(int samples, std::vector<Sample>& outputs) override;

private:
    std::mt19937 rng;  // Mersenne Twister PRNG
};

#endif // STRATIFIED_ENGINE_H
