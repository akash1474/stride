#include "pch.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "managers/DragDropManager.h"
#include "renderers/CardRenderer.h"

namespace Stride
{
    void DragDropManager::DrawTooltipOfDraggedItem(const BoardData* board)
    {
        if (!board) return;

        const ImGuiPayload* global_payload = ImGui::GetDragDropPayload();
        bool payload_active = (global_payload && global_payload->IsDataType("CARD_PAYLOAD"));

        if (payload_active && global_payload->IsDataType("CARD_PAYLOAD"))
        {
            const DragDropPayload* d = (const DragDropPayload*)global_payload->Data;
            Card* card = GetCard(board, d->GetSourceListId(), d->card_index);

            if (card)
            {
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.7f);
                ImGui::BeginTooltip();
                std::string preview_id = std::string("preview_tooltip");
                CardRenderer::Render(
                    *card,
                    preview_id.c_str(),
                    false // isDragging false for tooltip to show content
                );
                ImGui::EndTooltip();
                ImGui::PopStyleVar();
            }
        }
    }

    void DragDropManager::PerformDropOperation(BoardData* board)
    {
        if (!board) return;

        DragOperation& dragOp = Get().mDragOperation;
        if (!dragOp.IsPending()) return;

        // Find source and target lists by ID
        CardList* source_list = board->FindList(dragOp.source_list_id);
        CardList* target_list = board->FindList(dragOp.target_list_id);

        // Validation: ensure lists exist
        if (!source_list || !target_list)
        {
            dragOp.Reset();
            return;
        }

        // Validation: ensure source card exists
        if (dragOp.source_index < 0 || dragOp.source_index >= (int)source_list->cards.size())
        {
            dragOp.Reset();
            return;
        }

        // Perform the move
        Card moved_card = std::move(source_list->cards[dragOp.source_index]);
        source_list->cards.erase(source_list->cards.begin() + dragOp.source_index);

        int insert_index = dragOp.target_index;
        if (insert_index < 0)
            insert_index = (int)target_list->cards.size();

        // Adjust index if moving within same list
        if (source_list == target_list && dragOp.source_index < insert_index)
        {
            insert_index--;
        }

        if (insert_index < 0)
            insert_index = 0;
        if (insert_index > (int)target_list->cards.size())
            insert_index = (int)target_list->cards.size();

        target_list->cards.insert(target_list->cards.begin() + insert_index, std::move(moved_card));

        dragOp.Reset();
    }

    void DragDropManager::UpdateDropZone()
    {
        Get().mCurrentDropZonePtr = FindCurrentDropzone();
    }

    void DragDropManager::RegisterListBounds(const std::string& list_id, ImRect bounds)
    {
        Get().mListBounds.push_back({ list_id, bounds });
    }

    void DragDropManager::ClearListBounds()
    {
        Get().mListBounds.clear();
    }

    Dropzone* DragDropManager::FindCurrentDropzone()
    {
        DragOperation& dragOp = Get().mDragOperation;

        if (const ImGuiPayload* payload = ImGui::GetDragDropPayload())
        {
            if (payload->IsDataType("CARD_PAYLOAD"))
            {
                ImVec2 mouse = ImGui::GetIO().MousePos;

                // 1. Find which list we are hovering
                std::string hovered_list_id;
                for (const auto& bounds : Get().mListBounds)
                {
                    if (bounds.rect.Contains(mouse))
                    {
                        hovered_list_id = bounds.list_id;
                        break;
                    }
                }

                float closest_dist = FLT_MAX;
                Dropzone* closest_zone = nullptr;

                // 2. Filter dropzones by hovered list
                for (auto& zone : Get().mDropZones)
                {
                    // Skip if hovering a list and this zone isn't in it
                    if (!hovered_list_id.empty() && zone.list_id != hovered_list_id)
                        continue;

                    ImVec2 center = (zone.rect.Min + zone.rect.Max) * 0.5f;
                    float dx = mouse.x - center.x;
                    float dy = mouse.y - center.y;
                    float dist = dx * dx + dy * dy;

                    if (dist + 10.0f < closest_dist)
                    {
                        closest_dist = dist;
                        closest_zone = &zone;
                    }
                }

                // 3. Set pending operation if mouse released
                if (closest_zone && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
                {
                    const DragDropPayload* d = (const DragDropPayload*)payload->Data;
                    dragOp.source_list_id = d->GetSourceListId();
                    dragOp.source_index = d->card_index;
                    dragOp.target_list_id = closest_zone->list_id;
                    dragOp.target_index = closest_zone->insert_index;
                    ImGui::ClearDragDrop();
                }

                return closest_zone;
            }
        }

        return nullptr;
    }

    Card* DragDropManager::GetCard(const BoardData* board, const std::string& list_id, int card_index)
    {
        if (!board) return nullptr;

        // Use BoardData's helper method
        const CardList* list = board->FindList(list_id);
        if (!list) return nullptr;

        if (card_index >= 0 && card_index < (int)list->cards.size())
        {
            return const_cast<Card*>(&list->cards[card_index]);
        }

        return nullptr;
    }
}
