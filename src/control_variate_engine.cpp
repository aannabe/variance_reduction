#include "control_variate_engine.h"
#include <cmath>       // for std::sqrt
#include <random>      // for std::random_device, std::uniform_real_distribution
#include <vector>      // for std::vector
#include <numeric>     // for std::accumulate

// Constructor: seed the RNG with a nondeterministic seed from std::random_device
ControlVariateEngine::ControlVariateEngine() {
    std::random_device rd;             // Obtain a random seed from the OS
    rng = std::mt19937(rd());          // Seed the Mersenne Twister
}

// Destructor: no dynamic resources, so default is fine
ControlVariateEngine::~ControlVariateEngine() {}

// sample(): perform control‐variates to estimate π
void ControlVariateEngine::sample(int samples, std::vector<Sample>& outputs) {
    // 1) We will draw `samples` i.i.d. Uniform(0,1)^2 points.
    //    For each, compute f_i = 4·I{x^2 + y^2 ≤ 1} and g_i = x^2 + y^2.
    //    Then form h_i = f_i + β·(2/3 – g_i), where
    //       β = Cov(f,g)/Var(g),   E[g] = 2/3.
    //
    //    At the end, we push (x_i, y_i, h_i) into `outputs`.

    // 2) Prepare distributions and storage vectors
    std::uniform_real_distribution<double> dist(0.0, 1.0);  
    // dist(rng) generates U~Uniform(0,1)

    // We'll store x_i, y_i in parallel arrays, plus f_i and g_i
    std::vector<double> xs(samples), ys(samples);
    std::vector<double> fs(samples), gs(samples);

    // 3) Draw all samples and compute f_i, g_i
    for (int i = 0; i < samples; ++i) {
        double x = dist(rng);            // draw x ∈ [0,1]
        double y = dist(rng);            // draw y ∈ [0,1]

        // f_i = 4·I{x^2 + y^2 ≤ 1}
        bool inside = (x * x + y * y) <= 1.0;
        double fi = inside ? 4.0 : 0.0;

        // g_i = x^2 + y^2
        double gi = x * x + y * y;

        xs[i] = x;
        ys[i] = y;
        fs[i] = fi;
        gs[i] = gi;
    }

    // 4) Compute sample means bar_f and bar_g:
    //    bar_f = (1/N) ∑ f_i,   bar_g = (1/N) ∑ g_i
    double sum_f = std::accumulate(fs.begin(), fs.end(), 0.0);
    double sum_g = std::accumulate(gs.begin(), gs.end(), 0.0);
    double bar_f = sum_f / static_cast<double>(samples);
    double bar_g = sum_g / static_cast<double>(samples);

    // 5) Compute sample covariance Cov(f,g) and variance Var(g):
    //    Cov(f,g) ≈ (1/(N-1)) ∑ (f_i - bar_f)(g_i - bar_g)
    //    Var(g)   ≈ (1/(N-1)) ∑ (g_i - bar_g)^2
    double sum_cov = 0.0;
    double sum_varg = 0.0;
    for (int i = 0; i < samples; ++i) {
        double df = fs[i] - bar_f;    // deviation of f_i
        double dg = gs[i] - bar_g;    // deviation of g_i
        sum_cov  += df * dg;          // accumulate f_i * g_i cross‐term
        sum_varg += dg * dg;          // accumulate (g_i - bar_g)^2
    }
    // Use N-1 in denominator for unbiased estimation
    double cov_fg = sum_cov / static_cast<double>(samples - 1);
    double var_g  = sum_varg / static_cast<double>(samples - 1);

    // 6) Estimate β = Cov(f,g) / Var(g).  If Var(g)=0 (degenerate), set β=0.
    double beta = 0.0;
    if (var_g > 0.0) {
        beta = cov_fg / var_g;
    }

    // 7) We know analytically E[g] = ∫₀¹ ∫₀¹ (x^2 + y^2) dx dy = 1/3 + 1/3 = 2/3
    constexpr double E_g = 2.0 / 3.0;

    // 8) Now build the adjusted values:
    //    h_i = f_i + β·(2/3 - g_i)
    //    E[h] = E[f] + β·(2/3 - E[g]) = π + β·(2/3 - 2/3) = π
    outputs.clear();
    outputs.reserve(static_cast<size_t>(samples));
    for (int i = 0; i < samples; ++i) {
        double hi = fs[i] + beta * (E_g - gs[i]);  
        // hi = f_i + β·(2/3 - g_i)

        // Push (x_i, y_i, h_i) so main.cpp can do Welford/logging as usual
        outputs.push_back(Sample{ xs[i], ys[i], hi });
    }
}
