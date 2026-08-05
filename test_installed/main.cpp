#include <lds/version.h>

#include <iostream>

auto main() -> int {
    const auto ok = (LDS_VERSION_MAJOR >= 1);
    std::cout << "lds installed test: version " << LDS_VERSION << "\n";
    return ok ? 0 : 1;
}
