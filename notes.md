# Low Discrepancy Sequences (LDS)

## Background

van der Corput sequence (base: b = 2), [0, 1], 1935
uniform, deterministic, incremental
rand()

Sobol, ...
Halton (b1, b2, b3, ...) [0, 1]^n 
- QMC (Quasi Monte Carlo) -> Numerical Integration
- spare TSV 

## Mappings

LDS on unit Sphere (S^2)
- Graphic application

LDS on unit Circle (S^1)
- Initial guess for Aberth's method

Grid points on SO(3), S^3 (2008, 2010), Hopf coordinate
- Robotic application

LDS on unit n-Sphere (S^n)
- Filter bank design
- MIMO wireless
- normalized training: p1 + p2 + ... + pn = 1

## Implementation

base 2: reverse all the bits
base 3 = 4 - 1, base 7 = 8 - 1
9
CORDIC = smart table lookup

Table lookup (64)
thread-safety, atomic (lock-free) lds-gen-cpp
C++: constexpr lds-cpp

