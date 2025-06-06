Variance Reduction Techniques in Monte Carlo: Sampling Pi via Circle-in-a-Square
==================================================================

## Variance Reduction Techniques (Engines):

- **Random**  
Plain IID draws $(x,y) \sim U([0,1]^2)$, $f = 4 \cdot I[x^2+y^2≤1]$.

- **Stratified**  
Partition $[0,1]^2$ into an $(m \times m)$ grid (where $m^2 \approx N$), draw one random point per cell, and average.

- **Exponential**  
Importance sampling via an exponential function: $p(x, y) = λ^2 e^{-\lambda(x+y)} / (1 - e^{-\lambda})^2$. $\lambda = 0.6$ is hard-coded.

- **ControlVariate**  
Uniform draws + control variate $g=x^2+y^2$. Adjusts each $f_i$ by $\beta (2/3 − g_i)$ to reduce variance.

- **Antithetic**  
Forms antithetic pairs $[(x,y),(1−x,1−y)]$. Returns floor($N/2$) samples, each sample’s value = $(f_1+f_2) / 2$.

- **ControlAntithetic**  
Antithetic pairs + control variate. For each pair, calculate $f_{avg} = (f_1 + f_2) / 2$ and $g_{avg} = (g_1 + g_2) / 2$, compute $\beta$, and adjust $f_{avg}$ by $\beta (2/3 − g_{avg})$.

## Variance Comparisons:
Using $N = 25 \times 10^6$ samples:
| Method                  | Variance |
| ----------------------- | -------- |
| Random                  | 2.6966   |
| Stratified              | 2.6968   |
| Exponential             | 2.1597   |
| ControlVariate          | 1.1548   |
| Antithetic              | 0.9799   |
| ControlAntithetic       | 0.7665   |

For this specific problem, the Control Variate technique, combined with Antithetic pair samples, appears to yield the best variance.
The Stratified method did not result in any resolvable improvement in variance.

## To Build (C++14, CMake 3.10 required):
```
mkdir build
cd build
cmake ..
make
```
## To Run:
```
cd build
cp ../examples/input.in . # Adjust the input as needed
./monte_carlo_pi
```

## Output
`results.log` will include running statistics for the number of samples, $(x, y)$ sample coordinates, sample value, and running values for mean, variance, and standard error.
