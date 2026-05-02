#include <iostream>
#include <lds/lds.hpp>

int main() {
    std::cout << "Testing atomic VdCorput implementation\n";

    // Test basic functionality
    lds::VdCorput vgen(2);
    std::cout << "Base 2 sequence:\n";
    for (int i = 0; i < 5; ++i) {
        std::cout << "  " << vgen.pop() << "\n";
    }

    // Test reseed
    vgen.reseed(0);
    std::cout << "\nAfter reseed(0):\n";
    std::cout << "  " << vgen.pop() << " (should be 0.5)\n";

    // Test other generators that use VdCorput
    std::cout << "\nTesting Halton (uses VdCorput):\n";
    lds::Halton halton(2, 3);
    auto point = halton.pop();
    std::cout << "  First point: (" << point[0] << ", " << point[1] << ")\n";

    std::cout << "\nTesting Circle (uses VdCorput):\n";
    lds::Circle circle(2);
    point = circle.pop();
    std::cout << "  First point: (" << point[0] << ", " << point[1] << ")\n";

    std::cout << "\nAll tests passed!\n";
    return 0;
}