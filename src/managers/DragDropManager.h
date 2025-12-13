#pragma once

#include "imgui_internal.h"
#include "vector"
#include "Card.h"
#include "CardList.h"
#include "DragDropTypes.h"

using Stride::Card;

class DragDropManager
{
  public:
    static DragDropManager& Get()
    {
        static DragDropManager instance;
        return instance;
    }

    static void PerformDropOperation();
    static void DrawTooltipOfDraggedItem();
    static void UpdateDropZone();

    static DragOperation& GetDragOperation() { return Get().mDragOperation; }
    static std::vector<Dropzone>& GetDropZones() { return Get().mDropZones; }
    static Dropzone* GetCurrentDropZonePtr() { return Get().mCurrentDropZonePtr; }
    static Card* GetCard(int list_id, int card_index);

    static void RegisterListBounds(int list_id, ImRect bounds);
    static void ClearListBounds();

  private:
    DragDropManager() = default;

    struct ListBounds
    {
        int list_id;
        ImRect rect;
    };

    DragOperation mDragOperation;
    std::vector<Dropzone> mDropZones;
    std::vector<ListBounds> mListBounds;
    Dropzone* mCurrentDropZonePtr = nullptr;

    static Dropzone* FindCurrentDropzone();
};
