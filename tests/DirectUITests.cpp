// tests/DirectUITests.cpp

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <string>
#include <vector>

// Adjust these include paths to match your project.
#include <kromakit/Control.h>
#include <kromakit/Panel.h>
#include <kromakit/Button.h>
#include <kromakit/Label.h>
#include <kromakit/grid_layout_panel/GridLayoutPanel.h>
#include <kromakit/layout/layout_options/LayoutLength.h>

namespace Test {

struct TestResult {
  std::string name;
  bool passed = true;
  std::vector<std::string> failures;
};

struct TestContext {
  TestResult result;

  explicit TestContext(std::string name) {
    result.name = std::move(name);
  }

  void Fail(
    const char* expr,
    const char* file,
    int line,
    const std::string& message = "") {
    result.passed = false;

    std::string out;
    out += file;
    out += ":";
    out += std::to_string(line);
    out += ": expected ";
    out += expr;

    if (!message.empty()) {
      out += " | ";
      out += message;
    }

    result.failures.push_back(out);
  }

  void ExpectTrue(
    bool value,
    const char* expr,
    const char* file,
    int line) {
    if (!value) {
      Fail(expr, file, line);
    }
  }

  void ExpectFalse(
    bool value,
    const char* expr,
    const char* file,
    int line) {
    if (value) {
      Fail(expr, file, line);
    }
  }

  template<typename A, typename B>
  void ExpectEq(
    const A& actual,
    const B& expected,
    const char* actualExpr,
    const char* expectedExpr,
    const char* file,
    int line) {
    if (!(actual == expected)) {
      std::string msg;
      msg += actualExpr;
      msg += " != ";
      msg += expectedExpr;
      msg += " | actual=";
      msg += std::to_string(actual);
      msg += " expected=";
      msg += std::to_string(expected);
      Fail("equality", file, line, msg);
    }
  }

  void ExpectNear(
    float actual,
    float expected,
    float epsilon,
    const char* actualExpr,
    const char* expectedExpr,
    const char* file,
    int line) {
    if (std::fabs(actual - expected) > epsilon) {
      std::string msg;
      msg += actualExpr;
      msg += " != ";
      msg += expectedExpr;
      msg += " | actual=";
      msg += std::to_string(actual);
      msg += " expected=";
      msg += std::to_string(expected);
      msg += " epsilon=";
      msg += std::to_string(epsilon);
      Fail("near equality", file, line, msg);
    }
  }
};

using TestFn = std::function<void(TestContext&)>;

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

} // namespace Test

#define TEST_CASE(name)                                            \
  static void name(Test::TestContext& t);                          \
  struct name##_registrar {                                        \
    name##_registrar() { Test::Register(#name, name); }            \
  };                                                               \
  static name##_registrar name##_registrar_instance;               \
  static void name(Test::TestContext& t)

#define EXPECT_TRUE(expr) t.ExpectTrue((expr), #expr, __FILE__, __LINE__)
#define EXPECT_FALSE(expr) t.ExpectFalse((expr), #expr, __FILE__, __LINE__)
#define EXPECT_EQ(actual, expected) t.ExpectEq((actual), (expected), #actual, #expected, __FILE__, __LINE__)
#define EXPECT_NEAR(actual, expected, epsilon) t.ExpectNear((actual), (expected), (epsilon), #actual, #expected, __FILE__, __LINE__)

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static bool HasVisualChild(Control& parent, Control* child) {
  for (auto* existing : parent.Children) {
    if (existing == child) {
      return true;
    }
  }

  return false;
}

static int CountVisualChild(Control& parent, Control* child) {
  int count = 0;

  for (auto* existing : parent.Children) {
    if (existing == child) {
      ++count;
    }
  }

  return count;
}

static void LayoutControl(Control& control) {
  // Your DoLayout is probably protected on some controls.
  // If this does not compile, either:
  // 1. expose a public PerformLayoutForTest() helper, or
  // 2. call whatever your framework normally calls for layout.
  control.DoLayout(nullptr);
}

// -----------------------------------------------------------------------------
// Ownership tests
// -----------------------------------------------------------------------------

TEST_CASE(Control_CreateControl_OwnsAndAttachesChild) {
  Panel parent;

  Button* child = parent.CreateControl<Button>();

  EXPECT_TRUE(child != nullptr);
  EXPECT_TRUE(child->owner == &parent);
  EXPECT_TRUE(child->parent == &parent);
  EXPECT_TRUE(HasVisualChild(parent, child));
  EXPECT_EQ(CountVisualChild(parent, child), 1);
}

TEST_CASE(Control_CreateControl_DoesNotDoubleAttachSameChild) {
  Panel parent;

  Button* child = parent.CreateControl<Button>();

  // This should either safely no-op or refuse, depending on your API.
  // If AttachControl is private, delete this test or test the public path.
  parent.AttachControl(child);

  EXPECT_EQ(CountVisualChild(parent, child), 1);
}

TEST_CASE(Control_CreateOwnedControl_OwnsButDoesNotAttachChild) {
  Panel parent;

  Button* child = parent.CreateOwnedControl<Button>();

  EXPECT_TRUE(child != nullptr);
  EXPECT_TRUE(child->owner == &parent);
  EXPECT_TRUE(child->parent == nullptr);
  EXPECT_FALSE(HasVisualChild(parent, child));
}

TEST_CASE(Control_AttachOwnedControl_AttachesAfterCreateOwnedControl) {
  Panel parent;

  Button* child = parent.CreateOwnedControl<Button>();

  parent.AttachControl(child);

  EXPECT_TRUE(child->owner == &parent);
  EXPECT_TRUE(child->parent == &parent);
  EXPECT_TRUE(HasVisualChild(parent, child));
  EXPECT_EQ(CountVisualChild(parent, child), 1);
}

TEST_CASE(Control_RemoveControl_RemovesVisualChild) {
  Panel parent;

  Button* child = parent.CreateControl<Button>();

  EXPECT_TRUE(HasVisualChild(parent, child));

  parent.RemoveControl(child);

  EXPECT_FALSE(HasVisualChild(parent, child));
}

TEST_CASE(Control_RemoveControl_NullptrIsSafe) {
  Panel parent;

  parent.RemoveControl(nullptr);

  EXPECT_EQ((int)parent.Children.size(), 0);
}

TEST_CASE(Control_ClearControls_RemovesAllVisualChildren) {
  Panel parent;

  Button* a = parent.CreateControl<Button>();
  Label* b = parent.CreateControl<Label>();
  Panel* c = parent.CreateControl<Panel>();

  EXPECT_TRUE(HasVisualChild(parent, a));
  EXPECT_TRUE(HasVisualChild(parent, b));
  EXPECT_TRUE(HasVisualChild(parent, c));

  parent.ClearControls();

  EXPECT_EQ((int)parent.Children.size(), 0);
}

TEST_CASE(Control_CreateControl_AssignsParentToNestedChildren) {
  Panel root;
  Panel* childPanel = root.CreateControl<Panel>();
  Button* nestedButton = childPanel->CreateControl<Button>();

  EXPECT_TRUE(childPanel->owner == &root);
  EXPECT_TRUE(childPanel->parent == &root);

  EXPECT_TRUE(nestedButton->owner == childPanel);
  EXPECT_TRUE(nestedButton->parent == childPanel);

  EXPECT_TRUE(HasVisualChild(root, childPanel));
  EXPECT_TRUE(HasVisualChild(*childPanel, nestedButton));
}

TEST_CASE(Control_CreateOwnedNestedControl_CanAttachLater) {
  Panel root;
  Panel* childPanel = root.CreateControl<Panel>();
  Button* nestedButton = childPanel->CreateOwnedControl<Button>();

  EXPECT_TRUE(nestedButton->owner == childPanel);
  EXPECT_TRUE(nestedButton->parent == nullptr);
  EXPECT_FALSE(HasVisualChild(*childPanel, nestedButton));

  childPanel->AttachControl(nestedButton);

  EXPECT_TRUE(nestedButton->parent == childPanel);
  EXPECT_TRUE(HasVisualChild(*childPanel, nestedButton));
}

TEST_CASE(Control_RemovingChildDoesNotRemoveUnrelatedChildren) {
  Panel parent;

  Button* a = parent.CreateControl<Button>();
  Button* b = parent.CreateControl<Button>();
  Button* c = parent.CreateControl<Button>();

  parent.RemoveControl(b);

  EXPECT_TRUE(HasVisualChild(parent, a));
  EXPECT_FALSE(HasVisualChild(parent, b));
  EXPECT_TRUE(HasVisualChild(parent, c));
}

// -----------------------------------------------------------------------------
// Basic layout/grid tests
// -----------------------------------------------------------------------------

TEST_CASE(GridLayout_OneFillCell_FillsWholePanel) {
  GridLayoutPanel grid;

  grid.SetSize(300.0f, 200.0f);

  grid.SetGridColumns({
    LayoutLength::Fill()
  });

  grid.SetGridRows({
    LayoutLength::Fill()
  });

  Panel* child = grid.CreateLayoutChild<Panel>(
    {0, 0, 1, 1}
  );

  LayoutControl(grid);

  EXPECT_NEAR(child->location.x, 0.0f, 0.01f);
  EXPECT_NEAR(child->location.y, 0.0f, 0.01f);
  EXPECT_NEAR(child->size.width, 300.0f, 0.01f);
  EXPECT_NEAR(child->size.height, 200.0f, 0.01f);
}

TEST_CASE(GridLayout_FixedTopRow_FillBody) {
  GridLayoutPanel grid;

  grid.SetSize(320.0f, 240.0f);

  grid.SetGridColumns({
    LayoutLength::Fill()
  });

  grid.SetGridRows({
    LayoutLength::Fixed(70.0f),
    LayoutLength::Fill()
  });

  Panel* top = grid.CreateLayoutChild<Panel>(
    {0, 0, 1, 1}
  );

  Panel* body = grid.CreateLayoutChild<Panel>(
    {0, 1, 1, 1}
  );

  LayoutControl(grid);

  EXPECT_NEAR(top->location.x, 0.0f, 0.01f);
  EXPECT_NEAR(top->location.y, 0.0f, 0.01f);
  EXPECT_NEAR(top->size.width, 320.0f, 0.01f);
  EXPECT_NEAR(top->size.height, 70.0f, 0.01f);

  EXPECT_NEAR(body->location.x, 0.0f, 0.01f);
  EXPECT_NEAR(body->location.y, 70.0f, 0.01f);
  EXPECT_NEAR(body->size.width, 320.0f, 0.01f);
  EXPECT_NEAR(body->size.height, 170.0f, 0.01f);
}

TEST_CASE(GridLayout_FixedLeftColumn_FillMainColumn) {
  GridLayoutPanel grid;

  grid.SetSize(400.0f, 200.0f);

  grid.SetGridColumns({
    LayoutLength::Fixed(160.0f),
    LayoutLength::Fill()
  });

  grid.SetGridRows({
    LayoutLength::Fill()
  });

  Panel* left = grid.CreateLayoutChild<Panel>(
    {0, 0, 1, 1}
  );

  Panel* main = grid.CreateLayoutChild<Panel>(
    {1, 0, 1, 1}
  );

  LayoutControl(grid);

  EXPECT_NEAR(left->location.x, 0.0f, 0.01f);
  EXPECT_NEAR(left->location.y, 0.0f, 0.01f);
  EXPECT_NEAR(left->size.width, 160.0f, 0.01f);
  EXPECT_NEAR(left->size.height, 200.0f, 0.01f);

  EXPECT_NEAR(main->location.x, 160.0f, 0.01f);
  EXPECT_NEAR(main->location.y, 0.0f, 0.01f);
  EXPECT_NEAR(main->size.width, 240.0f, 0.01f);
  EXPECT_NEAR(main->size.height, 200.0f, 0.01f);
}

TEST_CASE(GridLayout_TopBarSpansTwoColumns) {
  GridLayoutPanel grid;

  grid.SetSize(500.0f, 300.0f);

  grid.SetGridColumns({
    LayoutLength::Fixed(150.0f),
    LayoutLength::Fill()
  });

  grid.SetGridRows({
    LayoutLength::Fixed(50.0f),
    LayoutLength::Fill()
  });

  Panel* top = grid.CreateLayoutChild<Panel>(
    {0, 0, 2, 1}
  );

  Panel* left = grid.CreateLayoutChild<Panel>(
    {0, 1, 1, 1}
  );

  Panel* main = grid.CreateLayoutChild<Panel>(
    {1, 1, 1, 1}
  );

  LayoutControl(grid);

  EXPECT_NEAR(top->location.x, 0.0f, 0.01f);
  EXPECT_NEAR(top->location.y, 0.0f, 0.01f);
  EXPECT_NEAR(top->size.width, 500.0f, 0.01f);
  EXPECT_NEAR(top->size.height, 50.0f, 0.01f);

  EXPECT_NEAR(left->location.x, 0.0f, 0.01f);
  EXPECT_NEAR(left->location.y, 50.0f, 0.01f);
  EXPECT_NEAR(left->size.width, 150.0f, 0.01f);
  EXPECT_NEAR(left->size.height, 250.0f, 0.01f);

  EXPECT_NEAR(main->location.x, 150.0f, 0.01f);
  EXPECT_NEAR(main->location.y, 50.0f, 0.01f);
  EXPECT_NEAR(main->size.width, 350.0f, 0.01f);
  EXPECT_NEAR(main->size.height, 250.0f, 0.01f);
}

TEST_CASE(GridLayout_FillWeights_DistributeRemainingSpace) {
  GridLayoutPanel grid;

  grid.SetSize(300.0f, 100.0f);

  grid.SetGridColumns({
    LayoutLength::Fill(1.0f),
    LayoutLength::Fill(2.0f)
  });

  grid.SetGridRows({
    LayoutLength::Fill()
  });

  Panel* a = grid.CreateLayoutChild<Panel>(
    {0, 0, 1, 1}
  );

  Panel* b = grid.CreateLayoutChild<Panel>(
    {1, 0, 1, 1}
  );

  LayoutControl(grid);

  EXPECT_NEAR(a->location.x, 0.0f, 0.01f);
  EXPECT_NEAR(a->size.width, 100.0f, 0.01f);

  EXPECT_NEAR(b->location.x, 100.0f, 0.01f);
  EXPECT_NEAR(b->size.width, 200.0f, 0.01f);
}

TEST_CASE(GridLayout_FixedAndFillWeights_DistributeRemainingSpace) {
  GridLayoutPanel grid;

  grid.SetSize(500.0f, 100.0f);

  grid.SetGridColumns({
    LayoutLength::Fixed(100.0f),
    LayoutLength::Fill(1.0f),
    LayoutLength::Fill(3.0f)
  });

  grid.SetGridRows({
    LayoutLength::Fill()
  });

  Panel* fixed = grid.CreateLayoutChild<Panel>(
    {0, 0, 1, 1}
  );

  Panel* a = grid.CreateLayoutChild<Panel>(
    {1, 0, 1, 1}
  );

  Panel* b = grid.CreateLayoutChild<Panel>(
    {2, 0, 1, 1}
  );

  LayoutControl(grid);

  EXPECT_NEAR(fixed->location.x, 0.0f, 0.01f);
  EXPECT_NEAR(fixed->size.width, 100.0f, 0.01f);

  EXPECT_NEAR(a->location.x, 100.0f, 0.01f);
  EXPECT_NEAR(a->size.width, 100.0f, 0.01f);

  EXPECT_NEAR(b->location.x, 200.0f, 0.01f);
  EXPECT_NEAR(b->size.width, 300.0f, 0.01f);
}

TEST_CASE(GridLayout_PercentColumn_UsesParentWidth) {
  GridLayoutPanel grid;

  grid.SetSize(400.0f, 100.0f);

  grid.SetGridColumns({
    LayoutLength::Percent(0.25f),
    LayoutLength::Fill()
  });

  grid.SetGridRows({
    LayoutLength::Fill()
  });

  Panel* quarter = grid.CreateLayoutChild<Panel>(
    {0, 0, 1, 1}
  );

  Panel* rest = grid.CreateLayoutChild<Panel>(
    {1, 0, 1, 1}
  );

  LayoutControl(grid);

  EXPECT_NEAR(quarter->location.x, 0.0f, 0.01f);
  EXPECT_NEAR(quarter->size.width, 100.0f, 0.01f);

  EXPECT_NEAR(rest->location.x, 100.0f, 0.01f);
  EXPECT_NEAR(rest->size.width, 300.0f, 0.01f);
}

TEST_CASE(GridLayout_ClampsSpansToAvailableGrid) {
  GridLayoutPanel grid;

  grid.SetSize(300.0f, 200.0f);

  grid.SetGridColumns({
    LayoutLength::Fixed(100.0f),
    LayoutLength::Fixed(100.0f),
    LayoutLength::Fixed(100.0f)
  });

  grid.SetGridRows({
    LayoutLength::Fixed(100.0f),
    LayoutLength::Fixed(100.0f)
  });

  Panel* child = grid.CreateLayoutChild<Panel>(
    {1, 0, 99, 99}
  );

  LayoutControl(grid);

  EXPECT_NEAR(child->location.x, 100.0f, 0.01f);
  EXPECT_NEAR(child->location.y, 0.0f, 0.01f);
  EXPECT_NEAR(child->size.width, 200.0f, 0.01f);
  EXPECT_NEAR(child->size.height, 200.0f, 0.01f);
}

TEST_CASE(GridLayout_ZeroOrNegativeSpan_BecomesOne) {
  GridLayoutPanel grid;

  grid.SetSize(300.0f, 100.0f);

  grid.SetGridColumns({
    LayoutLength::Fixed(100.0f),
    LayoutLength::Fixed(100.0f),
    LayoutLength::Fixed(100.0f)
  });

  grid.SetGridRows({
    LayoutLength::Fill()
  });

  Panel* child = grid.CreateLayoutChild<Panel>(
    {1, 0, 0, -10}
  );

  LayoutControl(grid);

  EXPECT_NEAR(child->location.x, 100.0f, 0.01f);
  EXPECT_NEAR(child->size.width, 100.0f, 0.01f);
}

TEST_CASE(GridLayout_EmptyRowsOrColumns_DoesNotCrash) {
  GridLayoutPanel grid;

  grid.SetSize(300.0f, 100.0f);

  Panel* child = grid.CreateControl<Panel>();

  LayoutControl(grid);

  EXPECT_TRUE(child != nullptr);
}

// -----------------------------------------------------------------------------
// Style/lifecycle-ish regression tests
// -----------------------------------------------------------------------------

TEST_CASE(Control_CanCreateChildThenStyleIt) {
  Panel parent;

  Label* label = parent.CreateControl<Label>();

  label->sContent = "hello";
  label->isHitTestVisible = false;

  EXPECT_TRUE(label->owner == &parent);
  EXPECT_TRUE(label->parent == &parent);
  EXPECT_TRUE(label->sContent == "hello");
  EXPECT_FALSE(label->isHitTestVisible);
}

TEST_CASE(Control_CreatedOwnedControlCanBeConfiguredBeforeAttach) {
  Panel parent;

  Label* label = parent.CreateOwnedControl<Label>();

  label->sContent = "detached";
  label->isVisible = false;

  EXPECT_TRUE(label->owner == &parent);
  EXPECT_TRUE(label->parent == nullptr);
  EXPECT_FALSE(HasVisualChild(parent, label));

  parent.AttachControl(label);

  EXPECT_TRUE(label->parent == &parent);
  EXPECT_TRUE(HasVisualChild(parent, label));
  EXPECT_TRUE(label->sContent == "detached");
  EXPECT_FALSE(label->isVisible);
}

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------

int main() {
  int passed = 0;
  int failed = 0;

  std::printf("Running %zu DirectUI tests...\n\n", Test::Registry().size());

  for (const auto& test : Test::Registry()) {
    Test::TestContext context(test.name);

    try {
      test.fn(context);
    } catch (const std::exception& ex) {
      context.result.passed = false;
      context.result.failures.push_back(std::string("Unhandled std::exception: ") + ex.what());
    } catch (...) {
      context.result.passed = false;
      context.result.failures.push_back("Unhandled unknown exception");
    }

    if (context.result.passed) {
      ++passed;
      std::printf("[PASS] %s\n", context.result.name.c_str());
    } else {
      ++failed;
      std::printf("[FAIL] %s\n", context.result.name.c_str());

      for (const auto& failure : context.result.failures) {
        std::printf("       %s\n", failure.c_str());
      }
    }
  }

  std::printf("\nResult: %d passed, %d failed\n", passed, failed);

  return failed == 0 ? 0 : 1;
}
