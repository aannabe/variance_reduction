#include "antithetic_engine.h"
#include <cmath>     // for the inside‐circle test: x*x + y*y ≤ 1
#include <random>    // for std::random_device, std::uniform_real_distribution

AntitheticEngine::AntitheticEngine() {
    // Seed the RNG once at construction
    std::random_device rd;
    rng = std::mt19937(rd());
}

AntitheticEngine::~AntitheticEngine() { }

void AntitheticEngine::sample(int n, std::vector<Sample>& outputs) {
    // 1) Determine how many full antithetic pairs we can form:
    //    If n is even, pairs = n/2; if n is odd, pairs = (n-1)/2.
    int pairs = n / 2;  // integer division automatically drops 0.5 if n is odd

    // 2) Prepare the output vector to hold exactly `pairs` Samples
    outputs.clear();
    outputs.reserve(static_cast<size_t>(pairs));

    // 3) We'll draw (u,v) ∼ Uniform([0,1]^2) for each pair
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    // 4) Loop over each of the `pairs`:
    for (int i = 0; i < pairs; ++i) {
        // 4a) Draw one Uniform‐pair (u,v)
        double u = dist(rng);
        double v = dist(rng);

        // 4b) Compute f(u,v) = 4·I{u^2 + v^2 ≤ 1}
        bool inside1 = (u * u + v * v) <= 1.0;
        double f1 = inside1 ? 4.0 : 0.0;

        // 4c) Form the antithetic partner (u2,v2) = (1-u, 1-v)
        double u2 = 1.0 - u;
        double v2 = 1.0 - v;

        // 4d) Compute f(u2,v2) = 4·I{u2^2 + v2^2 ≤ 1}
        bool inside2 = (u2 * u2 + v2 * v2) <= 1.0;
        double f2 = inside2 ? 4.0 : 0.0;

        // 4e) Average the two values
        double avg = 0.5 * (f1 + f2);

        // 4f) Store one Sample, using (u,v) for coordinates, and avg for value
        outputs.push_back(Sample{ u, v, avg });
    }

    // If n was odd, we simply ignore the “last” unpaired request.
    // Hence outputs.size() == ⌊n/2⌋.
}
