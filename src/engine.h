#ifndef ENGINE_H
#define ENGINE_H

#include <vector>   // for std::vector

// A single "sample" consists of (x, y) in [0,1]^2 and the integrand value = 4*I[x^2 + y^2 ≤ 1].
struct Sample {
    double x;      // x‐coordinate ∈ [0,1]
    double y;      // y‐coordinate ∈ [0,1]
    double value;  // integrand value = 4.0 if (x^2 + y^2 ≤ 1), else 0.0
};

// Abstract base class for different sampling engines.
// Each engine must implement `sample(n, outputs)` by appending exactly `n` Sample structs
// (or fewer if it internally adjusts n, e.g. stratified requiring a perfect square).
class Engine {
public:
    // Virtual destructor so derived classes clean up properly
    virtual ~Engine() {}

    // Pure virtual: generate up to `samples` points and fill `outputs` with Sample structs.
    // Derived classes push back into `outputs` exactly one Sample per draw.
    virtual void sample(int samples, std::vector<Sample>& outputs) = 0;
};

#endif // ENGINE_H
