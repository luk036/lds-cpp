#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

#include <lds/lds.hpp>

int main() {
    {
        ankerl::nanobench::Bench bench;
        bench.title("lds-cpp (compile-time template) - Full Benchmark")
            .unit("op")
            .warmup(100)
            .epochs(50)
            .minEpochIterations(200000);

        lds::VdCorput<2> v;
        v.reseed(0);
        bench.run("VdCorput<2> [double]", [&] {
            auto r = v.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });

        lds::Halton<2, 3> h;
        h.reseed(0);
        bench.run("Halton<2,3> [double;2]", [&] {
            auto r = h.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });

        lds::Circle<2> c;
        c.reseed(0);
        bench.run("Circle<2> [double;2]", [&] {
            auto r = c.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });

        lds::Disk<2, 3> d;
        d.reseed(0);
        bench.run("Disk<2,3> [double;2]", [&] {
            auto r = d.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });

        lds::Sphere<2, 3> s;
        s.reseed(0);
        bench.run("Sphere<2,3> [double;3]", [&] {
            auto r = s.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });

        lds::Sphere3Hopf<2, 3, 5> h3;
        h3.reseed(0);
        bench.run("Sphere3Hopf<2,3,5> [double;4]", [&] {
            auto r = h3.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });
    }
}
