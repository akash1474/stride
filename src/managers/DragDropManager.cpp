#include "pch.h"
#include "managers/DragDropManager.h"
#include "managers/Board.h"
#include "managers/FontManager.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <algorithm>

void DragDropManager::DrawTooltipOfDraggedItem()
{
    const ImGuiPayload* global_payload = ImGui::GetDragDropPayload();
    bool payload_active = (global_payload && global_payload->IsDataType("CARD_PAYLOAD"));

    if(payload_active && global_payload->IsDataType("CARD_PAYLOAD"))
    {
        const DragDropPayload* d = (const DragDropPayload*)global_payload->Data;
        Card* card = GetCard(d->source_list_id, d->card_index);
        
        if(card)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.7f);
            ImGui::BeginTooltip();
            std::string preview_id = std::string("preview_tooltip");
            Card::Render(
                preview_id.c_str(),
                card->mTitle.c_str(),
                card->mBadges,
                false // isDragging false for tooltip to show content
            );
            ImGui::EndTooltip();
            ImGui::PopStyleVar();
        }
    }
}

void DragDropManager::PerformDropOperation()
{
    DragOperation& aDragOperation = Get().mDragOperation;
    
    // Access lists via Board
    std::vector<CardList>& lists = Board::Get().mCardLists;
    
    if (aDragOperation.source_list_id < 0 || aDragOperation.source_list_id >= lists.size() ||
        aDragOperation.target_list_id < 0 || aDragOperation.target_list_id >= lists.size())
    {
        aDragOperation.Reset();
        return;
    }

    std::vector<Card>* source_list = &lists[aDragOperation.source_list_id].mCards;
    std::vector<Card>* target_list = &lists[aDragOperation.target_list_id].mCards;

    if(aDragOperation.source_index >= 0 && aDragOperation.source_index < (int)source_list->size())
    {
        Card moved_card = std::move((*source_list)[aDragOperation.source_index]);
        source_list->erase(source_list->begin() + aDragOperation.source_index);

        int insert_index = aDragOperation.target_index;
        if(insert_index < 0)
            insert_index = (int)target_list->size();

        if(source_list == target_list && aDragOperation.source_index < insert_index)
        {
            insert_index--;
        }

        if(insert_index < 0)
            insert_index = 0;
        if(insert_index > (int)target_list->size())
            insert_index = (int)target_list->size();

        target_list->insert(target_list->begin() + insert_index, std::move(moved_card));
    }
    aDragOperation.Reset();
}

void DragDropManager::UpdateDropZone()
{
    Get().mCurrentDropZonePtr = FindCurrentDropzone();
}

void DragDropManager::RegisterListBounds(int list_id, ImRect bounds)
{
    Get().mListBounds.push_back({ list_id, bounds });
}

void DragDropManager::ClearListBounds()
{
    Get().mListBounds.clear();
}

Dropzone* DragDropManager::FindCurrentDropzone()
{
    DragOperation& aDragOperation = Get().mDragOperation;
    if(const ImGuiPayload* payload = ImGui::GetDragDropPayload())
    {
        if(payload->IsDataType("CARD_PAYLOAD"))
        {
            ImVec2 mouse = ImGui::GetIO().MousePos;
            
            // 1. Find which list we are hovering
            int hovered_list_id = -1;
            for (const auto& bounds : Get().mListBounds)
            {
                if (bounds.rect.Contains(mouse))
                {
                    hovered_list_id = bounds.list_id;
                    break; // Found the list we are in
                }
            }

            float closest_dist = FLT_MAX;
            Dropzone* closest_zone = nullptr;

            // 2. Filter dropzones
            // If we are hovering a list, ONLY consider dropzones in that list.
            // If we are NOT hovering any list, consider ALL dropzones (fallback).
            
            for(auto& zone : Get().mDropZones)
            {
                if (hovered_list_id != -1 && zone.list_id != hovered_list_id)
                    continue;

                ImVec2 center = (zone.rect.Min + zone.rect.Max) * 0.5f;
                float dx = mouse.x - center.x;
                float dy = mouse.y - center.y;
                float dist = dx * dx + dy * dy;

                if(dist + 10.0f < closest_dist)
                {
                    closest_dist = dist;
                    closest_zone = &zone;
                }
            }

            // Highlight closest dropzone
            if(closest_zone)
            {
                // If mouse released -> perform drop
                if(!ImGui::IsMouseDown(ImGuiMouseButton_Left))
                {
                    const DragDropPayload* d = (const DragDropPayload*)payload->Data;
                    aDragOperation = { d->source_list_id,
                                       d->card_index,
                                       closest_zone->list_id,
                                       closest_zone->insert_index };
                    ImGui::ClearDragDrop(); // cancel imgui internal payload
                }
                return closest_zone;
            }
        }
    }

    return nullptr;
}

Card* DragDropManager::GetCard(int list_id, int card_index)
{
    std::vector<CardList>& lists = Board::Get().mCardLists;
    if (list_id >= 0 && list_id < lists.size())
    {
        std::vector<Card>& cards = lists[list_id].mCards;
        if (card_index >= 0 && card_index < cards.size())
        {
            return &cards[card_index];
        }
    }
    return nullptr;
}
