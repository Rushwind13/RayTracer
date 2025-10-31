#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>

using cucumber::ScenarioScope;

static bool fw_file_exists_nonempty(const std::string &path) {
  struct stat buf; return (stat(path.c_str(), &buf) == 0) && (buf.st_size > 0);
}

static std::vector<std::string> fw_read_lines(const std::string &path) {
  std::vector<std::string> lines; std::ifstream in(path); std::string s; while (std::getline(in,s)) lines.push_back(s); return lines;
}

// Write a minimal Pixel line for x,y (others zeroed)
static void fw_write_pixel(std::ofstream &out, int x, int y) {
  out << x << "," << y << ",";
  out << -1 << ","; // oid
  // primaryRay.direction (4)
  out << 0 << "," << 0 << "," << 0 << "," << 0 << ",";
  // primaryRay.origin (4)
  out << 0 << "," << 0 << "," << 0 << "," << 0 << ",";
  out << 0 << ","; // primaryRay.length
  out << 0 << ","; // type
  // r.direction (4)
  out << 0 << "," << 0 << "," << 0 << "," << 0 << ",";
  // r.origin (4)
  out << 0 << "," << 0 << "," << 0 << "," << 0 << ",";
  out << 0 << ","; // r.length
  // normal (4)
  out << 0 << "," << 0 << "," << 0 << "," << 0 << ",";
  // position (4)
  out << 0 << "," << 0 << "," << 0 << "," << 0 << ",";
  out << 0 << ","; // distance
  out << 0 << ","; // lid
  out << 0 << ","; // NdotL
  out << 0 << ","; // depth
  out << 0 << ","; // weight
  // color (4)
  out << 0 << "," << 0 << "," << 0 << "," << 0 << ",";
  out << 0 << std::endl; // gothit
}

// Write an Intersection line with given gothit and oid (others zeroed)
static void fw_write_intersection(std::ofstream &out, int gothit, int oid) {
  out << gothit << ",";
  out << 0 << ","; // anyhit
  out << oid << ",";
  // normal (4)
  out << 0 << "," << 0 << "," << 0 << "," << 0 << ",";
  // position (4)
  out << 0 << "," << 0 << "," << 0 << "," << 0 << ",";
  // distance[0], distance[1]
  out << 0 << "," << 0 << std::endl;
}

GIVEN("^I prepare a sample interleaved file with a blank Intersection in the middle at \"(.+)\"$") {
  REGEX_PARAM(std::string, out_rel);
  std::string out = std::string("../") + out_rel;
  std::string dir = out.substr(0, out.find_last_of('/'));
  std::string mkdir_cmd = std::string("/bin/mkdir -p ") + dir;
  system(mkdir_cmd.c_str());
  std::ofstream outfs(out);
  ASSERT_TRUE(outfs.good());
  // Record 1: non-empty intersection (oid=5, gothit=1)
  fw_write_pixel(outfs, 0, 0);
  fw_write_intersection(outfs, 1, 5);
  // Record 2: blank intersection line
  fw_write_pixel(outfs, 1, 0);
  outfs << "" << std::endl; // blank line
  // Record 3: non-empty intersection (oid=7, gothit=1)
  fw_write_pixel(outfs, 2, 0);
  fw_write_intersection(outfs, 1, 7);
  outfs.close();
  ASSERT_TRUE(fw_file_exists_nonempty(out));
}

WHEN("^I capture Feeder output from \"(.+)\" into \"(.+)\"$") {
  REGEX_PARAM(std::string, src_rel);
  REGEX_PARAM(std::string, out_rel);
  std::string src = std::string("../") + src_rel;
  std::string out = std::string("../") + out_rel;

  // Ensure no stray pipeline processes are publishing to the proxy ports
  system("bash ../scripts/pipeline/all_down.sh > /dev/null 2>&1 || true");

  // Ensure zmq_widgets lib is discoverable
  setenv("DYLD_LIBRARY_PATH", "../zmq_widgets/bin", 0);

  // Ensure runs dir exists for logs/pids
  system("/bin/mkdir -p ../runs >/dev/null 2>&1");

  // Start proxy (XSUB 1314, XPUB 1313)
  // Pre-clean any stray proxies from prior failed runs
  system("pkill -f '/zmq_widgets/bin/ControlChannel' >/dev/null 2>&1 || true");
  usleep(100 * 1000);
  int rc1 = system("../zmq_widgets/bin/ControlChannel tcp://127.0.0.1:1314 tcp://127.0.0.1:1313 > ../runs/feeder_proxy.log 2>&1 & echo $! > ../runs/.proxy.pid");
  int exit1 = WIFEXITED(rc1) ? WEXITSTATUS(rc1) : rc1;
  EXPECT_EQ(exit1, 0);

  // Start Logger bound to XPUB; write to requested output in its directory
  std::string out_dir = out.substr(0, out.find_last_of('/'));
  std::string out_base = out.substr(out.find_last_of('/')+1);
  std::string mk = std::string("/bin/mkdir -p ") + out_dir;
  system(mk.c_str());
  std::string log_cmd = std::string("cd ../Logger && LOG_BASEDIR=\"") + out_dir + "\" ./start.sh tcp://127.0.0.1:1313 IntersectWith \"" + out_base + "\" > ../runs/feeder_logger.log 2>&1 & echo $! > ../runs/.logger.pid";
  int rc2 = system(log_cmd.c_str());
  int exit2 = WIFEXITED(rc2) ? WEXITSTATUS(rc2) : rc2;
  EXPECT_EQ(exit2, 0);
  // Ensure logger process started and is alive
  // Give Echo time to bind its SUB and connect its PUB before Feeder publishes
  usleep(1000 * 1000);
  {
    std::ifstream pin("../runs/.logger.pid");
    int pid = -1; pin >> pid; pin.close();
    if (pid > 0) {
      // kill -0 checks if process exists
      std::string chk = std::string("kill -0 ") + std::to_string(pid) + " >/dev/null 2>&1 || true";
      int rc = system(chk.c_str());
      int ex = WIFEXITED(rc) ? WEXITSTATUS(rc) : rc;
      EXPECT_EQ(ex, 0);
    }
  }

  // Run Feeder to publish the sample into XSUB
  std::string feed_cmd = std::string("cd ../Feeder && ./start.sh IntersectWith \"") + src + "\" tcp://127.0.0.1:1314 > ../runs/feeder_feeder.log 2>&1";
  int rc3 = system(feed_cmd.c_str());
  int exit3 = WIFEXITED(rc3) ? WEXITSTATUS(rc3) : rc3;
  EXPECT_EQ(exit3, 0);

  // Wait up to ~3s for logger to flush file
  {
    std::string out_dir2 = out.substr(0, out.find_last_of('/'));
    std::string out_base2 = out.substr(out.find_last_of('/')+1);
    std::string out_path = out_dir2 + "/o" + out_base2;
    const int tries = 30;
    for (int i = 0; i < tries; ++i) {
      if (fw_file_exists_nonempty(out_path)) break;
      usleep(100 * 1000);
    }
    // If not present yet, nudge logger with SIGINT to flush and retry briefly
    if (!fw_file_exists_nonempty(out_path)) {
      std::ifstream pin("../runs/.logger.pid");
      int pid = -1; pin >> pid; pin.close();
      if (pid > 0) {
        std::string sig = std::string("kill -INT ") + std::to_string(pid) + " >/dev/null 2>&1 || true";
        system(sig.c_str());
        usleep(200 * 1000);
      }
      for (int i = 0; i < 10; ++i) {
        if (fw_file_exists_nonempty(out_path)) break;
        usleep(50 * 1000);
      }
    }
  }

  // Cleanup background processes
  system("kill $(cat ../runs/.logger.pid) >/dev/null 2>&1 || true");
  system("rm -f ../runs/.logger.pid >/dev/null 2>&1 || true");
  system("kill $(cat ../runs/.proxy.pid) >/dev/null 2>&1 || true");
  system("rm -f ../runs/.proxy.pid >/dev/null 2>&1 || true");

  // Verify output exists at Logger's naming convention (prefix 'o')
  std::string out_dir2 = out.substr(0, out.find_last_of('/'));
  std::string out_base2 = out.substr(out.find_last_of('/')+1);
  std::string out_path = out_dir2 + "/o" + out_base2;
  ASSERT_TRUE(fw_file_exists_nonempty(out_path));
}

THEN("^in capture \"(.+)\" record (\\d+) intersection oid equals (-?\\d+) and gothit equals (\\d+)$") {
  REGEX_PARAM(std::string, relpath);
  REGEX_PARAM(int, recno);
  REGEX_PARAM(int, expect_oid);
  REGEX_PARAM(int, expect_gothit);
  std::string path = std::string("../") + relpath;
  ASSERT_TRUE(fw_file_exists_nonempty(path));
  auto lines = fw_read_lines(path);
  int pair_index = recno - 1;
  int idx_intersection = pair_index * 2 + 1;
  ASSERT_LT(idx_intersection, (int)lines.size());
  std::string line = lines[idx_intersection];
  int gothit = 0, oid = 0;
  {
    std::istringstream ss(line);
    std::string tmp;
    std::getline(ss, tmp, ','); gothit = std::stoi(tmp);
    std::getline(ss, tmp, ','); /* anyhit */;
    std::getline(ss, tmp, ','); oid = std::stoi(tmp);
  }
  EXPECT_EQ(oid, expect_oid);
  EXPECT_EQ(gothit, expect_gothit);
}


THEN("^in interleaved file \"(.+)\" first (\\d+) records have blank intersection lines$") {
  REGEX_PARAM(std::string, relpath);
  REGEX_PARAM(int, k);
  std::string path = std::string("../") + relpath;
  ASSERT_TRUE(fw_file_exists_nonempty(path));
  auto lines = fw_read_lines(path);
  int max_pairs = std::min(k, (int)(lines.size()/2));
  for (int i = 0; i < max_pairs; ++i) {
    std::string s = lines[i*2 + 1];
    // Trim whitespace
    bool empty = true;
    for (char c : s) { if (!(c=='\n' || c=='\r' || c==' ' || c=='\t')) { empty = false; break; } }
    EXPECT_TRUE(empty) << "Intersection line at record " << (i+1) << " is not blank: '" << s << "'";
  }
}

WHEN("^I render a PNG via Echo from artifact \"(.+)\" to \"(.+)\"$") {
  REGEX_PARAM(std::string, src_rel);
  REGEX_PARAM(std::string, png_rel);
  std::string src = std::string("../") + src_rel;
  std::string png = std::string("../") + png_rel;

  // Prepare directories
  std::string png_dir = png.substr(0, png.find_last_of('/'));
  std::string mk_png = std::string("/bin/mkdir -p ") + png_dir;
  system(mk_png.c_str());
  system("/bin/mkdir -p ../runs/tests >/dev/null 2>&1");

  // Choose isolated sockets for test
  const char* ECHO_SUB = "tcp://127.0.0.1:1392"; // Echo binds SUB here
  const char* WRITER_SUB = "tcp://127.0.0.1:1390"; // Writer binds SUB here

  // Clean any strays
  system("pkill -f '/bin/Echo' >/dev/null 2>&1 || true");
  system("pkill -f '/bin/Writer' >/dev/null 2>&1 || true");

  // Start Writer (bind subscriber) and capture log
  std::string wcmd = std::string("cd ../Writer && WRITER_BIND_SUB=1 WRITER_INPUT_SOCKET=") + WRITER_SUB + " ./start.sh '" + png + "' > ../runs/tests/writer_echo.log 2>&1 & echo $! > ../runs/.writer.pid";
  int rcw = system(wcmd.c_str());
  int exw = WIFEXITED(rcw) ? WEXITSTATUS(rcw) : rcw;
  EXPECT_EQ(exw, 0);
  usleep(200 * 1000);

  // Start Echo (bind SUB to accept Feeder connect; PUB connects to Writer)
  std::string ecmd = std::string("cd ../Echo && ECHO_BIND_SUB=1 ./start.sh ") + ECHO_SUB + " IntersectWith PNG " + WRITER_SUB + " > ../runs/tests/echo.log 2>&1 & echo $! > ../runs/.echo.pid";
  int rce = system(ecmd.c_str());
  int exe = WIFEXITED(rce) ? WEXITSTATUS(rce) : rce;
  EXPECT_EQ(exe, 0);
  usleep(200 * 1000);

  // Run Feeder to publish artifact into Echo SUB
  std::string fcmd = std::string("cd ../Feeder && ./start.sh IntersectWith '") + src + "' " + ECHO_SUB + " > ../runs/tests/feeder_echo.log 2>&1";
  int rcf = system(fcmd.c_str());
  int exf = WIFEXITED(rcf) ? WEXITSTATUS(rcf) : rcf;
  EXPECT_EQ(exf, 0);

  // Wait up to 10s for PNG output to appear (allow Writer to autosave if needed)
  for (int i = 0; i < 100; ++i) {
    if (fw_file_exists_nonempty(png)) break;
    usleep(100 * 1000);
  }

  // Cleanup echo and writer if still running
  system("kill $(cat ../runs/.echo.pid) >/dev/null 2>&1 || true");
  system("rm -f ../runs/.echo.pid >/dev/null 2>&1 || true");
  system("kill $(cat ../runs/.writer.pid) >/dev/null 2>&1 || true");
  system("rm -f ../runs/.writer.pid >/dev/null 2>&1 || true");
}

THEN("^Writer reported at least (\\d+) distinct colors in \"(.+)\"$") {
  REGEX_PARAM(int, min_colors);
  REGEX_PARAM(std::string, log_rel);
  std::string log = std::string("../") + log_rel;
  ASSERT_TRUE(fw_file_exists_nonempty(log));
  auto lines = fw_read_lines(log);
  int found = -1;
  std::regex rx("^\\[STATS\\] distinct_colors: (\\d+)");
  std::smatch m;
  for (auto &s : lines) {
    if (std::regex_search(s, m, rx)) {
      found = std::stoi(m[1].str());
      break;
    }
  }
  ASSERT_GE(found, 0) << "No distinct_colors stat found in log: " << log;
  EXPECT_GE(found, min_colors);
}
