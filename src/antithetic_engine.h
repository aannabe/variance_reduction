#ifndef ANTITHETIC_ENGINE_H
#define ANTITHETIC_ENGINE_H

#include "engine.h"     // brings in `struct Sample { double x, y, value; };`
#include <random>       // for std::mt19937

// AntitheticEngine:  
//   - If the user asks for N samples and N is odd, we round up to N_even = N+1 (an even number).
//   - We generate N_even/2 i.i.d. uniforms (u,v), then for each pair we also include (1-u,1-v).
//   - That produces N_even total points, all of which get tested against the quarter‐circle.
//   - Each Sample.value = 4·I{x^2 + y^2 ≤ 1}.
//   - Because we round up, actual_samples = N_even ≥ requested_samples.
//
// Example: if the user writes “SAMPLES = 7”, we actually draw 8 points, in 4 antithetic pairs.
//
// No duplicated “inside‐circle” logic: we factor the test out into a little helper lambda.
class AntitheticEngine : public Engine {
public:
    // Constructor: seed the RNG
    AntitheticEngine();

    // Destructor: nothing special
    ~AntitheticEngine();

    // sample():  
    //   * Round requested `samples` up to an even integer N_even.  
    //   * Draw N_even/2 independent uniforms (u,v).  
    //   * For each (u,v), append Sample{u, v, value} and Sample{1-u, 1-v, value}.  
    //   * Each value = 4.0 if point is inside x^2+y^2 ≤ 1, else 0.0.
    void sample(int samples, std::vector<Sample>& outputs) override;

private:
    std::mt19937 rng;  // Mersenne Twister PRNG
};

#endif // ANTITHETIC_ENGINE_H
