#pragma once

#include "imgui_internal.h"
#include "managers/DragDropTypes.h"
#include "BoardData.h"
#include <vector>
#include <string>

namespace Stride
{
    class DragDropManager
    {
    public:
        static DragDropManager& Get()
        {
            static DragDropManager instance;
            return instance;
        }

        // Main operations - now require board data to be passed in
        static void PerformDropOperation(BoardData* board);
        static void DrawTooltipOfDraggedItem(const BoardData* board);
        static void UpdateDropZone();

        // State accessors
        static DragOperation& GetDragOperation() { return Get().mDragOperation; }
        static std::vector<Dropzone>& GetDropZones() { return Get().mDropZones; }
        static Dropzone* GetCurrentDropZonePtr() { return Get().mCurrentDropZonePtr; }

        // Helper - now takes board data
        static Card* GetCard(const BoardData* board, const std::string& list_id, int card_index);

        // List bounds management
        static void RegisterListBounds(const std::string& list_id, ImRect bounds);
        static void ClearListBounds();

    private:
        DragDropManager() = default;

        struct ListBounds
        {
            std::string list_id;
            ImRect rect;
        };

        DragOperation mDragOperation;
        std::vector<Dropzone> mDropZones;
        std::vector<ListBounds> mListBounds;
        Dropzone* mCurrentDropZonePtr = nullptr;

        static Dropzone* FindCurrentDropzone();
    };
}
