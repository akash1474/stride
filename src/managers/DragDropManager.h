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
        static void PerformListDropOperation(BoardData* board);
        static void DrawTooltipOfDraggedItem(const BoardData* board);
        static void UpdateDropZone();
        static void UpdateListDropZone();
        
        // List preview positioning
        static void UpdateListPreviewPosition(BoardData* board);
        static void ResetListPreviewPosition(BoardData* board);

        // State accessors
        static DragOperation& GetDragOperation() { return Get().mDragOperation; }
        static ListDragOperation& GetListDragOperation() { return Get().mListDragOperation; }
        static std::vector<Dropzone>& GetDropZones() { return Get().mDropZones; }
        static std::vector<ListDropzone>& GetListDropZones() { return Get().mListDropZones; }
        static Dropzone* GetCurrentDropZonePtr() { return Get().mCurrentDropZonePtr; }
        static ListDropzone* GetCurrentListDropZonePtr() { return Get().mCurrentListDropZonePtr; }

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
        ListDragOperation mListDragOperation;
        std::vector<Dropzone> mDropZones;
        std::vector<ListDropzone> mListDropZones;
        std::vector<ListBounds> mListBounds;
        Dropzone* mCurrentDropZonePtr = nullptr;
        ListDropzone* mCurrentListDropZonePtr = nullptr;
        
        // List preview tracking
        int mListPreviewOriginalIndex = -1;
        int mListPreviewCurrentIndex = -1;

        static Dropzone* FindCurrentDropzone();
        static ListDropzone* FindCurrentListDropzone();
    };
}
