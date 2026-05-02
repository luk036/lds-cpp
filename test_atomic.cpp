#include <iostream>
#include <lds/lds.hpp>
#include <thread>
#include <vector>

int main() {
    // Test that VdCorput compiles with atomic changes
    lds::VdCorput vgen(2);

    std::cout << "Testing atomic VdCorput:\n";
    for (int i = 0; i < 5; ++i) {
        std::cout << "pop() = " << vgen.pop() << '\n';
    }

    vgen.reseed(0);
    std::cout << "\nAfter reseed(0):\n";
    for (int i = 0; i < 3; ++i) {
        std::cout << "pop() = " << vgen.pop() << '\n';
    }

    // Test thread safety
    std::cout << "\nTesting thread safety...\n";
    lds::VdCorput sharedVgen(3);
    std::vector<std::thread> threads;
    std::vector<double> results;

    for (int t = 0; t < 4; ++t) {
        threads.emplace_back([&sharedVgen, &results]() {
            for (int i = 0; i < 3; ++i) {
                results.emplace_back(sharedVgen.pop());
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Got " << results.size() << " results from threads\n";

    return 0;
}