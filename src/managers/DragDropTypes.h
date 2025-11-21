#pragma once
#include "imgui.h"
#include "imgui_internal.h"

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
