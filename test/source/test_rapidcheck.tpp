#include <doctest/doctest.h>

#include <cmath>
#include <lds/lds.hpp>

#ifdef RAPIDCHECK_H
#    include <rapidcheck.h>

TEST_CASE("Property-based test: VdCorput values in [0,1]") {
    rc::check("vdc(n, base) returns values in [0, 1]", []() {
        auto base = static_cast<unsigned long>(*rc::gen::inRange(2, 101));
        for (unsigned long i = 0; i < 100; ++i) {
            auto val = lds::vdc(i, base);
            RC_ASSERT(val >= 0.0 && val < 1.0);
        }
    });
}

TEST_CASE("Property-based test: VdCorput sequence is deterministic") {
    rc::check("VdCorput produces same sequence after reseed", []() {
        auto base = static_cast<unsigned long>(*rc::gen::inRange(2, 51));
        auto seed = static_cast<unsigned long>(*rc::gen::inRange(0, 1000));

        lds::VdCorput gen1(base);
        lds::VdCorput gen2(base);

        gen1.reseed(seed);
        gen2.reseed(seed);

        for (unsigned long i = 0; i < 10; ++i) {
            RC_ASSERT(gen1.pop() == gen2.pop());
        }
    });
}

TEST_CASE("Property-based test: VdCorput generates values in [0,1)") {
    lds::VdCorput gen(2);
    for (unsigned long i = 0; i < 100; ++i) {
        auto curr = gen.pop();
        CHECK_GE(curr, 0.0);
        CHECK_LT(curr, 1.0);
    }
}

TEST_CASE("Property-based test: Circle points on unit circle") {
    rc::check("All Circle points lie on unit circle", []() {
        auto base = static_cast<unsigned long>(*rc::gen::inRange(2, 101));
        lds::Circle gen(base);

        for (unsigned long i = 0; i < 50; ++i) {
            auto point = gen.pop();
            double radius_squared = point[0] * point[0] + point[1] * point[1];
            RC_ASSERT(radius_squared == doctest::Approx(1.0));
        }
    });
}

TEST_CASE("Property-based test: Disk points inside unit disk") {
    rc::check("All Disk points lie inside unit disk", []() {
        auto base0 = static_cast<unsigned long>(*rc::gen::inRange(2, 51));
        auto base1 = static_cast<unsigned long>(*rc::gen::inRange(2, 51));
        lds::Disk gen(base0, base1);

        for (unsigned long i = 0; i < 50; ++i) {
            auto point = gen.pop();
            double radius_squared = point[0] * point[0] + point[1] * point[1];
            RC_ASSERT(radius_squared >= 0.0 && radius_squared <= 1.0);
        }
    });
}

TEST_CASE("Property-based test: Sphere points on unit sphere") {
    rc::check("All Sphere points lie on unit sphere", []() {
        auto base0 = static_cast<unsigned long>(*rc::gen::inRange(2, 51));
        auto base1 = static_cast<unsigned long>(*rc::gen::inRange(2, 51));
        lds::Sphere gen(base0, base1);

        for (unsigned long i = 0; i < 50; ++i) {
            auto point = gen.pop();
            double radius_squared = point[0] * point[0] + point[1] * point[1] + point[2] * point[2];
            RC_ASSERT(radius_squared == doctest::Approx(1.0));
        }
    });
}

TEST_CASE("Property-based test: Halton points in [0,1]^2") {
    rc::check("All Halton points lie in unit square [0,1]x[0,1]", []() {
        auto base0 = static_cast<unsigned long>(*rc::gen::inRange(2, 51));
        auto base1 = static_cast<unsigned long>(*rc::gen::inRange(2, 51));
        lds::Halton gen(base0, base1);

        for (unsigned long i = 0; i < 50; ++i) {
            auto point = gen.pop();
            RC_ASSERT(point[0] >= 0.0 && point[0] < 1.0);
            RC_ASSERT(point[1] >= 0.0 && point[1] < 1.0);
        }
    });
}

TEST_CASE("Property-based test: Sphere3Hopf points on unit 3-sphere") {
    rc::check("All Sphere3Hopf points lie on unit 3-sphere", []() {
        auto base0 = static_cast<unsigned long>(*rc::gen::inRange(2, 31));
        auto base1 = static_cast<unsigned long>(*rc::gen::inRange(2, 31));
        auto base2 = static_cast<unsigned long>(*rc::gen::inRange(2, 31));
        lds::Sphere3Hopf gen(base0, base1, base2);

        for (unsigned long i = 0; i < 50; ++i) {
            auto point = gen.pop();
            double radius_squared = point[0] * point[0] + point[1] * point[1] + point[2] * point[2]
                                    + point[3] * point[3];
            RC_ASSERT(radius_squared == doctest::Approx(1.0));
        }
    });
}

TEST_CASE("Property-based test: peek does not advance state") {
    rc::check("peek() returns same value as subsequent pop()", []() {
        auto base = static_cast<unsigned long>(*rc::gen::inRange(2, 51));
        lds::VdCorput gen(base);

        for (unsigned long i = 0; i < 10; ++i) {
            auto peeked = gen.peek();
            auto popped = gen.pop();
            RC_ASSERT(peeked == popped);
        }
    });
}

TEST_CASE("Property-based test: skip advances state correctly") {
    rc::check("skip(n) advances state by n positions", []() {
        auto base = static_cast<unsigned long>(*rc::gen::inRange(2, 51));
        auto skip_count = static_cast<unsigned long>(*rc::gen::inRange(0, 100));

        lds::VdCorput gen1(base);
        lds::VdCorput gen2(base);

        gen2.skip(skip_count);

        for (unsigned long i = 0; i < skip_count; ++i) {
            gen1.pop();
        }

        RC_ASSERT(gen1.pop() == gen2.pop());
    });
}

TEST_CASE("Property-based test: reseed resets generator state") {
    rc::check("reseed(n) produces deterministic sequence", []() {
        auto base = static_cast<unsigned long>(*rc::gen::inRange(2, 51));
        auto seed = static_cast<unsigned long>(*rc::gen::inRange(0, 1000));

        lds::VdCorput gen1(base);
        lds::VdCorput gen2(base);

        gen1.reseed(seed);
        gen2.reseed(seed);

        for (unsigned long i = 0; i < 10; ++i) {
            RC_ASSERT(gen1.pop() == gen2.pop());
        }
    });
}

TEST_CASE("Property-based test: iterator produces same values as pop") {
    rc::check("Iterator produces same sequence as pop()", []() {
        auto base = static_cast<unsigned long>(*rc::gen::inRange(2, 51));

        lds::VdCorput gen1(base);
        lds::VdCorput gen2(base);

        auto it = gen1.begin();
        for (unsigned long i = 0; i < 50; ++i, ++it) {
            RC_ASSERT(*it == gen2.pop());
        }
    });
}

TEST_CASE("Property-based test: Circle iterator produces points on unit circle") {
    rc::check("Circle iterator produces points on unit circle", []() {
        auto base = static_cast<unsigned long>(*rc::gen::inRange(2, 51));

        lds::Circle gen(base);
        auto it = gen.begin();

        for (unsigned long i = 0; i < 50; ++i, ++it) {
            auto point = *it;
            double radius_squared = point[0] * point[0] + point[1] * point[1];
            RC_ASSERT(radius_squared == doctest::Approx(1.0));
        }
    });
}

TEST_CASE("Property-based test: Halton iterator produces points in unit square") {
    rc::check("Halton iterator produces points in [0,1]^2", []() {
        auto base0 = static_cast<unsigned long>(*rc::gen::inRange(2, 51));
        auto base1 = static_cast<unsigned long>(*rc::gen::inRange(2, 51));

        lds::Halton gen(base0, base1);
        auto it = gen.begin();

        for (unsigned long i = 0; i < 50; ++i, ++it) {
            auto point = *it;
            RC_ASSERT(point[0] >= 0.0 && point[0] < 1.0);
            RC_ASSERT(point[1] >= 0.0 && point[1] < 1.0);
        }
    });
}

TEST_CASE("Property-based test: get_index reflects correct position") {
    rc::check("get_index() returns current sequence position", []() {
        auto base = static_cast<unsigned long>(*rc::gen::inRange(2, 51));

        lds::VdCorput gen(base);
        RC_ASSERT(gen.get_index() == static_cast<unsigned long>(0));

        for (unsigned long i = 0; i < 100; ++i) {
            gen.pop();
            RC_ASSERT(gen.get_index() == i + static_cast<unsigned long>(1));
        }
    });
}

TEST_CASE("Property-based test: reseed and get_index consistency") {
    rc::check("reseed(n) sets index to n", []() {
        auto base = static_cast<unsigned long>(*rc::gen::inRange(2, 51));
        auto seed = static_cast<unsigned long>(*rc::gen::inRange(0, 1000));

        lds::VdCorput gen(base);
        gen.reseed(seed);

        RC_ASSERT(gen.get_index() == seed);

        gen.pop();
        RC_ASSERT(gen.get_index() == seed + static_cast<unsigned long>(1));
    });
}

#endif