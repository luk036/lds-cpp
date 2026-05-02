#include <fstream>
#include <iostream>
#include <lds/logger.hpp>

int main() {
    std::cout << "Testing wrapper function only...\n";

    try {
        std::cout << "Calling lds::log_with_spdlog()...\n";
        lds::log_with_spdlog("Test message from wrapper");
        std::cout << "Function returned successfully\n";

        std::cout << "Checking if lds.log exists...\n";
        std::ifstream file("lds.log");
        if (file.good()) {
            std::cout << "File exists!\n";
            std::string line;
            while (std::getline(file, line)) {
                std::cout << "  " << line << '\n';
            }
        } else {
            std::cout << "File does NOT exist\n";
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
        return 1;
    }
}