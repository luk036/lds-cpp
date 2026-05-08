#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <lds/logger.hpp>

int main() {
    std::cout << "Testing spdlogger integration...\n";

    // Test basic logging functionality
    std::cout << "Logging first message...\n";
    lds::log_with_spdlog("Starting spdlogger test");

    std::cout << "Logged a message to lds.log (in build directory)\n";

    // Test with different messages
    std::cout << "Logging second message...\n";
    lds::log_with_spdlog("Testing message 2");

    std::cout << "Logging third message...\n";
    lds::log_with_spdlog("Testing message 3");

    std::cout << R"(Check build\windows\x64\debug\lds.log for the logged messages)" << '\n';

    // Also test direct spdlog usage
    std::cout << "\nTesting direct spdlog usage...\n";
    auto direct_logger = spdlog::basic_logger_mt("direct_logger", "direct_spdlog.log");
    direct_logger->info("Direct spdlog message 1");
    direct_logger->info("Direct spdlog message 2");
    direct_logger->flush();
    std::cout << "Direct spdlog messages logged to direct_spdlog.log\n";

    std::cout << "\nAll tests completed successfully!\n";

    return 0;
}