// #include <cxxopts.hpp>
// #include <iostream>
#include <lds/lds.hpp>
#include <lds/logger.hpp>
// #include <string>
// #include <vector>

int main() {
    // cxxopts::Options options("Lds", "Low Discrepancy Sequence Generator");

    // std::string sequence_type = "vdc";
    // int count = 10;
    // int base = 2;
    // bool use_logger = false;

    // // clang-format off
    // options.add_options()
    //     ("h,help", "Show help")
    //     ("s,sequence", "Sequence type (vdc, halton, circle, disk, sphere)",
    //     cxxopts::value(sequence_type)->default_value("vdc"))
    //     ("n,count", "Number of points to generate", cxxopts::value(count)->default_value("10"))
    //     ("b,base", "Base for VdCorput sequence", cxxopts::value(base)->default_value("2"))
    //     ("l,logger", "Enable logging to file",
    //     cxxopts::value(use_logger)->default_value("false"))
    // ;
    // // clang-format on

    // auto result = options.parse(argc, argv);

    // if (result["help"].as<bool>()) {
    //     std::cout << options.help() << '\n';
    //     return 0;
    // }

    // if (use_logger) {
    //     lds::log_with_spdlog("Lds standalone started");
    //     lds::log_with_spdlog("Generating " + std::to_string(count) + " points");
    // }

    // std::cout << "Generating " << count << " points using " << sequence_type << " sequence" <<
    // '\n';

    // if (sequence_type == "vdc") {
    //     lds::VdCorput vdc(base);
    //     for (int i = 0; i < count; ++i) {
    //         auto point = vdc.pop();
    //         std::cout << "  Point " << i << ": " << point << '\n';
    //     }
    // } else if (sequence_type == "halton") {
    //     lds::Halton halton(2, 3);
    //     for (int i = 0; i < count; ++i) {
    //         auto point = halton.pop();
    //         std::cout << "  Point " << i << ": (" << point[0] << ", " << point[1] << ")" << '\n';
    //     }
    // } else if (sequence_type == "circle") {
    //     lds::Circle circle(base);
    //     for (int i = 0; i < count; ++i) {
    //         auto point = circle.pop();
    //         std::cout << "  Point " << i << ": (" << point[0] << ", " << point[1] << ")" << '\n';
    //     }
    // } else if (sequence_type == "disk") {
    //     lds::Disk disk(2, 3);
    //     for (int i = 0; i < count; ++i) {
    //         auto point = disk.pop();
    //         std::cout << "  Point " << i << ": (" << point[0] << ", " << point[1] << ")" << '\n';
    //     }
    // } else if (sequence_type == "sphere") {
    //     lds::Sphere sphere(2, 3);
    //     for (int i = 0; i < count; ++i) {
    //         auto point = sphere.pop();
    //         std::cout << "  Point " << i << ": (" << point[0] << ", " << point[1] << ", "
    //                   << point[2] << ")\n";
    //     }
    // } else {
    //     std::cerr << "Unknown sequence type: " << sequence_type << '\n';
    //     std::cerr << "Available types: vdc, halton, circle, disk, sphere\n";
    //     return 1;
    // }

    // if (use_logger) {
    //     lds::log_with_spdlog("Sequence generation completed");
    // }

    // std::cout << "Done!\n";
    return 0;
}
