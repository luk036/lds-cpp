#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <iostream>

int main() {
    std::cout << "Testing spdlogger directly...\n";

    try {
        // Create a simple file logger
        auto logger = spdlog::basic_logger_mt("basic_logger", "test_spdlog.log");
        logger->set_level(spdlog::level::info);

        // Log some messages
        logger->info("Hello from spdlog!");
        logger->info("This is a test message");
        logger->info("Testing line 3");

        // Flush to ensure messages are written
        logger->flush();

        std::cout << "Messages logged to test_spdlog.log\n";
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Log init failed: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}