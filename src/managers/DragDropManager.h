#pragma once

#include "imgui_internal.h"
#include "string"
#include "vector"

struct Card
{
    std::string title;
    std::string description;
    std::vector<std::string> badges;
};

struct DragDropPayload
{
    int source_list_id;
    int card_index;
};

struct DragOperation
{
    int source_list_id = -1;
    int source_index = -1;
    int target_list_id = -1;
    int target_index = -1;
    bool IsPending() const { return source_list_id != -1; }
    void Reset()
    {
        source_list_id = -1;
        source_index = -1;
        target_list_id = -1;
        target_index = -1;
    }
};

struct Dropzone
{
    ImRect rect;
    int list_id;
    int insert_index;
};


class DragDropManager
{
  public:
    static DragDropManager& Get()
    {
        static DragDropManager instance;
        return instance;
    }

    static void RenderExperimentalLayout();

  private:
    DragDropManager() = default;
    std::vector<Card> mWindowACards = {
        { "Card A1",
          "This is a longer card description meant to test text wrapping inside the card layout. "
          "It should correctly wrap and not overflow beyond the card boundary. Make sure to check "
          "alignment.",
          { "UI", "Feature", "High Priority", "In Progress" } },
        { "Card A2", "Short description for testing minimal content rendering.", { "Bug", "Low" } },
        { "Card A3",
          "This card has no badges and helps ensure layout spacing remains consistent.",
          {} },
        { "Card A4",
          "Card with many badges to test line breaks within the badge layout area.",
          { "Backend", "Refactor", "Urgent", "Core", "Stable", "Code Review", "V2.1" } },
        { "Card A5",
          "Multiline\nText\nCheck — this description explicitly uses line breaks to verify "
          "rendering behavior.",
          { "Testing", "Formatting" } }
    };

    std::vector<Card> mWindowBCards = {
        { "Card B1",
          "This card belongs to window B. It should be draggable to window A and maintain state.",
          { "UX", "Design" } },
        { "Card B2",
          "A long description to simulate overflow behavior. If dragging multiple times, ensure "
          "the visual "
          "feedback stays correct and data structures remain consistent.",
          { "Drag", "Drop", "Review", "Feature" } },
        { "Card B3",
          "Stress test with emoji 🚀🔥💡 and UTF-8 characters to ensure proper rendering and "
          "encoding handling.",
          { "Unicode", "Emoji", "Test" } },
        { "Card B4", "Card with minimal description.", { "Minor", "Note" } },
        { "Card B5",
          "Empty badges list to confirm rendering spacing below description still looks good.",
          {} },
        { "Card B5 No Description", "", {} },
        { "Card B5 No Description", "", { "Unicode", "Emoji", "Test" } }
    };

    DragOperation mDragOperation;
    DragDropPayload mPayload;
    std::vector<Dropzone> mDropZones;
    Dropzone* mCurrentDropZonePtr = nullptr;

    static void CardList(const char* title, int list_id, std::vector<Card>& cards);
    static void PerformDropOperation();
    static void DrawTooltipOfDraggedItem();
};