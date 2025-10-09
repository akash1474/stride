#pragma once

#include "imgui_internal.h"
#include "string"
#include "vector"

struct Card
{
    std::string title;
    std::string description;
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
    std::vector<Card> mWindowACards
        = { { "Card A1", "This is the first card in list A. It has some descriptive text." },
            { "Card A2", "This is the second card, demonstrating the layout." },
            { "Card A3", "The last card in the initial set for window A." } };
    std::vector<Card> mWindowBCards
        = { { "Card B1", "This card belongs to the second window, window B." },
            { "Card B2", "Drag and drop me to another list or reorder me!" } };

    DragOperation mDragOperation;
    DragDropPayload mPayload;
    std::vector<Dropzone> mDropZones;

    static void DrawCardList(const char* title, int list_id, std::vector<Card>& cards);
};