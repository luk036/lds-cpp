/**
 * Simple example demonstrating spdlogger integration in lds-cpp
 *
 * This example shows how to use the lds::log_with_spdlog() function
 * to log messages to a file.
 *
 * To build and run:
 *   xmake build test_final_spdlogger
 *   xmake run test_final_spdlogger
 */

#include <iostream>
#include <lds/lds.hpp>
#include <lds/logger.hpp>

int main() {
    std::cout << "Lds Spdlogger Example\n";
    std::cout << "=========================\n";

    // Example 1: Basic logging
    std::cout << "\nExample 1: Basic logging\n";
    lds::log_with_spdlog("Application started");
    lds::log_with_spdlog("Processing data...");

    // Example 2: Logging with sequence generation
    std::cout << "\nExample 2: Logging with sequence generation\n";
    lds::VdCorput<2> vdc{};
    for (int i = 0; i < 5; ++i) {
        auto point = vdc.pop();
        std::cout << "  Generated point: " << point << '\n';
    }
    lds::log_with_spdlog("Sequence generation completed");

    std::cout << "\nCheck lds.log for logged messages\n";

    return 0;
}