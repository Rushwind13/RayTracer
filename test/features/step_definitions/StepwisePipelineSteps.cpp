#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>

using cucumber::ScenarioScope;

static bool file_exists_nonempty(const std::string &path) {
  struct stat buf;
  return (stat(path.c_str(), &buf) == 0) && (buf.st_size > 0);
}

static long count_nonempty_lines(const std::string &path) {
  std::ifstream in(path);
  if (!in.good()) return -1;
  long count = 0;
  std::string line;
  while (std::getline(in, line)) {
    bool nonempty = false;
    for (char c : line) {
      if (c != '\n' && c != '\r' && c != ' ' && c != '\t') { nonempty = true; break; }
    }
    if (nonempty) count++;
  }
  return count;
}

static long count_records_pairs(const std::string &path) {
  long lines = count_nonempty_lines(path);
  if (lines < 0) return -1;
  return lines / 2; // Pixel + Intersection per record
}

GIVEN("^I run the stepwise script with input \"(.+)\" as run \"(.+)\"$") {
  REGEX_PARAM(std::string, input_rel);
  REGEX_PARAM(std::string, run_id);
  // Run from test directory; scripts are in ../scripts
  std::string cmd = std::string("RUN_ID=") + run_id +
                    " STEPWISE_INPUT_FILE=../" + input_rel +
                    " SMOKE_MODE=1 bash ../scripts/pipeline/all_up_stepwise.sh";
  int rc = system(cmd.c_str());
  int exit_code = WIFEXITED(rc) ? WEXITSTATUS(rc) : rc;
  EXPECT_EQ(exit_code, 0);
}

WHEN("^the input file \"(.+)\" should exist with at least (\\d+) records$") {
  REGEX_PARAM(std::string, relpath);
  REGEX_PARAM(int, min_records);
  std::string path = std::string("../") + relpath;
  ASSERT_TRUE(file_exists_nonempty(path));
  long recs = count_records_pairs(path);
  ASSERT_GE(recs, min_records);
}

THEN("^the output file \"(.+)\" should exist with at least (\\d+) records$") {
  REGEX_PARAM(std::string, relpath);
  REGEX_PARAM(int, min_records);
  std::string path = std::string("../") + relpath;
  ASSERT_TRUE(file_exists_nonempty(path));
  long recs = count_records_pairs(path);
  ASSERT_GE(recs, min_records);
}
