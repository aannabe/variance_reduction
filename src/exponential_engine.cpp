#include "exponential_engine.h"
#include <cmath>
#include <random>

ExponentialEngine::ExponentialEngine(double lambda_)
    : lambda(lambda_)
{
    std::random_device rd;
    rng = std::mt19937(rd());
}

ExponentialEngine::~ExponentialEngine() {}

void ExponentialEngine::sample(int samples, std::vector<Sample>& outputs) {
    // PDF p(x,y) = [λ e^{-λx}/(1-e^{-λ})] * [λ e^{-λy}/(1-e^{-λ})]
    // so weight = 4·I[x^2+y^2≤1] / p(x,y)

    outputs.clear();
    outputs.reserve(static_cast<size_t>(samples));

    std::uniform_real_distribution<double> uni(0.0, 1.0);
    double Z = 1.0 - std::exp(-lambda); 
    // normalizing factor for each coordinate over [0,1] is (1 - e^{-λ})

    for (int i = 0; i < samples; ++i) {
        // draw x via inverse‐cdf of truncated exp(λ) on [0,1]
        double U1 = uni(rng);
        double x  = -std::log(1.0 - U1 * Z) / lambda;

        // draw y similarly
        double U2 = uni(rng);
        double y  = -std::log(1.0 - U2 * Z) / lambda;

        Sample s;
        s.x = x;
        s.y = y;

        // indicator for circle
        if (x*x + y*y <= 1.0) {
            // p(x,y) = [λ e^{-λx}/Z]·[λ e^{-λy}/Z] = λ^2 e^{-λ(x+y)} / Z^2
            double pxy = (lambda * lambda * std::exp(-lambda * (x + y))) / (Z * Z);
            // f(x,y) = 4·1
            s.value = 4.0 / pxy;
        }
        else {
            s.value = 0.0;
        }

        outputs.push_back(s);
    }
}
