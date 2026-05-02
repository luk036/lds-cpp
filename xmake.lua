add_rules("mode.debug", "mode.release", "mode.coverage")
add_requires("doctest", {alias = "doctest"})
add_requires("fmt", {alias = "fmt"})
add_requires("spdlog", {alias = "spdlog"})

if is_mode("coverage") then
    add_cxflags("-ftest-coverage", "-fprofile-arcs", {force = true})
end

if is_plat("linux") then
    add_cxflags("-Wconversion", {force = true})
    add_cxflags("-Wno-unused-command-line-argument", {force = true})
    -- Check if we're on Termux/Android
    local termux_prefix = os.getenv("PREFIX")
    if termux_prefix then
        add_sysincludedirs(termux_prefix .. "/include/c++/v1", {public = true})
        add_sysincludedirs(termux_prefix .. "/include", {public = true})
    end
elseif is_plat("windows") then
    add_cxflags("/EHsc /W4 /WX /wd4459", {force = true})
end

target("Lds")
    set_languages("c++20")
    set_kind("static")
    add_includedirs("include", {public = true})
    add_files("source/*.cpp")
    add_packages("fmt", "spdlog")
    if is_plat("linux") then
        add_syslinks("pthread")
    end

target("test_lds")
    set_languages("c++20")
    set_kind("binary")
    add_deps("Lds")
    add_files("test/source/*.cpp")
    add_packages("doctest", "fmt", "spdlog")
    add_tests("default")
    if is_plat("linux") then
        add_syslinks("pthread")
    end

target("test_spdlogger")
    set_languages("c++20")
    set_kind("binary")
    add_files("test_spdlogger.cpp")
    add_files("source/logger.cpp")
    add_includedirs("include", {public = true})
    add_packages("spdlog", "fmt")

target("test_spdlogger_simple")
    set_languages("c++20")
    set_kind("binary")
    add_files("test_spdlogger_simple.cpp")
    add_packages("spdlog")

target("test_spdlogger_debug")
    set_languages("c++20")
    set_kind("binary")
    add_files("test_spdlogger_debug.cpp")
    add_files("source/logger.cpp")
    add_includedirs("include", {public = true})
    add_packages("spdlog", "fmt")

target("test_logger_diagnostic")
    set_languages("c++20")
    set_kind("binary")
    add_files("test_logger_diagnostic.cpp")
    add_packages("spdlog")

target("test_final_spdlogger")
    set_languages("c++20")
    set_kind("binary")
    add_files("test_final_spdlogger.cpp")
    add_files("source/logger.cpp")
    add_includedirs("include", {public = true})
    add_packages("spdlog", "fmt")

target("test_wrapper_only")
    set_languages("c++20")
    set_kind("binary")
    add_files("test_wrapper_only.cpp")
    add_files("source/logger.cpp")
    add_includedirs("include", {public = true})
    add_packages("spdlog", "fmt")

    -- Check if rapidcheck was downloaded by CMake
    local rapidcheck_dir = path.join(os.projectdir(), "build", "_deps", "rapidcheck-src")
    local rapidcheck_lib_dir = path.join(os.projectdir(), "build", "_deps", "rapidcheck-build")
    if is_plat("windows") then
        rapidcheck_lib_dir = path.join(rapidcheck_lib_dir, "Release")
    end

    if os.isdir(rapidcheck_dir) and os.isdir(rapidcheck_lib_dir) then
        add_includedirs(path.join(rapidcheck_dir, "include"))
        add_linkdirs(rapidcheck_lib_dir)
        add_links("rapidcheck")
        add_defines("RAPIDCHECK_H")
    end

-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro defination
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

