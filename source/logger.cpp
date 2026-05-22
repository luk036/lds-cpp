#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <lds/logger.hpp>

namespace lds {

void log_with_spdlog(const std::string& message) {
    // Function-local static: created once on first call, reused thereafter
    static auto logger = []() -> std::shared_ptr<spdlog::logger> {
        auto log = spdlog::basic_logger_mt("file_logger", "lds.log");
        log->set_level(spdlog::level::info);
        log->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
        log->flush_on(spdlog::level::info);
        return log;
    }();
    logger->info("Lds message: {}", message);
    logger->flush();
}

}  // namespace lds