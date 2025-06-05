#include "antithetic_engine.h"
#include <cmath>     // for std::sqrt (inside‐circle test)
#include <random>    // for std::random_device, std::uniform_real_distribution

AntitheticEngine::AntitheticEngine() {
    // Seed the Mersenne Twister with a nondeterministic random_device
    std::random_device rd;
    rng = std::mt19937(rd());
}

AntitheticEngine::~AntitheticEngine() { }

void AntitheticEngine::sample(int samples, std::vector<Sample>& outputs) {
    // 1) Round `samples` up to the next even integer (if needed)
    int N_even = (samples % 2 == 0) ? samples : (samples + 1);

    // 2) We will draw N_even/2 uniforms and mirror each
    int half = N_even / 2;

    // 3) Prepare the output vector
    outputs.clear();
    outputs.reserve(static_cast<size_t>(N_even));

    // 4) Uniform[0,1) distribution for generating u,v
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    // 5) Helper lambda to test “inside quarter‐circle” and append a Sample
    auto append_sample = [&](double x, double y) {
        // Check if (x,y) lies inside x^2 + y^2 ≤ 1
        bool inside = (x * x + y * y) <= 1.0;
        // If inside, value = 4; otherwise 0
        double val = inside ? 4.0 : 0.0;
        outputs.push_back(Sample{ x, y, val });
    };

    // 6) Generate `half` i.i.d. uniforms, then mirror each
    for (int i = 0; i < half; ++i) {
        // Draw one uniform pair (u,v) in [0,1]^2
        double u = dist(rng);
        double v = dist(rng);

        // Append the “original” sample
        append_sample(u, v);

        // Append its antithetic partner (1-u, 1-v)
        append_sample(1.0 - u, 1.0 - v);
    }

    // At this point, outputs.size() == N_even.
    // If the user had requested an odd `samples`, we have actually generated one extra.
    // We leave that extra in `outputs`, and main() will simply see actual_samples = N_even.
}
