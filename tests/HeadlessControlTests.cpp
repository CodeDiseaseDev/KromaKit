#include <cstdio>
#include <string>

#include <kromakit/Label.h>
#include <kromakit/Panel.h>
#include <kromakit/gesture/hold/HoldInteraction.h>
#include <kromakit/context_menu/ContextMenu.h>
#include <kromakit/layout/layout_options/LayoutLength.h>
#include "../synthem/editor/ISequence.h"
#define private public
#include <kromakit/grid_layout_panel/GridLayoutPanel.h>
#undef private
#include "TestHarness.h"

namespace {

constexpr int kImmediateHoldThresholdMs = 0;

class PointerCounterControl final : public Panel {
public:
  int pointerDownCount = 0;
  int pointerMoveCount = 0;
  int pointerUpCount = 0;
  int pointerCancelCount = 0;
  int mouseDownCount = 0;
  int mouseUpCount = 0;

  void OnPointerDown(const PointerEvent& e) override {
    (void)e;
    ++pointerDownCount;
  }

  void OnPointerMove(const PointerEvent& e) override {
    (void)e;
    ++pointerMoveCount;
  }

  void OnPointerUp(const PointerEvent& e) override {
    (void)e;
    ++pointerUpCount;
  }

  void OnPointerCancel(const PointerEvent& e) override {
    (void)e;
    ++pointerCancelCount;
  }

  void OnMouseDown() override {
    ++mouseDownCount;
  }

  void OnMouseUp() override {
    ++mouseUpCount;
  }
};

class SequenceViewportProbe final : public ISequence {
public:
  void SetProbeSize(float width, float height) {
    size = { width, height };
  }

  float ProbeBodyWidth() const {
    return BodyWidth();
  }

  float ProbeBodyHeight() const {
    return BodyHeight();
  }

  float ProbeGridWidth() const {
    return GridWidth();
  }

  float ProbeGridHeight() const {
    return GridHeight();
  }

  float ProbeBeatToX(double beat) const {
    return BeatToX(beat);
  }

  double ProbeXToBeat(float x) const {
    return XToBeat(x);
  }

  void ProbeSetViewportOffset(float x, float y) {
    SetViewportOffset({ x, y });
  }
};

void LogGridState(const char* tag, GridLayoutPanel& grid) {
  std::printf(
    "[%s] Children.size=%zu layoutChildren.size=%zu\n",
    tag,
    grid.Children.size(),
    grid.layoutChildren.size());

  for (size_t i = 0; i < grid.Children.size(); ++i) {
    Control* child = grid.Children[i];
    std::printf(
      "[%s] Children[%zu]=%p parent=%p loc=(%.1f,%.1f) size=(%.1f,%.1f)\n",
      tag,
      i,
      static_cast<void*>(child),
      child != nullptr ? static_cast<void*>(child->parent) : nullptr,
      child != nullptr ? child->location.x : -1.0f,
      child != nullptr ? child->location.y : -1.0f,
      child != nullptr ? child->size.width : -1.0f,
      child != nullptr ? child->size.height : -1.0f);
  }

  for (size_t i = 0; i < grid.layoutChildren.size(); ++i) {
    const LayoutItem& item = grid.layoutChildren[i];
    std::printf(
      "[%s] layoutChildren[%zu] control=%p row=%d column=%d rowSpan=%d columnSpan=%d\n",
      tag,
      i,
      static_cast<void*>(item.control),
      item.options.gridLayout.row,
      item.options.gridLayout.column,
      item.options.gridLayout.rowSpan,
      item.options.gridLayout.columnSpan);
  }
}

}

TEST_CASE(LabelInitialState) {
  Label label;
  EXPECT_EQ(label.sContent, std::string(""));
}

TEST_CASE(LabelContentMutation) {
  Label label;
  label.SetContent("hello");
  EXPECT_EQ(label.sContent, std::string("hello"));
}

TEST_CASE(LabelGeometryMutation) {
  Label label;
  label.SetSize(120.0f, 24.0f);
  label.location = {10.0f, 20.0f};

  EXPECT_EQ(label.size.width, 120.0f);
  EXPECT_EQ(label.size.height, 24.0f);
  EXPECT_EQ(label.location.x, 10.0f);
  EXPECT_EQ(label.location.y, 20.0f);
}

TEST_CASE(LabelVisibilityFlags) {
  Label label;
  label.isVisible = true;
  label.isHitTestVisible = true;

  EXPECT_TRUE(label.isVisible);
  EXPECT_TRUE(label.isHitTestVisible);
}

TEST_CASE(PanelConstructDestroy) {
  {
    Panel panel;
    EXPECT_TRUE(panel.GetControlName() == "Panel");
  }
  EXPECT_TRUE(true);
}

TEST_CASE(PanelGeometryMutation) {
  Panel panel;
  panel.SetSize(240.0f, 80.0f);
  panel.location = {30.0f, 40.0f};

  EXPECT_EQ(panel.size.width, 240.0f);
  EXPECT_EQ(panel.size.height, 80.0f);
  EXPECT_EQ(panel.location.x, 30.0f);
  EXPECT_EQ(panel.location.y, 40.0f);
}

TEST_CASE(PanelVisibilityFlags) {
  Panel panel;
  panel.isVisible = false;
  panel.isHitTestVisible = false;
  EXPECT_FALSE(panel.isVisible);
  EXPECT_FALSE(panel.isHitTestVisible);

  panel.isVisible = true;
  panel.isHitTestVisible = true;
  EXPECT_TRUE(panel.isVisible);
  EXPECT_TRUE(panel.isHitTestVisible);
}

TEST_CASE(PanelChildHierarchyWithLabel) {
  Panel panel;
  Label* child = panel.CreateControl<Label>();

  EXPECT_TRUE(child != nullptr);
  EXPECT_EQ(panel.Children.size(), static_cast<size_t>(1));
  EXPECT_EQ(panel.ownedChildren.size(), static_cast<size_t>(1));
  EXPECT_TRUE(child != nullptr && child->parent == &panel);
  EXPECT_TRUE(child != nullptr && child->owner == &panel);

  panel.RemoveControl(child);
  EXPECT_EQ(panel.Children.size(), static_cast<size_t>(0));
  EXPECT_EQ(panel.ownedChildren.size(), static_cast<size_t>(0));
}

TEST_CASE(HoldInteraction_PointerDownUpWithinThreshold_IsTap) {
  HoldInteraction interaction;
  interaction.SetHoldThresholdMs(1000);

  interaction.PointerDown();

  const HoldInteraction::Result result = interaction.PointerUp();
  EXPECT_EQ(result, HoldInteraction::Result::Tap);
}

TEST_CASE(HoldInteraction_ZeroThreshold_ReturnsHoldOnPointerUp) {
  HoldInteraction interaction;
  interaction.SetHoldThresholdMs(0);

  interaction.PointerDown();

  const HoldInteraction::Result result = interaction.PointerUp();
  EXPECT_EQ(result, HoldInteraction::Result::Hold);
}

TEST_CASE(HoldInteraction_ConsumeHoldIfReady_ConsumesOnce) {
  HoldInteraction interaction;
  interaction.SetHoldThresholdMs(kImmediateHoldThresholdMs);

  interaction.PointerDown();

  EXPECT_TRUE(interaction.IsHolding());
  EXPECT_TRUE(interaction.ConsumeHoldIfReady());
  EXPECT_FALSE(interaction.IsHolding());
  EXPECT_FALSE(interaction.ConsumeHoldIfReady());
  EXPECT_EQ(interaction.PointerUp(), HoldInteraction::Result::Hold);
}

TEST_CASE(HoldInteraction_Cancel_ResetsState) {
  HoldInteraction interaction;
  interaction.SetHoldThresholdMs(kImmediateHoldThresholdMs);
  interaction.PointerDown();
  EXPECT_TRUE(interaction.IsPointerDown());

  interaction.Cancel();

  EXPECT_FALSE(interaction.IsPointerDown());
  EXPECT_EQ(interaction.PointerUp(), HoldInteraction::Result::None);
}

TEST_CASE(HoldInteraction_PointerUpWithoutPointerDown_IsNone) {
  HoldInteraction interaction;
  EXPECT_EQ(interaction.PointerUp(), HoldInteraction::Result::None);
}

TEST_CASE(HoldInteraction_ConsumeWithoutPointerDown_IsFalse) {
  HoldInteraction interaction;
  EXPECT_FALSE(interaction.ConsumeHoldIfReady());
}

TEST_CASE(HoldInteraction_CancelAfterConsume_ClearsHoldAndDownState) {
  HoldInteraction interaction;
  interaction.SetHoldThresholdMs(kImmediateHoldThresholdMs);
  interaction.PointerDown();

  EXPECT_TRUE(interaction.ConsumeHoldIfReady());
  interaction.Cancel();

  EXPECT_FALSE(interaction.IsPointerDown());
  EXPECT_FALSE(interaction.IsHolding());
  EXPECT_FALSE(interaction.ConsumeHoldIfReady());
}

TEST_CASE(Control_ContextMenuProvider_SetClearAndPresence) {
  Panel panel;

  EXPECT_FALSE(panel.HasContextMenuProvider());
  EXPECT_EQ(panel.GetContextMenuHoldThresholdMs(), 450);

  panel.SetContextMenuProvider([] {
    return std::unique_ptr<ContextMenu> {};
  });

  EXPECT_TRUE(panel.HasContextMenuProvider());

  panel.ClearContextMenuProvider();
  EXPECT_FALSE(panel.HasContextMenuProvider());
}

TEST_CASE(Control_ContextMenuProvider_IsPerInstance) {
  Panel a;
  Panel b;

  a.SetContextMenuProvider([] {
    return std::unique_ptr<ContextMenu> {};
  });

  EXPECT_TRUE(a.HasContextMenuProvider());
  EXPECT_FALSE(b.HasContextMenuProvider());

  b.SetContextMenuProvider([] {
    return std::unique_ptr<ContextMenu> {};
  });

  EXPECT_TRUE(a.HasContextMenuProvider());
  EXPECT_TRUE(b.HasContextMenuProvider());

  a.ClearContextMenuProvider();
  EXPECT_FALSE(a.HasContextMenuProvider());
  EXPECT_TRUE(b.HasContextMenuProvider());
}

TEST_CASE(Control_ContextMenuProvider_HoldThreshold_PerInstance) {
  Panel a;
  Panel b;

  EXPECT_EQ(a.GetContextMenuHoldThresholdMs(), 450);
  EXPECT_EQ(b.GetContextMenuHoldThresholdMs(), 450);

  a.SetContextMenuHoldThresholdMs(120);

  EXPECT_EQ(a.GetContextMenuHoldThresholdMs(), 120);
  EXPECT_EQ(b.GetContextMenuHoldThresholdMs(), 450);
}

TEST_CASE(Control_ContextMenuHoldTracking_BeginConsumeAndEnd) {
  Panel panel;
  panel.SetContextMenuProvider([] {
    return std::unique_ptr<ContextMenu> {};
  });
  panel.SetContextMenuHoldThresholdMs(kImmediateHoldThresholdMs);

  EXPECT_TRUE(panel.BeginContextMenuHoldTracking());
  EXPECT_TRUE(panel.TryConsumeContextMenuHoldIfReady());
  EXPECT_FALSE(panel.TryConsumeContextMenuHoldIfReady());
  panel.EndContextMenuHoldTracking();
}

TEST_CASE(Control_ContextMenuHoldTracking_CancelResets) {
  Panel panel;
  panel.SetContextMenuProvider([] {
    return std::unique_ptr<ContextMenu> {};
  });
  panel.SetContextMenuHoldThresholdMs(kImmediateHoldThresholdMs);

  EXPECT_TRUE(panel.BeginContextMenuHoldTracking());
  panel.CancelContextMenuHoldTracking();
  EXPECT_FALSE(panel.TryConsumeContextMenuHoldIfReady());
}

TEST_CASE(Control_ContextMenuHoldTracking_BeginWithoutProviderFails) {
  Panel panel;
  EXPECT_FALSE(panel.BeginContextMenuHoldTracking());
}

TEST_CASE(Control_ContextMenuProvider_CreateMenuWithoutProvider_IsNull) {
  Panel panel;
  auto menu = panel.CreateContextMenuFromProvider();
  EXPECT_TRUE(menu == nullptr);
}

TEST_CASE(Control_ContextMenuProvider_CreateMenuInvokesProviderPerCall) {
  Panel panel;
  int createMenuCount = 0;
  panel.SetContextMenuProvider([&createMenuCount] {
    ++createMenuCount;
    return std::unique_ptr<ContextMenu> {};
  });

  auto menu1 = panel.CreateContextMenuFromProvider();
  auto menu2 = panel.CreateContextMenuFromProvider();

  EXPECT_TRUE(menu1 == nullptr);
  EXPECT_TRUE(menu2 == nullptr);
  EXPECT_EQ(createMenuCount, 2);
}

TEST_CASE(Control_ContextMenuProvider_ClearAfterPointerDown_CancelsHoldSafely) {
  Panel panel;
  panel.SetContextMenuProvider([] {
    return std::unique_ptr<ContextMenu> {};
  });
  panel.SetContextMenuHoldThresholdMs(kImmediateHoldThresholdMs);

  EXPECT_TRUE(panel.BeginContextMenuHoldTracking());
  panel.ClearContextMenuProvider();

  EXPECT_FALSE(panel.HasContextMenuProvider());
  EXPECT_FALSE(panel.TryConsumeContextMenuHoldIfReady());
  EXPECT_TRUE(panel.CreateContextMenuFromProvider() == nullptr);
}

TEST_CASE(Control_ContextMenuProvider_ChangeProvider_BetweenHoldsUsesLatest) {
  Panel panel;
  panel.SetContextMenuHoldThresholdMs(kImmediateHoldThresholdMs);

  int firstProviderCalls = 0;
  int secondProviderCalls = 0;

  panel.SetContextMenuProvider([&firstProviderCalls] {
    ++firstProviderCalls;
    return std::unique_ptr<ContextMenu> {};
  });
  auto firstMenu = panel.CreateContextMenuFromProvider();
  EXPECT_TRUE(firstMenu == nullptr);
  EXPECT_EQ(firstProviderCalls, 1);
  EXPECT_EQ(secondProviderCalls, 0);

  panel.SetContextMenuProvider([&secondProviderCalls] {
    ++secondProviderCalls;
    return std::unique_ptr<ContextMenu> {};
  });
  auto secondMenu = panel.CreateContextMenuFromProvider();
  EXPECT_TRUE(secondMenu == nullptr);
  EXPECT_EQ(firstProviderCalls, 1);
  EXPECT_EQ(secondProviderCalls, 1);
}

TEST_CASE(Control_ContextMenuHoldTracking_PerInstanceIsolation) {
  Panel a;
  Panel b;
  a.SetContextMenuProvider([] {
    return std::unique_ptr<ContextMenu> {};
  });
  b.SetContextMenuProvider([] {
    return std::unique_ptr<ContextMenu> {};
  });
  a.SetContextMenuHoldThresholdMs(kImmediateHoldThresholdMs);
  b.SetContextMenuHoldThresholdMs(kImmediateHoldThresholdMs);

  EXPECT_TRUE(a.BeginContextMenuHoldTracking());
  EXPECT_TRUE(a.TryConsumeContextMenuHoldIfReady());
  EXPECT_FALSE(a.TryConsumeContextMenuHoldIfReady());

  EXPECT_TRUE(b.BeginContextMenuHoldTracking());
  EXPECT_TRUE(b.TryConsumeContextMenuHoldIfReady());
  EXPECT_FALSE(b.TryConsumeContextMenuHoldIfReady());
}

TEST_CASE(Control_PointerEventOverrides_TrackCounts) {
  PointerCounterControl control;
  PointerEvent e {};

  control.OnPointerDown(e);
  control.OnPointerMove(e);
  control.OnPointerUp(e);
  control.OnPointerCancel(e);
  control.OnMouseDown();
  control.OnMouseUp();

  EXPECT_EQ(control.pointerDownCount, 1);
  EXPECT_EQ(control.pointerMoveCount, 1);
  EXPECT_EQ(control.pointerUpCount, 1);
  EXPECT_EQ(control.pointerCancelCount, 1);
  EXPECT_EQ(control.mouseDownCount, 1);
  EXPECT_EQ(control.mouseUpCount, 1);
}

TEST_CASE(ISequence_DefaultViewportMode_IsLegacy) {
  SequenceViewportProbe sequence;
  EXPECT_FALSE(sequence.IsUsingModernViewportScroll());
}

TEST_CASE(ISequence_ModernViewportOffset_ClampsToGridBounds) {
  SequenceViewportProbe sequence;
  sequence.SetProbeSize(400.0f, 200.0f);
  sequence.SetViewportScrollMode(ISequence::ViewportScrollMode::ModernScrollOffset);
  sequence.SetTotalBeats(16.0f);
  sequence.SetTotalRows(12);

  sequence.ProbeSetViewportOffset(99999.0f, 99999.0f);

  const DUIPoint offset = sequence.GetViewportOffset();
  const float expectedMaxX = std::max(0.0f, sequence.ProbeGridWidth() - sequence.ProbeBodyWidth());
  const float expectedMaxY = std::max(0.0f, sequence.ProbeGridHeight() - sequence.ProbeBodyHeight());

  EXPECT_NEAR(offset.x, expectedMaxX, 0.001f);
  EXPECT_NEAR(offset.y, expectedMaxY, 0.001f);

  // Compatibility mirror should track modern source-of-truth during transition.
  EXPECT_NEAR(sequence.viewOffsetX, offset.x, 0.001f);
  EXPECT_NEAR(sequence.viewOffsetY, offset.y, 0.001f);
}

TEST_CASE(ISequence_ModernViewportOffset_BeatRoundTrip) {
  SequenceViewportProbe sequence;
  sequence.SetProbeSize(480.0f, 240.0f);
  sequence.SetViewportScrollMode(ISequence::ViewportScrollMode::ModernScrollOffset);
  sequence.SetTotalBeats(24.0f);
  sequence.SetTotalRows(12);
  sequence.ProbeSetViewportOffset(137.5f, 64.0f);

  constexpr double kBeat = 3.75;
  const float x = sequence.ProbeBeatToX(kBeat);
  const double roundTrip = sequence.ProbeXToBeat(x);

  EXPECT_NEAR(roundTrip, kBeat, 0.0001);
}

TEST_CASE(GridLayout_CreateLayoutChild_PositionalOptions_UsesColumnRowOrder) {
  GridLayoutPanel grid;
  grid.SetSize(320.0f, 240.0f);
  grid.SetGridColumns({
    LayoutLength::Fill()
  });
  grid.SetGridRows({
    LayoutLength::Fixed(30.0f),
    LayoutLength::Fill()
  });

  Panel* top = grid.CreateLayoutChild<Panel>(GridLayoutOptions{0, 0, 1, 1});
  Panel* body = grid.CreateLayoutChild<Panel>(GridLayoutOptions{0, 1, 1, 1});

  LogGridState("grid-before-layout", grid);
  EXPECT_TRUE(top != nullptr);
  EXPECT_TRUE(body != nullptr);
  EXPECT_TRUE(top->parent == &grid);
  EXPECT_TRUE(body->parent == &grid);
  EXPECT_EQ(grid.Children.size(), static_cast<size_t>(2));
  EXPECT_EQ(grid.layoutChildren.size(), static_cast<size_t>(2));

  EXPECT_TRUE(grid.layoutChildren[0].control == top);
  EXPECT_EQ(grid.layoutChildren[0].options.gridLayout.column, 0);
  EXPECT_EQ(grid.layoutChildren[0].options.gridLayout.row, 0);
  EXPECT_EQ(grid.layoutChildren[0].options.gridLayout.columnSpan, 1);
  EXPECT_EQ(grid.layoutChildren[0].options.gridLayout.rowSpan, 1);

  EXPECT_TRUE(grid.layoutChildren[1].control == body);
  EXPECT_EQ(grid.layoutChildren[1].options.gridLayout.column, 0);
  EXPECT_EQ(grid.layoutChildren[1].options.gridLayout.row, 1);
  EXPECT_EQ(grid.layoutChildren[1].options.gridLayout.columnSpan, 1);
  EXPECT_EQ(grid.layoutChildren[1].options.gridLayout.rowSpan, 1);

  grid.DoLayout(nullptr);
  LogGridState("grid-after-layout", grid);

  EXPECT_NEAR(top->location.x, 0.0f, 0.01f);
  EXPECT_NEAR(top->location.y, 0.0f, 0.01f);
  EXPECT_NEAR(top->size.width, 320.0f, 0.01f);
  EXPECT_NEAR(top->size.height, 30.0f, 0.01f);

  EXPECT_NEAR(body->location.x, 0.0f, 0.01f);
  EXPECT_NEAR(body->location.y, 30.0f, 0.01f);
  EXPECT_NEAR(body->size.width, 320.0f, 0.01f);
  EXPECT_NEAR(body->size.height, 210.0f, 0.01f);
}

TEST_CASE(GridLayout_CreateLayoutChild_And_AddBorrowedLayoutChild_AreEquivalent) {
  GridLayoutPanel gridA;
  GridLayoutPanel gridB;

  const auto configureGrid = [](GridLayoutPanel& grid) {
    grid.SetSize(320.0f, 240.0f);
    grid.SetGridColumns({
      LayoutLength::Fill()
    });
    grid.SetGridRows({
      LayoutLength::Fixed(30.0f),
      LayoutLength::Fill()
    });
  };

  configureGrid(gridA);
  configureGrid(gridB);

  Panel* a = gridA.CreateControl<Panel>();
  gridA.AddBorrowedLayoutChild(a, 0, 1, 1, 1);

  Panel* b = gridB.CreateLayoutChild<Panel>(GridLayoutOptions{0, 1, 1, 1});

  LogGridState("gridA-before-layout", gridA);
  LogGridState("gridB-before-layout", gridB);

  EXPECT_TRUE(a != nullptr);
  EXPECT_TRUE(b != nullptr);
  EXPECT_EQ(gridA.Children.size(), static_cast<size_t>(1));
  EXPECT_EQ(gridA.layoutChildren.size(), static_cast<size_t>(1));
  EXPECT_EQ(gridB.Children.size(), static_cast<size_t>(1));
  EXPECT_EQ(gridB.layoutChildren.size(), static_cast<size_t>(1));

  EXPECT_TRUE(gridA.layoutChildren[0].control == a);
  EXPECT_TRUE(gridB.layoutChildren[0].control == b);
  EXPECT_EQ(gridA.layoutChildren[0].options.gridLayout.column,
            gridB.layoutChildren[0].options.gridLayout.column);
  EXPECT_EQ(gridA.layoutChildren[0].options.gridLayout.row,
            gridB.layoutChildren[0].options.gridLayout.row);
  EXPECT_EQ(gridA.layoutChildren[0].options.gridLayout.columnSpan,
            gridB.layoutChildren[0].options.gridLayout.columnSpan);
  EXPECT_EQ(gridA.layoutChildren[0].options.gridLayout.rowSpan,
            gridB.layoutChildren[0].options.gridLayout.rowSpan);

  gridA.DoLayout(nullptr);
  gridB.DoLayout(nullptr);

  LogGridState("gridA-after-layout", gridA);
  LogGridState("gridB-after-layout", gridB);

  EXPECT_NEAR(a->location.x, b->location.x, 0.01f);
  EXPECT_NEAR(a->location.y, b->location.y, 0.01f);
  EXPECT_NEAR(a->size.width, b->size.width, 0.01f);
  EXPECT_NEAR(a->size.height, b->size.height, 0.01f);
}
