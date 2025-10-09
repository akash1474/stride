#include "pch.h"
#include "managers/DragDropManager.h"
#include <vector>
#include "imgui.h"
#include "imgui_internal.h"
#include "BadgeColors.h"

ImU32 GetBadgeColor(const std::string& text)
{
    unsigned hash = std::hash<std::string>{}(text);
    float hue = (hash % 256) / 256.0f;
    ImVec4 col = ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_Button));
    ImVec4 hsv;
    ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, hsv.x, hsv.y, hsv.z);
    hsv.x = hue;
    ImVec4 rgb(1.0, 1.0, 1.0, 1.0);
    ImGui::ColorConvertHSVtoRGB(hsv.x, hsv.y, hsv.z, rgb.x, rgb.y, rgb.z);
    return ImGui::GetColorU32(rgb);
}

void RenderCardManual(
    const char* unique_id,
    const char* title,
    const char* description,
    const std::vector<std::string>& badges,
    bool& out_is_hovered
)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if(window->SkipItems)
        return;

    const ImGuiStyle& style = ImGui::GetStyle();

    const ImGuiID id = window->GetID(unique_id);
    const ImVec2 pos = window->DC.CursorPos;

    const float padding = 15.0f;
    const float badge_padding = 5.0f;
    const float badge_height = 20.0f;
    const float badge_spacing = 5.0f;
    const float card_width = std::max(250.0f, ImGui::GetContentRegionAvail().x);

    ImVec2 title_size = ImGui::CalcTextSize(title, nullptr, true, card_width - (padding * 2.0f));
    float title_height = title_size.y;
    ImVec2 description_size
        = ImGui::CalcTextSize(description, nullptr, true, card_width - (padding * 2.0f));


    float separator_height = 1.0f + style.ItemSpacing.y * 2.0f;

    float badge_row_width = 0.0f;
    float badge_total_height = badge_height;

    for(const std::string& badge : badges)
    {
        ImVec2 badge_size = ImGui::CalcTextSize(badge.c_str());
        badge_size.x += badge_padding * 2;
        if(badge_row_width + badge_size.x > card_width - (padding * 2.0f))
        {
            badge_total_height += badge_height + badge_spacing;
            badge_row_width = badge_size.x;
        }
        else
        {
            badge_row_width += badge_size.x + badge_spacing;
        }
    }

    float card_height = padding + title_height;

    const bool hasDescription = description && strlen(description) > 0;
    if(hasDescription)
    {
        card_height += separator_height;
        card_height += description_size.y + padding * 0.5f;
    }

    // Add badges if present
    if(!badges.empty())
    {
        card_height += badge_total_height + padding * 0.5f;
    }

    // Add bottom padding
    card_height += padding;
    const ImVec2 card_size(card_width, card_height);
    const ImRect bb(pos, ImVec2(pos.x + card_size.x, pos.y + card_size.y));

    ImGui::ItemSize(bb, style.FramePadding.y);
    if(!ImGui::ItemAdd(bb, id))
        return;

    bool is_held;
    ImGui::ButtonBehavior(bb, id, &out_is_hovered, &is_held);

    ImU32 bg_color = out_is_hovered ? IM_COL32(50, 50, 55, 255) : IM_COL32(40, 40, 45, 255);
    ImU32 border_color = IM_COL32(80, 80, 80, 255);
    if(out_is_hovered)
        border_color = IM_COL32(120, 150, 255, 255);

    window->DrawList->AddRectFilled(bb.Min, bb.Max, bg_color, 5.0f); // Background
    window->DrawList->AddRect(bb.Min, bb.Max, border_color, 5.0f);   // Border

    // --- 5. Content Layout & Rendering ---
    ImVec2 text_pos = ImVec2(bb.Min.x + padding, bb.Min.y + padding);

    // Title
    ImGui::RenderTextWrapped(text_pos, title, nullptr, card_width - (padding * 2.0f));
    text_pos.y += title_height;

    // Separator
    window->DrawList->AddLine(
        ImVec2(text_pos.x, text_pos.y),
        ImVec2(bb.Max.x - padding, text_pos.y),
        border_color,
        1.0f
    );
    text_pos.y += separator_height;

    if(hasDescription){
        ImGui::RenderTextWrapped(text_pos, description, NULL, card_width - (padding * 2));
        text_pos.y += description_size.y;
        text_pos.y += padding * 0.5f;
    }

    ImVec2 badge_pos = text_pos;

    for(const std::string& badge : badges)
    {
        BadgeColors::BadgeStyle badgeColor = BadgeColors::Get("indigo");
        ImVec2 badge_size = ImGui::CalcTextSize(badge.c_str());
        badge_size.x += (badge_padding * 2);

        if(badge_pos.x + badge_size.x > bb.Max.x - padding)
        {
            badge_pos.x = bb.Min.x + padding;
            badge_pos.y += badge_height + badge_spacing;
        }

        window->DrawList->AddRectFilled(
            badge_pos,
            { badge_pos.x + badge_size.x, badge_pos.y + badge_height },
            badgeColor.text,
            2.0f
        );

        ImVec2 text_center = ImVec2(
            badge_pos.x + badge_padding,
            badge_pos.y + (badge_height - badge_size.y) * 0.5f
        );

        ImGui::PushStyleColor(ImGuiCol_Text, badgeColor.bg);
        ImGui::RenderText(text_center, badge.c_str());
        ImGui::PopStyleColor();

        badge_pos.x += badge_size.x + badge_spacing;
    }
}

void DragDropManager::DrawCardList(const char* title, int list_id, std::vector<Card>& cards)
{
    std::vector<Dropzone>& aDropZones = Get().mDropZones;
    if(list_id == 0)
    {
        aDropZones.clear();
    }
    DragOperation& aDragOperation = Get().mDragOperation;
    ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoMove);

    const ImGuiPayload* global_payload = ImGui::GetDragDropPayload();
    bool is_payload_active = (global_payload && global_payload->IsDataType("CARD_PAYLOAD"));

    for(size_t i = 0; i < cards.size(); ++i)
    {
        std::string dropzone_id
            = std::string("dropzone_") + std::to_string(list_id) + "_" + std::to_string(i);
        ImGui::InvisibleButton(dropzone_id.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 4));
        ImRect zone_rect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
        aDropZones.push_back({ zone_rect, list_id, (int)i });

        if(ImGui::BeginDragDropTarget())
        {
            if(const ImGuiPayload* p = ImGui::AcceptDragDropPayload("CARD_PAYLOAD"))
            {
                const DragDropPayload* d = (const DragDropPayload*)p->Data;
                aDragOperation = { d->source_list_id, d->card_index, list_id, (int)i };
            }
            ImGui::EndDragDropTarget();
        }

        std::string card_id
            = std::string("card_") + std::to_string(list_id) + "_" + std::to_string(i);
        bool isHovered = false;
        // float x_center = (ImGui::GetWindowWidth() - ImGui::GetContentRegionAvail().x) * 0.5f;
        // ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x_center);
        RenderCardManual(
            card_id.c_str(),
            cards[i].title.c_str(),
            cards[i].description.c_str(),
            cards[i].badges,
            isHovered
        );

        if(ImGui::BeginDragDropSource(
               ImGuiDragDropFlags_AcceptNoDrawDefaultRect
               | ImGuiDragDropFlags_AcceptNoPreviewTooltip
           ))
        {
            DragDropPayload d = { list_id, (int)i };
            ImGui::SetDragDropPayload("CARD_PAYLOAD", &d, sizeof(d));

            std::string preview_id
                = std::string("preview_") + std::to_string(list_id) + "_" + std::to_string(i);
            RenderCardManual(
                preview_id.c_str(),
                cards[i].title.c_str(),
                cards[i].description.c_str(),
                cards[i].badges,
                isHovered
            );

            ImGui::EndDragDropSource();
        }
    }

    std::string drop_end_id = std::string("dropzone_end_") + std::to_string(list_id);
    ImGui::InvisibleButton(drop_end_id.c_str(), { ImGui::GetContentRegionAvail().x, 4.0f });
    ImRect end_rect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    aDropZones.push_back({ end_rect, list_id, (int)cards.size() });

    if(ImGui::BeginDragDropTarget())
    {
        if(const ImGuiPayload* p = ImGui::AcceptDragDropPayload("CARD_PAYLOAD"))
        {
            const DragDropPayload* d = (const DragDropPayload*)p->Data;
            aDragOperation = { d->source_list_id, d->card_index, list_id, (int)cards.size() };
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::End();
}



void DragDropManager::RenderExperimentalLayout()
{
    DragOperation& aDragOperation = Get().mDragOperation;

    ImGui::SetNextWindowPos({ 50, 70 }, ImGuiCond_Once);
    ImGui::SetNextWindowSize({ 300, 600 }, ImGuiCond_Once);
    DrawCardList("Window A", 0, Get().mWindowACards);
    ImGui::SetNextWindowSize({ 300, 600 }, ImGuiCond_Once);
    ImGui::SetNextWindowPos({ 400, 70 }, ImGuiCond_Once);
    DrawCardList("Window B", 1, Get().mWindowBCards);

    if(const ImGuiPayload* payload = ImGui::GetDragDropPayload())
    {
        if(payload->IsDataType("CARD_PAYLOAD"))
        {
            ImVec2 mouse = ImGui::GetIO().MousePos;
            float closest_dist = FLT_MAX;
            Dropzone* closest_zone = nullptr;

            for(auto& zone : Get().mDropZones)
            {
                ImVec2 center = (zone.rect.Min + zone.rect.Max) * 0.5f;
                float dx = mouse.x - center.x;
                float dy = mouse.y - center.y;
                float dist = dx * dx + dy * dy;

                if(dist < closest_dist)
                {
                    closest_dist = dist;
                    closest_zone = &zone;
                }
            }

            // Highlight closest dropzone
            if(closest_zone)
            {
                ImDrawList* fg = ImGui::GetForegroundDrawList();
                fg->AddRectFilled(
                    closest_zone->rect.Min,
                    closest_zone->rect.Max,
                    IM_COL32(255, 255, 255, 40),
                    2.0f
                );

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
            }
        }
    }

    if(aDragOperation.IsPending())
    {
        std::vector<Card>* source_list
            = (aDragOperation.source_list_id == 0) ? &Get().mWindowACards : &Get().mWindowBCards;
        std::vector<Card>* target_list
            = (aDragOperation.target_list_id == 0) ? &Get().mWindowACards : &Get().mWindowBCards;

        if(aDragOperation.source_index >= 0
           && aDragOperation.source_index < (int)source_list->size())
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
}