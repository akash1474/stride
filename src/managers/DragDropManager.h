#pragma once

#include "imgui_internal.h"
#include "vector"
#include "Card.h"
#include "CardList.h"
#include "DragDropTypes.h"

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

  private:
    DragDropManager() = default;

    DragOperation mDragOperation;
    std::vector<Dropzone> mDropZones;
    Dropzone* mCurrentDropZonePtr = nullptr;

    static Dropzone* FindCurrentDropzone();
};
