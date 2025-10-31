#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>

#include <Pixel.hpp>
#include <Intersection.hpp>

using cucumber::ScenarioScope;

namespace {
struct ArtifactCtx {
  std::vector<std::string> lines;        // loaded artifact lines
  std::vector<Intersection> synthesized; // intersections synthesized from payload lines
  int limit = -1;                        // simulated record limit
  std::string tmpPath;                   // temp artifact path for logger tests
  int px_oid = -1;
  int ix_oid = -1;
  std::string tmpDir;                    // temp directory for integration artifacts
  int proxy_pid = -1;                    // background ControlChannel pid
  std::string proxy_xsub;
  std::string proxy_xpub;
};

static std::vector<std::string> readAllLines(const std::string &path) {
  std::ifstream in(path.c_str());
  std::vector<std::string> out;
  std::string line;
  while (std::getline(in, line)) {
    out.push_back(line);
  }
  return out;
}

static void ensureDir(const std::string &dir) {
  struct stat st;
  if (stat(dir.c_str(), &st) != 0) {
    // best-effort create
    ::system((std::string("mkdir -p ") + dir).c_str());
  }
}
} // namespace
// Utility to write a 150x2 slice artifact from a full interleaved file
GIVEN("^a 150x2 slice artifact from \"([^\"]+)\" rows (\\d+)-(\\d+)$") {
  REGEX_PARAM(std::string, relpath);
  REGEX_PARAM(int, y0);
  REGEX_PARAM(int, y1);
  ScenarioScope<ArtifactCtx> ctx;
  // Ensure temp run dir exists
  if (ctx->tmpDir.empty()) {
    ctx->tmpDir = "tmp/run";
    ensureDir("tmp");
    ensureDir(ctx->tmpDir);
  }

  // Load the source artifact (expected to be PixelFactory-style: Pixel line + blank payload)
  std::string path = std::string("../") + relpath;
  auto L = readAllLines(path);
  ASSERT_FALSE(L.empty()) << "missing input artifact: " << path;
  ASSERT_EQ(L.size() % 2, 0u) << "source must be interleaved (2N lines)";

  // Prepare output slice path
  std::string outPath = ctx->tmpDir + "/slice.txt";
  std::ofstream out(outPath.c_str());
  ASSERT_TRUE(out.good()) << "unable to open output: " << outPath;

  int written = 0;
  for (size_t i = 0; i + 1 < L.size(); i += 2) {
    Pixel p; ReadPixel(L[i], p);
    if (p.y >= y0 && p.y <= y1) {
      // Re-emit in canonical form: Pixel CSV, then blank payload (slice keeps defaults)
      PrintPixel(out, p);
      out << '\n';
      written++;
    }
  }
  out.close();
  // Sanity: expect width*rows records; width comes from data; but we at least need >0
  ASSERT_GT(written, 0) << "no records matched rows " << y0 << "-" << y1;
}

// PixelFactory steps
GIVEN("^I load the sample artifact \"(.+)\"$") {
  REGEX_PARAM(std::string, relpath);
  ScenarioScope<ArtifactCtx> ctx;
  // Features run from test/; artifact lives under ../
  std::string path = std::string("../") + relpath;
  ctx->lines = readAllLines(path);
  ASSERT_GT(ctx->lines.size(), 0u);
}

THEN("^it has interleaved 2N lines with blank payloads$") {
  ScenarioScope<ArtifactCtx> ctx;
  const auto &L = ctx->lines;
  ASSERT_EQ(L.size() % 2, 0u) << "Total lines must be even (2N)";
  for (size_t i = 0; i < L.size(); i += 2) {
    ASSERT_FALSE(L[i].empty()) << "Pixel line must be non-empty at line " << (i+1);
    ASSERT_TRUE(L[i+1].empty()) << "Intersection payload must be blank at line " << (i+2);
  }
}

THEN("^the first row is row-major for (\\d+) pixels$") {
  REGEX_PARAM(int, n);
  ScenarioScope<ArtifactCtx> ctx;
  int checked = 0;
  float last_x = -1.0f;
  float y0 = -1.0f;
  for (size_t i = 0; i + 1 < ctx->lines.size() && checked < n; i += 2) {
    Pixel p; ReadPixel(ctx->lines[i], p);
    if (checked == 0) { y0 = p.y; }
    ASSERT_FLOAT_EQ(p.y, y0) << "Expected constant y for first row sample";
    if (checked > 0) {
      ASSERT_GT(p.x, last_x) << "x should increase in row-major order";
    }
    last_x = p.x;
    checked++;
  }
  ASSERT_EQ(checked, n) << "Insufficient pixel lines to check";
}

// Feeder steps
WHEN("^I synthesize default Intersections for blank payloads$") {
  ScenarioScope<ArtifactCtx> ctx;
  ctx->synthesized.clear();
  const auto &L = ctx->lines;
  for (size_t i = 0; i + 1 < L.size(); i += 2) {
    Intersection ix; // defaults
    const std::string &payload = L[i+1];
    if (!payload.empty()) {
      ReadIntersection(payload, ix);
    }
    ctx->synthesized.push_back(ix);
  }
}

THEN("^all blank payload records have default Intersection$") {
  ScenarioScope<ArtifactCtx> ctx;
  const auto &L = ctx->lines;
  ASSERT_EQ(ctx->synthesized.size(), L.size() / 2);
  for (size_t rec = 0; rec < ctx->synthesized.size(); ++rec) {
    const std::string &payload = L[2*rec + 1];
    const Intersection &ix = ctx->synthesized[rec];
    if (payload.empty()) {
      EXPECT_FALSE(ix.gothit);
      EXPECT_EQ(ix.oid, -1);
    }
  }
}

WHEN("^I limit to (\\d+) records$") {
  REGEX_PARAM(int, limit);
  ScenarioScope<ArtifactCtx> ctx;
  ctx->limit = limit;
}

THEN("^the limited records count is (\\d+)$") {
  REGEX_PARAM(int, expected);
  ScenarioScope<ArtifactCtx> ctx;
  int available = static_cast<int>(ctx->lines.size() / 2);
  int actual = ctx->limit < 0 ? available : std::min(ctx->limit, available);
  EXPECT_EQ(actual, expected);
}

// Logger steps
GIVEN("^a temporary artifact file$") {
  ScenarioScope<ArtifactCtx> ctx;
  ensureDir("tmp");
  ctx->tmpPath = "tmp/logger_artifact.txt";
  // clean any prior file
  std::remove(ctx->tmpPath.c_str());
}

WHEN("^I write (\\d+) records with payload presence pattern \"([01]+)\"$") {
  REGEX_PARAM(int, n);
  REGEX_PARAM(std::string, pattern);
  ScenarioScope<ArtifactCtx> ctx;
  ASSERT_EQ(static_cast<int>(pattern.size()), n);

  // Load source pixels from the sample artifact
  auto src = readAllLines("../test/fixtures/sample_3_records.txt");
  ASSERT_GE(static_cast<int>(src.size()/2), n);

  std::ofstream out(ctx->tmpPath.c_str(), std::ios::app);
  for (int i = 0; i < n; ++i) {
    Pixel p; ReadPixel(src[2*i], p);
    PrintPixel(out, p);
    if (pattern[i] == '1') {
      Intersection ix; ix.gothit = true; ix.oid = 42;
      PrintIntersection(out, ix);
    } else {
      out << "\n";
    }
  }
  out.close();
}

THEN("^the file has (\\d+) lines with blanks at positions \"([0-9, ]*)\"$") {
  REGEX_PARAM(int, expected_lines);
  REGEX_PARAM(std::string, blanks);
  ScenarioScope<ArtifactCtx> ctx;
  auto L = readAllLines(ctx->tmpPath);
  ASSERT_EQ(static_cast<int>(L.size()), expected_lines);
  // parse record indices (1-based) that should have blank payload lines
  std::vector<int> idxs;
  std::istringstream ss(blanks);
  std::string tok;
  while (std::getline(ss, tok, ',')) {
    if (!tok.empty()) idxs.push_back(std::stoi(tok));
  }
  for (int rec : idxs) {
    int payload_line = 2*rec - 1; // zero-based index for second line of record
    ASSERT_TRUE(payload_line >= 0 && payload_line < static_cast<int>(L.size()));
    EXPECT_TRUE(L[payload_line].empty());
  }
}

WHEN("^I append the same (\\d+) records to the file$") {
  REGEX_PARAM(int, n);
  ScenarioScope<ArtifactCtx> ctx;
  // Reuse pattern from previous step if needed: alternate present/absent
  std::string pattern(n, '0');
  for (int i = 0; i < n; ++i) pattern[i] = (i % 2 == 0) ? '1' : '0';
  // Delegate to the write step logic by duplicating behavior here
  auto src = readAllLines("../test/fixtures/sample_3_records.txt");
  std::ofstream out(ctx->tmpPath.c_str(), std::ios::app);
  for (int i = 0; i < n; ++i) {
    Pixel p; ReadPixel(src[2*i], p);
    PrintPixel(out, p);
    if (pattern[i] == '1') {
      Intersection ix; ix.gothit = true; ix.oid = 42;
      PrintIntersection(out, ix);
    } else {
      out << "\n";
    }
  }
  out.close();
}

THEN("^the file now has (\\d+) lines$") {
  REGEX_PARAM(int, expected);
  ScenarioScope<ArtifactCtx> ctx;
  auto L = readAllLines(ctx->tmpPath);
  EXPECT_EQ(static_cast<int>(L.size()), expected);
}

// Writer steps (minimal, rule-only)
GIVEN("^pixel oid (\\-?\\d+) and intersection oid (\\-?\\d+)$") {
  REGEX_PARAM(int, px_oid);
  REGEX_PARAM(int, ix_oid);
  ScenarioScope<ArtifactCtx> ctx;
  ctx->px_oid = px_oid;
  ctx->ix_oid = ix_oid;
}

THEN("^the chosen oid should be (\\-?\\d+)$") {
  REGEX_PARAM(int, expected);
  ScenarioScope<ArtifactCtx> ctx;
  int chosen = (ctx->ix_oid != -1) ? ctx->ix_oid : ctx->px_oid;
  EXPECT_EQ(chosen, expected);
}

// =========================
// Integration steps (actors)
// =========================
GIVEN("^a fresh temp run directory$") {
  ScenarioScope<ArtifactCtx> ctx;
  ctx->tmpDir = "tmp/run";
  ensureDir("tmp");
  // Clean and recreate
  ::system("rm -rf tmp/run >/dev/null 2>&1");
  ensureDir(ctx->tmpDir);
}

GIVEN("^Logger on socket \"([^\"]+)\" subscribing \"([^\"]+)\" writing label \"([^\"]+)\"$") {
  REGEX_PARAM(std::string, socket);
  REGEX_PARAM(std::string, topic);
  REGEX_PARAM(std::string, label);
  ScenarioScope<ArtifactCtx> ctx;
  ASSERT_FALSE(ctx->tmpDir.empty());
  // Decide bind vs connect: default to bind for direct Feeder->Logger; connect when using proxy XPUB.
  bool use_bind = true;
  if (!ctx->proxy_xpub.empty() && ctx->proxy_xpub == socket) {
    use_bind = false;
  }
  // Launch Logger in background, writing to tmpDir
  std::ostringstream cmd;
  cmd << "bash -lc 'cd ../Logger && LOG_BASEDIR=../test/" << ctx->tmpDir;
  // Set bind/connect explicitly to avoid inherited env surprises
  cmd << (use_bind ? " LOGGER_BIND=1" : " LOGGER_BIND=0");
  cmd << " DYLD_LIBRARY_PATH=../../zmq_widgets/bin:$DYLD_LIBRARY_PATH ./start.sh "
      << socket << " " << topic << " " << label << " > /dev/null 2>&1 &'";
  int rc = ::system(cmd.str().c_str());
  ASSERT_EQ(rc, 0);
  // Give the subscriber a brief moment to connect before feeding
  usleep(100*1000);
}

WHEN("^I feed (\\d+) records from \"([^\"]+)\" to socket \"([^\"]+)\" on channel \"([^\"]+)\"$") {
  REGEX_PARAM(int, n);
  REGEX_PARAM(std::string, relpath);
  REGEX_PARAM(std::string, socket);
  REGEX_PARAM(std::string, channel);
  // Drive Feeder to publish limited records to the given socket
  std::ostringstream cmd;
  cmd << "bash -lc 'cd ../Feeder && FEEDER_LIMIT=" << n
      << " DYLD_LIBRARY_PATH=../../zmq_widgets/bin:$DYLD_LIBRARY_PATH ./start.sh "
      << channel << " ../" << relpath << " " << socket << "'";
  int rc = ::system(cmd.str().c_str());
  ASSERT_EQ(rc, 0);
}

THEN("^the logger artifact \"([^\"]+)\" should have (\\d+) records$") {
  REGEX_PARAM(std::string, filename);
  REGEX_PARAM(int, records);
  ScenarioScope<ArtifactCtx> ctx;
  std::string path = std::string("../test/") + ctx->tmpDir + "/o" + filename;
  // Wait up to ~3s for file to appear
  for (int i = 0; i < 30; ++i) {
    std::ifstream in(path.c_str());
    if (in.good()) break;
    usleep(100*1000);
  }
  auto L = readAllLines(path);
  ASSERT_FALSE(L.empty()) << "logger output missing: " << path;
  EXPECT_EQ(static_cast<int>(L.size()), records*2) << "must be 2N lines";
}

GIVEN("^Writer bound on socket \"([^\"]+)\" output \"([^\"]+)\"$") {
  REGEX_PARAM(std::string, socket);
  REGEX_PARAM(std::string, pngPath);
  // Start Writer in background with a bound subscriber
  // Ensure idempotency: remove any prior PNG at the target path
  {
    std::string rm = std::string("../") + pngPath;
    std::remove(rm.c_str());
  }
  std::ostringstream cmd;
  cmd << "bash -lc 'cd ../Writer && WRITER_BIND_SUB=1 WRITER_INPUT_SOCKET=" << socket
      << " DYLD_LIBRARY_PATH=../../zmq_widgets/bin:$DYLD_LIBRARY_PATH ./start.sh "
      << pngPath << " > /dev/null 2>&1 &'";
  int rc = ::system(cmd.str().c_str());
  ASSERT_EQ(rc, 0);
}

THEN("^PNG \"([^\"]+)\" should exist$") {
  REGEX_PARAM(std::string, relpath);
  // Wait up to 5s for the PNG to materialize
  std::string path = std::string("../") + relpath;
  for (int i = 0; i < 50; ++i) {
    struct stat st{};
    if (stat(path.c_str(), &st) == 0 && st.st_size > 0) {
      SUCCEED();
      return;
    }
    usleep(100*1000);
  }
  ADD_FAILURE() << "PNG not found: " << path;
}

// ControlChannel proxy steps
GIVEN("^ControlChannel proxy on XSUB \"([^\"]+)\" and XPUB \"([^\"]+)\"$") {
  REGEX_PARAM(std::string, xsub);
  REGEX_PARAM(std::string, xpub);
  ScenarioScope<ArtifactCtx> ctx;
  ASSERT_FALSE(ctx->tmpDir.empty());
  ctx->proxy_xsub = xsub;
  ctx->proxy_xpub = xpub;
  std::string pidFile = ctx->tmpDir + "/proxy.pid";
  std::string logFile = ctx->tmpDir + "/proxy.log";
  std::ostringstream cmd;
  cmd << "bash -lc '"
      << " DYLD_LIBRARY_PATH=../../zmq_widgets/bin:$DYLD_LIBRARY_PATH"
      << " ../../zmq_widgets/bin/ControlChannel " << xsub << " " << xpub
      << " > " << logFile << " 2>&1 & echo $! > " << pidFile << "'";
  int rc = ::system(cmd.str().c_str());
  ASSERT_EQ(rc, 0);
  // Read pid back
  std::ifstream in(pidFile.c_str());
  int pid = -1; in >> pid; in.close();
  ASSERT_GT(pid, 0);
  ctx->proxy_pid = pid;
  // Brief pause to allow binds
  usleep(100*1000);
}

THEN("^I stop the proxy$") {
  ScenarioScope<ArtifactCtx> ctx;
  if (ctx->proxy_pid > 0) {
    ::kill(ctx->proxy_pid, SIGTERM);
    // Wait up to 1s
    for (int i = 0; i < 10; ++i) {
      if (kill(ctx->proxy_pid, 0) != 0) break;
      usleep(100*1000);
    }
    // Force kill if still alive
    if (kill(ctx->proxy_pid, 0) == 0) {
      ::kill(ctx->proxy_pid, SIGKILL);
    }
    ctx->proxy_pid = -1;
  }
}

GIVEN("^I bring all processes down$") {
  // Run the robust teardown script from repo root
  int rc = ::system("../scripts/pipeline/all_down.sh >/dev/null 2>&1");
  (void)rc; // best-effort; do not assert
  // Small wait to let ports free
  usleep(100*1000);
}

// PixelFactory streaming steps
GIVEN("^PixelFactory streaming (\\d+) records on socket \"([^\"]+)\" channel \"([^\"]+)\"$") {
  REGEX_PARAM(int, n);
  REGEX_PARAM(std::string, socket);
  REGEX_PARAM(std::string, channel);
  // Launch PixelFactory in background to publish Pixel-only messages
  std::ostringstream cmd;
  cmd << "bash -lc 'cd ../PixelFactory && sleep 0.5; PF_STREAM=1 PF_LIMIT=" << n
      << " PF_WRITE_FILE=0 PF_OUTPUT_SOCKET=" << socket
      << " PF_OUTPUT_CHANNEL=" << channel
      << " DYLD_LIBRARY_PATH=../../zmq_widgets/bin:$DYLD_LIBRARY_PATH ./start.sh > /dev/null 2>&1 &'";
  int rc = ::system(cmd.str().c_str());
  ASSERT_EQ(rc, 0);
}

THEN("^the logger artifact \"([^\"]+)\" should have (\\d+) blank payload lines$") {
  REGEX_PARAM(std::string, filename);
  REGEX_PARAM(int, blanks);
  ScenarioScope<ArtifactCtx> ctx;
  std::string path = std::string("../test/") + ctx->tmpDir + "/o" + filename;
  // Wait briefly for file to appear and flush
  for (int i = 0; i < 50; ++i) {
    std::ifstream in(path.c_str());
    if (in.good()) break;
    usleep(100*1000);
  }
  auto L = readAllLines(path);
  ASSERT_FALSE(L.empty()) << "logger output missing: " << path;
  int count_blank = 0;
  for (size_t i = 1; i < L.size(); i += 2) {
    if (L[i].empty()) count_blank++;
  }
  EXPECT_EQ(count_blank, blanks);
}
