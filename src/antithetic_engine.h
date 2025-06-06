#ifndef ANTITHETIC_ENGINE_H
#define ANTITHETIC_ENGINE_H

#include "engine.h"     // defines `struct Sample { double x, y, value; };`
#include <random>       // for std::mt19937

// AntitheticEngine:
//
//   When `sample(n, outputs)` is called with an integer n,
//   we treat n as the total number of function‐evaluations to perform.
//   In antithetic sampling, each “pair” uses two f‐calls:  f(u,v) and f(1-u,1-v).
//   Therefore we form (n/2) pairs and return exactly (n/2) Samples, where each
//   Sample.value = [f(u,v) + f(1-u,1-v)]/2.  We store (u,v) as the sample coordinates.
//
//   If n is odd, we drop the last sample (so we effectively do floor(n/2) pairs).
//   The returned outputs vector therefore has size ⌊n/2⌋.
//
//   In main.cpp, the estimator is still the average of all returned Sample.value’s;
//   since each value is already the average of its two antithetic f‐calls, one ends up
//   with exactly the usual “antithetic mean over n total f‐calls.”
class AntitheticEngine : public Engine {
public:
    // Constructor: seed the PRNG.
    AntitheticEngine();

    // Destructor: nothing special.
    ~AntitheticEngine();

    // sample(n, outputs):
    //  * If n is even, form n/2 pairs.
    //  * If n is odd, form (n-1)/2 pairs (dropping the last “unpaired”).
    //  * For each pair: draw (u,v) ∈ Uniform([0,1]^2), let (u2,v2)=(1-u,1-v).
    //    Compute f1 = 4·I{u^2+v^2 ≤1}, f2 = 4·I{u2^2+v2^2 ≤1}.
    //    Set avg = (f1 + f2)/2 and push Sample{u, v, avg}.
    //
    // At the end, outputs.size() = ⌊n/2⌋.
    void sample(int n, std::vector<Sample>& outputs) override;

private:
    std::mt19937 rng;  // Mersenne Twister PRNG
};

#endif // ANTITHETIC_ENGINE_H
