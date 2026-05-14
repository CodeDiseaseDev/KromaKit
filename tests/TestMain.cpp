#include "TestHarness.h"

#ifndef TEST_SUITE_NAME
#define TEST_SUITE_NAME "Test"
#endif

int main() {
  return TestHarness::RunAllTests(TEST_SUITE_NAME);
}

