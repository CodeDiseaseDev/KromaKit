#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <kromakit/graphics/types/DUIInsets.h>
#include <kromakit/graphics/types/DUIPoint.h>
#include <kromakit/graphics/types/DUIRect.h>
#include <kromakit/graphics/types/DUISize.h>
#include <kromakit/styling/DUIStyleName.h>
#include "../synthem/editor/model/Pattern.h"
#include "../synthem/editor/model/PatternPlaylistItem.h"
#include "../synthem/editor/model/PlaylistArrangement.h"
#include "../synthem/editor/model/PlaylistTrack.h"
#include "TestHarness.h"

TEST_CASE(DUIStyleName_NormalizationAndParts) {
  DUIStyleName name("  Panel . Header ");
  EXPECT_FALSE(name.Empty());
  EXPECT_EQ(name.ToString(), std::string("panel.header"));
  EXPECT_EQ(name.Count(), 2);
  EXPECT_EQ(name.Root(), std::string("panel"));
  EXPECT_EQ(name.Leaf(), std::string("header"));
  EXPECT_TRUE(name.StartsWith(std::string_view("panel")));
}

TEST_CASE(DUIStyleName_InvalidPartRejected) {
  DUIStyleName name;
  bool ok = name.Set("panel.$header");
  EXPECT_FALSE(ok);
  EXPECT_TRUE(name.Empty());
}

TEST_CASE(Geometry_InsetsInflateDeflateRoundTrip) {
  DUIRect original(10, 20, 100, 50);
  DUIInsets insets(2, 4, 6, 8);

  DUIRect deflated = insets.DeflateRect(original);
  DUIRect inflated = insets.InflateRect(deflated);

  EXPECT_NEAR(inflated.x, original.x, 0.001f);
  EXPECT_NEAR(inflated.y, original.y, 0.001f);
  EXPECT_NEAR(inflated.width, original.width, 0.001f);
  EXPECT_NEAR(inflated.height, original.height, 0.001f);
}

// Pattern tests

TEST_CASE(Pattern_DefaultConstructionInvariants) {
  Pattern pattern;
  EXPECT_EQ(pattern.name, std::string("Pattern"));
  EXPECT_NEAR(pattern.lengthBeats, 16.0, 0.0001);
  EXPECT_EQ(pattern.notes.size(), static_cast<size_t>(0));

  EXPECT_TRUE(pattern.FindNoteById(123456) == nullptr);
  EXPECT_FALSE(pattern.RemoveNoteById(123456));

  std::vector<ScheduledNoteEvent> events;
  pattern.CollectNoteEvents(events);
  EXPECT_EQ(events.size(), static_cast<size_t>(0));
}

TEST_CASE(Pattern_AddRemoveAndIdUniqueness) {
  Pattern pattern;

  SequenceNote* noteA = pattern.AddNote(60, 0, 0.0, 1.0, 0.5f);
  const int noteAId = noteA != nullptr ? noteA->id : -1;
  SequenceNote* noteB = pattern.AddNote(62, 1, 2.0, 0.5, 0.75f);
  const int noteBId = noteB != nullptr ? noteB->id : -1;

  EXPECT_TRUE(noteA != nullptr);
  EXPECT_TRUE(noteB != nullptr);
  EXPECT_EQ(pattern.notes.size(), static_cast<size_t>(2));
  EXPECT_TRUE(noteAId > 0);
  EXPECT_TRUE(noteBId > noteAId);

  EXPECT_TRUE(pattern.RemoveNoteById(noteAId));
  EXPECT_FALSE(pattern.RemoveNoteById(noteAId));
  EXPECT_TRUE(pattern.FindNoteById(noteAId) == nullptr);
  EXPECT_TRUE(pattern.FindNoteById(noteBId) != nullptr);
  EXPECT_EQ(pattern.notes.size(), static_cast<size_t>(1));
}

TEST_CASE(Pattern_SelectionExclusiveAndNonExclusive) {
  Pattern pattern;
  SequenceNote* noteA = pattern.AddNote(60, 0, 0.0, 1.0, 0.5f);
  const int noteAId = noteA != nullptr ? noteA->id : -1;
  SequenceNote* noteB = pattern.AddNote(62, 0, 1.0, 1.0, 0.5f);
  const int noteBId = noteB != nullptr ? noteB->id : -1;

  EXPECT_TRUE(noteA != nullptr);
  EXPECT_TRUE(noteB != nullptr);

  EXPECT_TRUE(pattern.SelectNote(noteAId, true));
  EXPECT_TRUE(pattern.FindNoteById(noteAId) != nullptr && pattern.FindNoteById(noteAId)->selected);
  EXPECT_TRUE(pattern.FindNoteById(noteBId) != nullptr && !pattern.FindNoteById(noteBId)->selected);

  EXPECT_TRUE(pattern.SelectNote(noteBId, false));
  EXPECT_TRUE(pattern.FindNoteById(noteAId) != nullptr && pattern.FindNoteById(noteAId)->selected);
  EXPECT_TRUE(pattern.FindNoteById(noteBId) != nullptr && pattern.FindNoteById(noteBId)->selected);

  pattern.ClearSelection();
  EXPECT_TRUE(pattern.FindNoteById(noteAId) != nullptr && !pattern.FindNoteById(noteAId)->selected);
  EXPECT_TRUE(pattern.FindNoteById(noteBId) != nullptr && !pattern.FindNoteById(noteBId)->selected);
}

TEST_CASE(Pattern_AddMoveResizeClampsValues) {
  Pattern pattern;
  pattern.lengthBeats = 4.0;

  SequenceNote* note = pattern.AddNote(64, 1, 3.75, 8.0, 2.0f);

  EXPECT_TRUE(note != nullptr);
  EXPECT_NEAR(note->lengthBeats, 4.0, 0.0001);
  EXPECT_NEAR(note->startBeat, 0.0, 0.0001);
  EXPECT_NEAR(note->velocity, 1.0f, 0.0001f);

  bool moved = pattern.MoveNote(note->id, -5.0, 72);
  EXPECT_TRUE(moved);
  EXPECT_NEAR(note->startBeat, 0.0, 0.0001);
  EXPECT_EQ(note->pitch, 72);

  bool resized = pattern.ResizeNote(note->id, 3.9, 0.001);
  EXPECT_TRUE(resized);
  EXPECT_NEAR(note->lengthBeats, 0.0625, 0.0001);
  EXPECT_NEAR(note->startBeat, 3.9, 0.0001);
}

// PlaylistItem / PatternPlaylistItem tests

TEST_CASE(PlaylistItem_BaseClampViaPatternPlaylistItem) {
  PatternPlaylistItem item;
  item.SetStartBeat(-4.0);
  item.SetLengthBeats(0.0);
  item.SetSourceOffsetBeats(-2.0);

  EXPECT_NEAR(item.startBeat, 0.0, 0.0001);
  EXPECT_NEAR(item.lengthBeats, 0.0625, 0.0001);
  EXPECT_NEAR(item.sourceOffsetBeats, 0.0, 0.0001);
  EXPECT_NEAR(item.GetEndBeat(), 0.0625, 0.0001);
}

TEST_CASE(PatternPlaylistItem_ReferencesPattern_NotCopy) {
  auto pattern = std::make_shared<Pattern>();
  PatternPlaylistItem item(pattern, 8.0, 4.0, 0);

  std::vector<ScheduledNoteEvent> eventsBefore;
  item.CollectNoteEvents(eventsBefore);
  EXPECT_EQ(eventsBefore.size(), static_cast<size_t>(0));

  SequenceNote* note = pattern->AddNote(67, 0, 1.0, 2.0, 0.8f);
  EXPECT_TRUE(note != nullptr);

  std::vector<ScheduledNoteEvent> eventsAfter;
  item.CollectNoteEvents(eventsAfter);
  EXPECT_EQ(eventsAfter.size(), static_cast<size_t>(1));
  if (eventsAfter.size() == 1) {
    EXPECT_EQ(eventsAfter[0].pitch, 67);
    EXPECT_NEAR(eventsAfter[0].startBeat, 9.0, 0.0001);
    EXPECT_NEAR(eventsAfter[0].endBeat, 11.0, 0.0001);
  }
}

TEST_CASE(PatternPlaylistItem_ClipAndOffsetWindow) {
  auto pattern = std::make_shared<Pattern>();
  pattern->lengthBeats = 16.0;
  SequenceNote* note = pattern->AddNote(60, 0, 1.0, 4.0, 0.5f);
  EXPECT_TRUE(note != nullptr);

  PatternPlaylistItem item(pattern, 10.0, 2.0, 0);
  item.sourceOffsetBeats = 1.0;

  std::vector<ScheduledNoteEvent> events;
  item.CollectNoteEvents(events);

  EXPECT_EQ(events.size(), static_cast<size_t>(1));
  if (events.size() == 1) {
    EXPECT_NEAR(events[0].startBeat, 10.0, 0.0001);
    EXPECT_NEAR(events[0].endBeat, 12.0, 0.0001);
    EXPECT_NEAR(events[0].lengthBeats, 2.0, 0.0001);
    EXPECT_EQ(events[0].pitch, 60);
  }
}

// PlaylistTrack tests

TEST_CASE(PlaylistTrack_AddFindRemoveOrderAndNullHandling) {
  PlaylistTrack track("Track-1");
  EXPECT_EQ(track.items.size(), static_cast<size_t>(0));

  std::shared_ptr<PlaylistItem> nullItem;
  EXPECT_TRUE(track.AddItem(nullItem) == nullptr);
  EXPECT_EQ(track.items.size(), static_cast<size_t>(0));

  auto pattern = std::make_shared<Pattern>();
  auto itemA = std::make_shared<PatternPlaylistItem>(pattern, 0.0, 1.0, 0);
  auto itemB = std::make_shared<PatternPlaylistItem>(pattern, 2.0, 1.0, 0);

  EXPECT_TRUE(track.AddItem(itemA) == itemA);
  EXPECT_TRUE(track.AddItem(itemB) == itemB);
  EXPECT_EQ(track.items.size(), static_cast<size_t>(2));
  EXPECT_TRUE(track.items[0] == itemA);
  EXPECT_TRUE(track.items[1] == itemB);

  EXPECT_TRUE(track.FindItemById(itemA->id) == itemA);
  EXPECT_TRUE(track.FindItemById(itemB->id) == itemB);
  EXPECT_FALSE(track.RemoveItemById(999999));
  EXPECT_TRUE(track.RemoveItemById(itemA->id));
  EXPECT_TRUE(track.FindItemById(itemA->id) == nullptr);
  EXPECT_EQ(track.items.size(), static_cast<size_t>(1));
}

// PlaylistArrangement tests

TEST_CASE(PlaylistArrangement_EmptyAndMissingOps) {
  PlaylistArrangement arrangement;
  EXPECT_EQ(arrangement.GetTrackCount(), 0);
  EXPECT_NEAR(arrangement.GetMaxEndBeat(), 0.0, 0.0001);
  EXPECT_FALSE(arrangement.RemoveTrackById(123456));

  std::vector<ScheduledNoteEvent> events;
  arrangement.CollectNoteEvents(events);
  EXPECT_EQ(events.size(), static_cast<size_t>(0));
}

TEST_CASE(PlaylistArrangement_AddItemTrackIndexAndFind) {
  PlaylistArrangement arrangement;
  PlaylistTrack* trackA = arrangement.AddTrack("A");
  PlaylistTrack* trackB = arrangement.AddTrack("B");
  EXPECT_TRUE(trackA != nullptr);
  EXPECT_TRUE(trackB != nullptr);

  const int trackBId = trackB != nullptr ? trackB->id : -1;
  auto pattern = std::make_shared<Pattern>();
  auto item = std::make_shared<PatternPlaylistItem>(pattern, 3.0, 2.0, 99);

  EXPECT_TRUE(arrangement.AddItemToTrack(trackBId, item));
  EXPECT_TRUE(arrangement.FindItemById(item->id) == item);
  EXPECT_EQ(item->trackIndex, 1);
  EXPECT_FALSE(arrangement.AddItemToTrack(-1, item));
}

TEST_CASE(PlaylistArrangement_RemoveTrackReindexesRemainingItems) {
  PlaylistArrangement arrangement;
  PlaylistTrack* trackA = arrangement.AddTrack("A");
  const int trackAId = trackA != nullptr ? trackA->id : -1;
  PlaylistTrack* trackB = arrangement.AddTrack("B");
  const int trackBId = trackB != nullptr ? trackB->id : -1;
  EXPECT_TRUE(trackA != nullptr);
  EXPECT_TRUE(trackB != nullptr);

  auto pattern = std::make_shared<Pattern>();
  auto itemA = std::make_shared<PatternPlaylistItem>(pattern, 1.0, 2.0, 0);
  auto itemB = std::make_shared<PatternPlaylistItem>(pattern, 5.0, 2.0, 0);

  EXPECT_TRUE(arrangement.AddItemToTrack(trackAId, itemA));
  EXPECT_TRUE(arrangement.AddItemToTrack(trackBId, itemB));
  EXPECT_EQ(itemA->trackIndex, 0);
  EXPECT_EQ(itemB->trackIndex, 1);

  EXPECT_TRUE(arrangement.RemoveTrackById(trackAId));
  EXPECT_TRUE(arrangement.FindTrackById(trackAId) == nullptr);
  EXPECT_TRUE(arrangement.FindTrackById(trackBId) != nullptr);
  EXPECT_EQ(itemB->trackIndex, 0);
}

TEST_CASE(PlaylistArrangement_CollectNoteEvents_SortsAndSkipsMutedTracks) {
  PlaylistArrangement arrangement;

  PlaylistTrack* trackA = arrangement.AddTrack("A");
  const int trackAId = trackA != nullptr ? trackA->id : -1;
  PlaylistTrack* trackB = arrangement.AddTrack("B");
  const int trackBId = trackB != nullptr ? trackB->id : -1;

  EXPECT_TRUE(trackA != nullptr);
  EXPECT_TRUE(trackB != nullptr);

  auto patternA = std::make_shared<Pattern>();
  patternA->AddNote(62, 0, 0.0, 1.0, 1.0f);
  auto patternB = std::make_shared<Pattern>();
  patternB->AddNote(64, 0, 0.0, 1.0, 1.0f);

  auto itemLate = std::make_shared<PatternPlaylistItem>(patternA, 4.0, 1.0, 0);
  auto itemEarly = std::make_shared<PatternPlaylistItem>(patternB, 2.0, 1.0, 0);

  EXPECT_TRUE(arrangement.AddItemToTrack(trackAId, itemLate));
  EXPECT_TRUE(arrangement.AddItemToTrack(trackBId, itemEarly));

  PlaylistTrack* mutedTrack = arrangement.FindTrackById(trackBId);
  EXPECT_TRUE(mutedTrack != nullptr);
  if (mutedTrack != nullptr) {
    mutedTrack->muted = true;
  }

  std::vector<ScheduledNoteEvent> events;
  arrangement.CollectNoteEvents(events);

  EXPECT_EQ(events.size(), static_cast<size_t>(1));
  if (events.size() == 1) {
    EXPECT_NEAR(events[0].startBeat, 4.0, 0.0001);
    EXPECT_EQ(events[0].pitch, 62);
  }
}
