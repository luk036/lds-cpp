add_rules("mode.debug", "mode.release")

target("test_spdlogger")
    set_languages("c++20")
    set_kind("binary")
    add_files("test_spdlogger.cpp")
    add_files("source/logger.cpp")
    add_includedirs("include", {public = true})
    add_packages("spdlog", "fmt")