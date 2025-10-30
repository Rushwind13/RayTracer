#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <sys/wait.h>

using cucumber::ScenarioScope;

static bool file_exists(const std::string &path) {
  struct stat buf;
  return (stat(path.c_str(), &buf) == 0) && (buf.st_size > 0);
}

GIVEN("^I run the all-up script$") {
  // Run the script relative to test directory: ../scripts/pipeline/all_up.sh
  // Enable SMOKE_MODE only for this invocation to keep regular shells clean
  int rc = system("SMOKE_MODE=1 bash ../scripts/pipeline/all_up.sh");
  ScenarioScope<int> status;
  *status = rc;
  // system() returns exit status in the high-order byte on POSIX
  int exit_code = WIFEXITED(rc) ? WEXITSTATUS(rc) : rc;
  EXPECT_EQ(exit_code, 0);
}

THEN("^a PNG \"(.+)\" should exist$") {
  REGEX_PARAM(std::string, relpath);
  // From test directory, bin/ is ../bin/
  std::string path = std::string("../") + relpath;
  EXPECT_EQ(file_exists(path), true);
}
