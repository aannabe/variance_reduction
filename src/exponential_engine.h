#ifndef EXPONENTIAL_ENGINE_H
#define EXPONENTIAL_ENGINE_H

#include "engine.h"
#include <random>

// ExponentialEngine: Importance Sampling via p(x,y) ∝ e^{-λ(x+y)} truncated to [0,1]^2.
class ExponentialEngine : public Engine {
public:
    // Constructor: seed the RNG and set λ
    ExponentialEngine(double lambda_ = 1.0);

    // Destructor
    ~ExponentialEngine();

    // Sample up to `samples` points in [0,1]^2, each weighted by f/p
    void sample(int samples, std::vector<Sample>& outputs) override;

private:
    std::mt19937 rng;   // Mersenne Twister RNG
    double lambda;      // rate parameter for the truncated exponential
};

#endif // EXPONENTIAL_ENGINE_H
