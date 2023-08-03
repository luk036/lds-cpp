# CPMAddPackage("gh:microsoft/GSL@3.1.0")

# set(SPECIFIC_LIBS GSL)

CPMAddPackage(
  NAME fmt
  GIT_TAG 9.1.0
  GITHUB_REPOSITORY fmtlib/fmt
  OPTIONS "FMT_INSTALL YES" # create an installable target
)

set(SPECIFIC_LIBS fmt::fmt)
