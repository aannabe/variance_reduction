#ifndef CONTROL_ANTITHETIC_ENGINE_H
#define CONTROL_ANTITHETIC_ENGINE_H

#include "engine.h"     // defines struct Sample { double x, y, value; };
#include <random>       // for std::mt19937

// ControlAntitheticEngine:
//   * Treat `n` as the total number of f‐calls; form M = floor(n/2) antithetic pairs.
//   * For each i=0..M-1:
//       – Draw (u_i, v_i) ∼ Uniform([0,1]^2).
//       – Let (u2_i, v2_i) = (1-u_i, 1-v_i).
//       – Compute f1 = 4·I{u_i^2 + v_i^2 ≤ 1},  f2 = 4·I{u2_i^2 + v2_i^2 ≤ 1}.
//       – Compute g1 = u_i^2 + v_i^2,           g2 = u2_i^2 + v2_i^2.
//       – Define f_pair_i = (f1 + f2)/2,  g_pair_i = (g1 + g2)/2.
//   * After looping all M pairs, compute sample covariance/variance of {f_pair_i, g_pair_i}.
//   * Let β = Cov(f_pair, g_pair) / Var(g_pair)  (if Var(g_pair)>0; else β=0).
//   * For each i build h_i = f_pair_i + β·(2/3 − g_pair_i).  Since E[g]=2/3, E[h]=π.
//   * Push back Sample{ u_i, v_i, h_i } for i=0..M-1.  These M samples go into outputs.
//
//   In main.cpp, these M “values” are then used by Welford to compute mean/variance.  Total f‐calls = 2M (≈n).
class ControlAntitheticEngine : public Engine {
public:
    // Constructor: seeds the RNG
    ControlAntitheticEngine();

    // Destructor: nothing special
    ~ControlAntitheticEngine();

    // sample(n, outputs):
    //   – M = n/2 pairs (floor if n odd)
    //   – outputs.size() == M
    void sample(int n, std::vector<Sample>& outputs) override;

private:
    std::mt19937 rng;  // Mersenne Twister PRNG
};

#endif // CONTROL_ANTITHETIC_ENGINE_H
