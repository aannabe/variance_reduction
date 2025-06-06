#include "control_antithetic_engine.h"
#include <cmath>      // for std::sqrt
#include <random>     // for std::random_device, std::uniform_real_distribution
#include <vector>     // for std::vector
#include <numeric>    // for std::accumulate

ControlAntitheticEngine::ControlAntitheticEngine() {
    // Seed the PRNG once at construction
    std::random_device rd;
    rng = std::mt19937(rd());
}

ControlAntitheticEngine::~ControlAntitheticEngine() { }

void ControlAntitheticEngine::sample(int n, std::vector<Sample>& outputs) {
    // 1) Determine number of antithetic pairs M = floor(n/2).
    int M = n / 2;  // integer division automatically floors if n is odd

    // 2) Reserve space for intermediate f_pair and g_pair
    std::vector<double> f_pair(M), g_pair(M);

    // 3) We'll also store the (u_i, v_i) for each pair so we can push Samples later
    std::vector<double> us(M), vs(M);

    // 4) Set up Uniform(0,1) distribution for drawing u, v
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    // 5) Loop over each pair to compute f_pair[i], g_pair[i]
    for (int i = 0; i < M; ++i) {
        // 5a) Draw one point (u, v)
        double u = dist(rng);
        double v = dist(rng);
        us[i] = u;
        vs[i] = v;

        // 5b) Antithetic partner (u2, v2) = (1-u, 1-v)
        double u2 = 1.0 - u;
        double v2 = 1.0 - v;

        // 5c) Compute f1 = 4·I{u^2 + v^2 ≤ 1}
        bool inside1 = (u * u + v * v) <= 1.0;
        double f1 = inside1 ? 4.0 : 0.0;

        // 5d) Compute f2 = 4·I{u2^2 + v2^2 ≤ 1}
        bool inside2 = (u2 * u2 + v2 * v2) <= 1.0;
        double f2 = inside2 ? 4.0 : 0.0;

        // 5e) Compute g1 = u^2 + v^2, g2 = u2^2 + v2^2
        double g1 = u * u + v * v;
        double g2 = u2 * u2 + v2 * v2;

        // 5f) Pair‐averages
        f_pair[i] = 0.5 * (f1 + f2);
        g_pair[i] = 0.5 * (g1 + g2);
    }

    // 6) Compute sample means of {f_pair} and {g_pair}
    double sum_fpair = std::accumulate(f_pair.begin(), f_pair.end(), 0.0);
    double sum_gpair = std::accumulate(g_pair.begin(), g_pair.end(), 0.0);
    double mean_fpair = sum_fpair / static_cast<double>(M);
    double mean_gpair = sum_gpair / static_cast<double>(M);

    // 7) Compute sample covariance Cov(f_pair, g_pair) and sample variance Var(g_pair)
    double sum_cov = 0.0;
    double sum_var_g = 0.0;
    for (int i = 0; i < M; ++i) {
        double df = f_pair[i] - mean_fpair;   // deviation in f_pair
        double dg = g_pair[i] - mean_gpair;   // deviation in g_pair
        sum_cov    += df * dg;    
        sum_var_g  += dg * dg;
    }
    // Unbiased estimates use denominator (M - 1)
    double cov_f_g  = sum_cov   / static_cast<double>(M - 1);
    double var_gpair = sum_var_g / static_cast<double>(M - 1);

    // 8) Estimate β = Cov(f_pair, g_pair) / Var(g_pair) if Var(g_pair)>0, else 0
    double beta = 0.0;
    if (var_gpair > 0.0) {
        beta = cov_f_g / var_gpair;
    }

    // 9) Known expectation: E_uniform[g] = ∫∫(x^2+y^2) dx dy = 1/3 + 1/3 = 2/3
    constexpr double E_g = 2.0 / 3.0;

    // 10) Build outputs: for each i, h_i = f_pair[i] + β·(2/3 - g_pair[i])
    outputs.clear();
    outputs.reserve(static_cast<size_t>(M));
    for (int i = 0; i < M; ++i) {
        double hi = f_pair[i] + beta * (E_g - g_pair[i]);
        // Use (u_i, v_i) = (us[i], vs[i]) as the stored coordinates
        outputs.push_back(Sample{ us[i], vs[i], hi });
    }

    // Done: outputs.size() == M = floor(n/2).  Total f‐calls = 2*M (≈ n).
}
