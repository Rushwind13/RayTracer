#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <string>

using cucumber::ScenarioScope;

static bool file_exists_nonempty_1up(const std::string &path) {
  struct stat buf;
  return (stat(path.c_str(), &buf) == 0) && (buf.st_size > 0);
}

GIVEN("^I run only-one-up stage \"([0-9]+)\" with input \"(.+)\" as run \"(.+)\"$") {
  REGEX_PARAM(std::string, stage);
  REGEX_PARAM(std::string, input_rel);
  REGEX_PARAM(std::string, run_id);
  std::string cmd = std::string("RUN_ID=") + run_id +
                    " INPUT_FILE=" + input_rel +
                    " FEEDER_LIMIT=200 SMOKE_MODE=1 bash ../scripts/pipeline/only_one_up.sh " + stage;
  int rc = system(cmd.c_str());
  int exit_code = WIFEXITED(rc) ? WEXITSTATUS(rc) : rc;
  EXPECT_EQ(exit_code, 0);
}

GIVEN("^I run only-one-up stage \"([0-9]+)\" as run \"(.+)\"$") {
  REGEX_PARAM(std::string, stage);
  REGEX_PARAM(std::string, run_id);
  std::string cmd = std::string("RUN_ID=") + run_id +
                    " FEEDER_LIMIT=200 SMOKE_MODE=1 bash ../scripts/pipeline/only_one_up.sh " + stage;
  int rc = system(cmd.c_str());
  int exit_code = WIFEXITED(rc) ? WEXITSTATUS(rc) : rc;
  EXPECT_EQ(exit_code, 0);
}
