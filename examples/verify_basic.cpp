#include <cmath>
#include <cstdio>
#include <lds/lds.hpp>

int main() {
    std::printf("=== lds-cpp (template) Basic Verification ===\n\n");

    // 1. VdCorput base 2
    std::printf("1. van der Corput sequence (base 2):\n");
    lds::VdCorput<2> vgen;
    vgen.reseed(0);
    for (int i = 0; i < 5; ++i) std::printf("  Value %d: %.15f\n", i + 1, vgen.pop());

    // 2. Halton bases [2,3]
    std::printf("\n2. Halton sequence (bases [2, 3]):\n");
    lds::Halton<2, 3> hgen;
    hgen.reseed(0);
    for (int i = 0; i < 3; ++i) {
        auto p = hgen.pop();
        std::printf("  Point %d: [%.15f, %.15f]\n", i + 1, p[0], p[1]);
    }

    // 3. Circle base 2
    std::printf("\n3. Points on unit circle (base 2):\n");
    lds::Circle<2> cgen;
    cgen.reseed(0);
    for (int i = 0; i < 3; ++i) {
        auto p = cgen.pop();
        std::printf("  Point %d: [%.15f, %.15f]\n", i + 1, p[0], p[1]);
    }

    // 4. Disk bases [2,3]
    std::printf("\n4. Points in unit disk (bases [2, 3]):\n");
    lds::Disk<2, 3> dgen;
    dgen.reseed(0);
    for (int i = 0; i < 3; ++i) {
        auto p = dgen.pop();
        auto r = std::sqrt(p[0] * p[0] + p[1] * p[1]);
        std::printf("  Point %d: [%.15f, %.15f] (radius: %.15f)\n", i + 1, p[0], p[1], r);
    }

    // 5. Sphere bases [2,3]
    std::printf("\n5. Points on unit sphere (bases [2, 3]):\n");
    lds::Sphere<2, 3> sgen;
    sgen.reseed(0);
    for (int i = 0; i < 3; ++i) {
        auto p = sgen.pop();
        auto r = std::sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]);
        std::printf("  Point %d: [%.15f, %.15f, %.15f] (radius: %.15f)\n", i + 1, p[0], p[1], p[2],
                    r);
    }

    // 6. Sphere3Hopf bases [2,3,5]
    std::printf("\n6. Sphere3 Hopf (bases [2, 3, 5]):\n");
    lds::Sphere3Hopf<2, 3, 5> s3hgen;
    s3hgen.reseed(0);
    for (int i = 0; i < 3; ++i) {
        auto p = s3hgen.pop();
        auto r = std::sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3]);
        std::printf("  Point %d: [%.15f, %.15f, %.15f, %.15f] (radius: %.15f)\n", i + 1, p[0], p[1],
                    p[2], p[3], r);
    }

    return 0;
}
