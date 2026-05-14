#pragma once

#include <cmath>
#include <cstdio>
#include <exception>
#include <functional>
#include <string>
#include <vector>

namespace TestHarness {

struct Result {
  std::string name;
  bool passed = true;
  int checksPassed = 0;
  int checksFailed = 0;
  std::vector<std::string> failures;
};

struct Context {
  explicit Context(std::string testName) {
    result.name = std::move(testName);
  }

  Result result;

  void PassCheck() {
    ++result.checksPassed;
  }

  void Fail(const char* file, int line, const std::string& msg) {
    result.passed = false;
    ++result.checksFailed;
    result.failures.push_back(
      std::string(file) + ":" + std::to_string(line) + ": " + msg);
  }
};

using TestFn = std::function<void(Context&)>;

struct TestCase {
  std::string name;
  TestFn fn;
};

inline std::vector<TestCase>& Registry() {
  static std::vector<TestCase> tests;
  return tests;
}

inline void Register(const std::string& name, TestFn fn) {
  Registry().push_back({name, std::move(fn)});
}

inline int RunAllTests(const char* suiteName) {
  int testsPassed = 0;
  int testsFailed = 0;
  int checksPassed = 0;
  int checksFailed = 0;

  std::printf("Running %zu %s tests...\n\n", Registry().size(), suiteName);

  for (const auto& test : Registry()) {
    Context t(test.name);
    std::printf("[RUN] %s\n", test.name.c_str());

    try {
      test.fn(t);
    } catch (const std::exception& ex) {
      t.Fail(__FILE__, __LINE__, std::string("Unhandled std::exception: ") + ex.what());
    } catch (...) {
      t.Fail(__FILE__, __LINE__, "Unhandled unknown exception");
    }

    checksPassed += t.result.checksPassed;
    checksFailed += t.result.checksFailed;

    if (t.result.passed) {
      ++testsPassed;
      std::printf("[PASS] %s\n", test.name.c_str());
      continue;
    }

    ++testsFailed;
    std::printf("[FAIL] %s\n", test.name.c_str());
    for (const auto& failure : t.result.failures) {
      std::printf("       %s\n", failure.c_str());
    }
  }

  std::printf(
    "\n[SUMMARY] tests passed: %d, tests failed: %d, checks passed: %d, checks failed: %d\n",
    testsPassed, testsFailed, checksPassed, checksFailed);

  return testsFailed == 0 ? 0 : 1;
}

}  // namespace TestHarness

#define TEST_CASE(name)                               \
  static void name(TestHarness::Context& t);          \
  struct name##_registrar {                           \
    name##_registrar() {                              \
      TestHarness::Register(#name, name);             \
    }                                                 \
  };                                                  \
  static name##_registrar name##_registrar_instance;  \
  static void name(TestHarness::Context& t)

#define EXPECT_TRUE(expr)                                                      \
  do {                                                                         \
    if (expr) {                                                                \
      t.PassCheck();                                                           \
    } else {                                                                   \
      t.Fail(__FILE__, __LINE__, std::string("expected true: ") + #expr);     \
    }                                                                          \
  } while (0)

#define EXPECT_FALSE(expr)                                                     \
  do {                                                                         \
    if (!(expr)) {                                                             \
      t.PassCheck();                                                           \
    } else {                                                                   \
      t.Fail(__FILE__, __LINE__, std::string("expected false: ") + #expr);    \
    }                                                                          \
  } while (0)

#define EXPECT_EQ(actual, expected)                                                \
  do {                                                                             \
    if ((actual) == (expected)) {                                                  \
      t.PassCheck();                                                                \
    } else {                                                                        \
      t.Fail(__FILE__, __LINE__, std::string("expected equality: ") + #actual +    \
                                   " == " + #expected);                            \
    }                                                                               \
  } while (0)

#define EXPECT_NEAR(actual, expected, eps)                                         \
  do {                                                                              \
    if (std::fabs((actual) - (expected)) <= (eps)) {                               \
      t.PassCheck();                                                                \
    } else {                                                                        \
      t.Fail(__FILE__, __LINE__, std::string("expected near: ") + #actual +        \
                                   " ~= " + #expected);                            \
    }                                                                               \
  } while (0)
