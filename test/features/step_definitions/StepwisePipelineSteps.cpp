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

GIVEN("^I run the stepwise script with full feed input \"(.+)\" as run \"(.+)\"$") {
  REGEX_PARAM(std::string, input_rel);
  REGEX_PARAM(std::string, run_id);
  std::string cmd = std::string("RUN_ID=") + run_id +
                    " STEPWISE_INPUT_FILE=../" + input_rel +
                    " STEPWISE_FEEDER_LIMIT=-1 SMOKE_MODE=0 bash ../scripts/pipeline/all_up_stepwise.sh";
  int rc = system(cmd.c_str());
  int exit_code = WIFEXITED(rc) ? WEXITSTATUS(rc) : rc;
  EXPECT_EQ(exit_code, 0);
}

GIVEN("^I create a pixel slice \"(.+)\" rows \"([0-9,]+)\" as \"(.+)\"$") {
  REGEX_PARAM(std::string, src_rel);
  REGEX_PARAM(std::string, rows_csv);
  REGEX_PARAM(std::string, out_rel);
  // Resolve test dir relative paths: prefix with ../ to reach repo root
  std::string src = std::string("../") + src_rel;
  std::string out = std::string("../") + out_rel;
  // Ensure output directory exists
  std::string mkdir_cmd = std::string("/bin/mkdir -p ") + out.substr(0, out.find_last_of('/'));
  system(mkdir_cmd.c_str());
  // Use Python to filter CSV by y field (second column)
  std::string cmd = std::string(
      "python3 - \"") + src + "\" \"" + rows_csv + "\" \"" + out + "\" <<'PY'\n"
      "import sys,csv,os\n"
      "src,rows_csv,out=sys.argv[1],sys.argv[2],sys.argv[3]\n"
      "rows=set(int(x) for x in rows_csv.split(',') if x)\n"
      "with open(src,'r') as f, open(out,'w') as g:\n"
      "  for line in f:\n"
      "    s=line.strip()\n"
      "    if not s:\n"
      "      continue\n"
      "    parts=s.split(',')\n"
      "    try:\n"
      "      y=int(parts[1])\n"
      "    except Exception:\n"
      "      continue\n"
      "    if y in rows:\n"
      "      g.write(line)\n"
      "      if not line.endswith('\n'): g.write('\n')\n"
      "PY\n";
  int rc = system(cmd.c_str());
  int exit_code = WIFEXITED(rc) ? WEXITSTATUS(rc) : rc;
  EXPECT_EQ(exit_code, 0);
  // Sanity: output must be non-empty and roughly 300 lines (but do not hard fail here)
  ASSERT_TRUE(file_exists_nonempty(out));
}

GIVEN("^I run the stepwise script resuming from stage \"([0-9]+)\" as run \"(.+)\"$") {
  REGEX_PARAM(std::string, stage_str);
  REGEX_PARAM(std::string, run_id);
  // Allow stage number as string; validate it's numeric-ish
  std::string stage = stage_str;
  if (stage.empty()) stage = "2";
  // Run from test directory; scripts are in ../scripts. RESUME_STAGE uses defaults from runs/complete/*
  std::string cmd = std::string("RUN_ID=") + run_id +
                    " RESUME_STAGE=" + stage +
                    " STEPWISE_FEEDER_LIMIT=200 SMOKE_MODE=1 nohup bash ../scripts/pipeline/all_up_stepwise.sh >/dev/null 2>&1 &";
  int rc = system(cmd.c_str());
  int exit_code = WIFEXITED(rc) ? WEXITSTATUS(rc) : rc;
  EXPECT_EQ(exit_code, 0);
}

GIVEN("^I render a PNG from artifact \"(.+)\" to \"(.+)\"$") {
  REGEX_PARAM(std::string, artifact_rel);
  REGEX_PARAM(std::string, out_png_rel);
  std::string cmd = std::string("bash ../scripts/tools/stage_to_png.sh ../") + out_png_rel + " ../" + artifact_rel;
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
  // Poll for up to ~45s to allow background pipeline to produce the artifact
  const int max_tries = 90; // 90 * 0.5s = 45s
  int tries = 0;
  while (!file_exists_nonempty(path) && tries < max_tries) {
    usleep(500 * 1000);
    tries++;
  }
  ASSERT_TRUE(file_exists_nonempty(path));
  long recs = count_records_pairs(path);
  ASSERT_GE(recs, min_records);
}
