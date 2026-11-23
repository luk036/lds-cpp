add_rules("mode.debug", "mode.release", "mode.coverage")
add_requires("doctest", "fmt", "nanobench")

set_languages("c++20")
add_includedirs("include")

if is_mode("coverage") then
    add_cxflags("-ftest-coverage", "-fprofile-arcs", {force = true})
end

if is_plat("linux") then
    add_cxflags("-Wconversion", {force = true})
    add_syslinks("pthread")
    -- Termux/Android ships its libc++ headers under $PREFIX
    local prefix = os.getenv("PREFIX")
    if prefix then
        add_cxflags("-Wno-unused-command-line-argument", {force = true})
        add_sysincludedirs(prefix .. "/include/c++/v1", prefix .. "/include", {public = true})
    end
elseif is_plat("windows") then
    add_cxflags("/EHsc /utf-8 /W4 /WX /wd4702", {force = true})
end

target("Lds")
    set_kind("static")
    add_includedirs("include", {public = true})
    add_files("source/*.cpp")

-- keep in sync with LDS_BUILD_BENCHMARKS=OFF in CMakeLists.txt
target("bench_all")
    set_default(false)
    add_deps("Lds")
    add_files("bench/BM_all.cpp")
    add_packages("nanobench")

target("verify_basic")
    add_deps("Lds")
    add_files("examples/verify_basic.cpp")
    add_packages("fmt")

target("test_lds")
    add_deps("Lds")
    add_files("test/source/*.cpp")
    add_packages("doctest")
    add_tests("default")
