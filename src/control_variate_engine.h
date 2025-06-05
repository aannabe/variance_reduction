#ifndef CONTROL_VARIATE_ENGINE_H
#define CONTROL_VARIATE_ENGINE_H

#include "engine.h"      // Brings in `struct Sample { double x, y, value; };`
#include <random>        // For std::mt19937, std::random_device

// ControlVariateEngine:
//   - Draws N i.i.d. points (x_i, y_i) ∼ Uniform([0,1]^2).
//   - For each point, computes:
//       f_i = 4 * I{x_i^2 + y_i^2 ≤ 1}      (so E[f] = π)
//       g_i = x_i^2 + y_i^2                  (so E[g] = 2/3)
//   - Estimates the regression coefficient
//       β = Cov(f,g) / Var(g)
//     via sample covariance and variance.
//   - Forms the adjusted value
//       h_i = f_i + β * (2/3 – g_i)
//     which satisfies E[h] = π and Var(h) < Var(f).
//     β can be obtained minimizing Var(h) w.r.t β. Namely, set d/dβ Var(h) = 0.
//   - Pushes Sample{x_i, y_i, h_i} into outputs, so Welford’s routines in main.cpp work unchanged.
class ControlVariateEngine : public Engine {
public:
    // Constructor: seed the PRNG
    ControlVariateEngine();

    // Destructor: nothing special to clean up
    ~ControlVariateEngine();

    // sample(): generate exactly `samples` adjusted values h_i,
    // storing them in `outputs` as Sample{x_i, y_i, h_i}.
    void sample(int samples, std::vector<Sample>& outputs) override;

private:
    std::mt19937 rng;  // Mersenne Twister PRNG, seeded in the constructor
};

#endif // CONTROL_VARIATE_ENGINE_H
