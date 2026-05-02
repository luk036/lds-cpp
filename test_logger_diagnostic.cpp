#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Logger Diagnostic Test ===\n";

    // Test 1: Create a fresh logger
    std::cout << "\n[Test 1] Creating fresh logger...\n";
    try {
        auto logger1 = spdlog::basic_logger_mt("test1", "test1.log");
        logger1->set_level(spdlog::level::info);
        logger1->info("Test 1 message");
        logger1->flush();
        std::cout << "  - Logger created and message logged\n";
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "  - Error: " << ex.what() << '\n';
    }

    // Test 2: Try to get the same logger
    std::cout << "\n[Test 2] Getting existing logger...\n";
    try {
        auto logger2 = spdlog::get("test1");
        if (logger2) {
            logger2->info("Test 2 message");
            logger2->flush();
            std::cout << "  - Logger retrieved and message logged\n";
        } else {
            std::cout << "  - Logger not found\n";
        }
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "  - Error: " << ex.what() << '\n';
    }

    // Test 3: Check if file exists
    std::cout << "\n[Test 3] Checking file existence...\n";
    std::ifstream file("test1.log");
    if (file.good()) {
        std::cout << "  - test1.log exists\n";
        std::string line;
        int count = 0;
        while (std::getline(file, line)) {
            std::cout << "    Line " << ++count << ": " << line << '\n';
        }
    } else {
        std::cout << "  - test1.log NOT found\n";
    }

    std::cout << "\n=== Diagnostic complete ===\n";
    return 0;
}