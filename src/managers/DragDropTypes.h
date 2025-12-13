#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include <string>

namespace Stride
{
    struct DragDropPayload
    {
        char source_list_id[64];  // Fixed-size for ImGui payload compatibility
        int card_index;
        
        // Helper to set from std::string
        void SetSourceListId(const std::string& id)
        {
            size_t len = id.length();
            if (len >= sizeof(source_list_id))
                len = sizeof(source_list_id) - 1;
            memcpy(source_list_id, id.c_str(), len);
            source_list_id[len] = '\0';
        }
        
        // Helper to get as std::string
        std::string GetSourceListId() const
        {
            return std::string(source_list_id);
        }
    };

    struct DragOperation
    {
        std::string source_list_id = "";
        int source_index = -1;
        std::string target_list_id = "";
        int target_index = -1;
        bool IsPending() const { return !source_list_id.empty(); }
        void Reset()
        {
            source_list_id = "";
            source_index = -1;
            target_list_id = "";
            target_index = -1;
        }
    };

    struct Dropzone
    {
        ImRect rect;
        std::string list_id;
        int insert_index;
    };
}
