#include <doctest/doctest.h>
#include <lds/greeter.h>
#include <lds/version.h>

#include <string>

TEST_CASE("Lds") {
  using namespace lds;

  Lds lds("Tests");

  CHECK(lds.greet(LanguageCode::EN) == "Hello, Tests!");
  CHECK(lds.greet(LanguageCode::DE) == "Hallo Tests!");
  CHECK(lds.greet(LanguageCode::ES) == "¡Hola Tests!");
  CHECK(lds.greet(LanguageCode::FR) == "Bonjour Tests!");
}

TEST_CASE("Lds version") {
  static_assert(std::string_view(LDS_VERSION) == std::string_view("1.0"));
  CHECK(std::string(LDS_VERSION) == std::string("1.0"));
}
