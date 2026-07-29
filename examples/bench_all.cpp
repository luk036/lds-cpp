#include <chrono>
#include <cmath>
#include <cstdio>
#include <lds/lds.hpp>

template <typename F> void bench(const char* name, F&& f, int iterations) {
    for (int i = 0; i < 1000; ++i) f();
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; ++i) f();
    auto end = std::chrono::steady_clock::now();
    auto ns = std::chrono::duration<double, std::nano>(end - start).count() / iterations;
    std::printf("  %-35s %8.1f ns/op  (%d iters)\n", name, ns, iterations);
}

int main() {
    std::printf("=== lds-cpp (compile-time template) - Full Benchmark ===\n\n");

    {
        lds::VdCorput<2> v;
        v.reseed(0);
        bench(
            "VdCorput<2> [double]",
            [&]() {
                volatile auto r = v.pop();
                (void)r;
            },
            1000000);
    }
    {
        lds::Halton<2, 3> h;
        h.reseed(0);
        bench(
            "Halton<2,3> [double;2]",
            [&]() {
                volatile auto r = h.pop();
                (void)r;
            },
            500000);
    }
    {
        lds::Circle<2> c;
        c.reseed(0);
        bench(
            "Circle<2> [double;2]",
            [&]() {
                volatile auto r = c.pop();
                (void)r;
            },
            500000);
    }
    {
        lds::Disk<2, 3> d;
        d.reseed(0);
        bench(
            "Disk<2,3> [double;2]",
            [&]() {
                volatile auto r = d.pop();
                (void)r;
            },
            500000);
    }
    {
        lds::Sphere<2, 3> s;
        s.reseed(0);
        bench(
            "Sphere<2,3> [double;3]",
            [&]() {
                volatile auto r = s.pop();
                (void)r;
            },
            500000);
    }
    {
        lds::Sphere3Hopf<2, 3, 5> h3;
        h3.reseed(0);
        bench(
            "Sphere3Hopf<2,3,5> [double;4]",
            [&]() {
                volatile auto r = h3.pop();
                (void)r;
            },
            500000);
    }
}
