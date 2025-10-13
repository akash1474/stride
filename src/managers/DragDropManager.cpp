#include "pch.h"
#include "managers/DragDropManager.h"
#include <string.h>
#include <vector>
#include "FontAwesome6.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "BadgeColors.h"
#include "managers/FontManager.h"

BadgeColors::BadgeStyle GetBadgeStyleForText(const std::string& text, bool darkMode = true)
{
    static const std::vector<std::string> color_names
        = { "red", "orange", "amber",  "yellow", "lime",   "green",   "emerald", "teal", "cyan",
            "sky", "blue",   "indigo", "violet", "purple", "fuchsia", "pink",    "rose", "zinc" };

    unsigned long long hash = std::hash<std::string>{}(text);
    const std::string& selected_color_name = color_names[hash % color_names.size()];
    return BadgeColors::Get(selected_color_name, darkMode);
}

void CardComponent(
    const char* unique_id,
    const char* title,
    const std::vector<std::string>& badges,
    bool isDragging = false
)
{
    const float dpiScale = FontManager::GetDpiScale();
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if(window->SkipItems)
        return;

    const ImGuiStyle& style = ImGui::GetStyle();

    const ImGuiID id = window->GetID(unique_id);
    const ImVec2 pos = window->DC.CursorPos;

    const float padding = 12.0f * dpiScale;
    const float badge_padding = 5.0f * dpiScale;
    const float badge_height = 20.0f * dpiScale;
    const float badge_spacing = 5.0f * dpiScale;
    const float card_width = 256.0f * dpiScale;

    ImVec2 title_size = ImGui::CalcTextSize(title, nullptr, true, card_width - (padding * 2.0f));
    float title_height = title_size.y;
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

    bool is_held, is_hovered;
    ImGui::ButtonBehavior(bb, id, &is_hovered, &is_held);

    ImU32 border_color = IM_COL32(255, 255, 255, 0);
    if(is_hovered)
        border_color = IM_COL32(255, 255, 255, 255);

    window->DrawList->AddRectFilled(bb.Min, bb.Max, IM_COL32(36, 37, 40, 255), 5.0f); // Background
    window->DrawList->AddRect(bb.Min, bb.Max, border_color, 5.0f, 0, 2.0f);           // Border

    if(isDragging) // Just render the card not it's elements
        return;

    // --- 5. Content Layout & Rendering ---
    ImVec2 text_pos = ImVec2(bb.Min.x + padding, bb.Min.y + padding);

    // Title
    ImGui::RenderTextWrapped(text_pos, title, nullptr, card_width - (padding * 2.0f));
    text_pos.y += title_height;

    if(!badges.empty())
    {
        text_pos.y += padding * 0.5f;
    }
    ImVec2 badge_pos = text_pos;

    for(const std::string& badge : badges)
    {
        BadgeColors::BadgeStyle badgeColor = GetBadgeStyleForText(badge);
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
            badgeColor.bg,
            4.0f * dpiScale
        );

        ImVec2 text_center = ImVec2(
            badge_pos.x + badge_padding,
            badge_pos.y + (badge_height - badge_size.y) * 0.5f
        );

        ImGui::PushStyleColor(ImGuiCol_Text, badgeColor.text);
        ImGui::RenderText(text_center, badge.c_str());
        ImGui::PopStyleColor();

        badge_pos.x += badge_size.x + badge_spacing;
    }
}

void ListHeader(const char* title)
{
    const float dpiScale = FontManager::GetDpiScale();
    const float x_center = (ImGui::GetContentRegionAvail().x - 256.0f * dpiScale) * 0.5f;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x_center);
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if(window->SkipItems)
        return;

    const float height = 40.0f * dpiScale;
    const float width = 256.0f * dpiScale;
    const float padding_x = 10.0f * dpiScale;
    // --- Buttons on right ---
    float button_size = 30.0f * dpiScale;
    float spacing = 6.0f * dpiScale;

    const ImGuiStyle& style = ImGui::GetStyle();
    static char buffer[256];
    const std::string idStr = genUID();
    // GL_INFO("{}-{}",title,idStr);
    const ImGuiID id = window->GetID(idStr.c_str());
    static bool init = false;
    static bool isEditingTitle = false;
    if(!init)
    {
        strcpy_s(buffer, title);
        init = true;
    }


    ImVec2 size(width, height);

    ImRect bb(
        window->DC.CursorPos,
        ImVec2(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y + size.y)
    );
    ImGui::ItemSize(size);
    if(!ImGui::ItemAdd(bb, id))
        return;

    // --- Background ---
    ImU32 bg_col = ImGui::GetColorU32(ImGuiCol_WindowBg);
    window->DrawList->AddRectFilled(
        bb.Min,
        bb.Max,
        bg_col,
        10.0f * dpiScale,
        ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight
    );

    // --- Layout ---
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(191, 193, 196, 255));
    ImVec2 text_pos = { bb.Min.x + padding_x,
                        bb.Min.y
                            + (height - ImGui::GetTextLineHeight()
                               - (isEditingTitle ? (style.FramePadding.y * 2.0f) : 0.0f))
                                  * 0.5f };
    FontManager::Push(FontFamily::SemiBold, FontSize::Regular);
    if(isEditingTitle)
    {
        ImGui::SetCursorScreenPos(text_pos);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(255, 255, 255, 0));
        ImGui::SetNextItemWidth(width - padding_x - (button_size * 2.0f) - spacing - 5.0f);
        ImGui::InputText(
            (std::string("##titleEditor") + idStr).c_str(),
            buffer,
            IM_ARRAYSIZE(buffer)
        );
        ImGui::PopStyleColor();
        if(ImGui::IsItemFocused())
        {
            ImDrawList* fg = ImGui::GetForegroundDrawList();
            fg->AddRect(
                { ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y - 2.0f },
                { ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y + 2.0f },
                IM_COL32(255, 255, 255, 255),
                2.0f
            );
        }
    }
    else
    {
        ImGui::RenderText(text_pos, title, nullptr);
        if(ImGui::IsItemClicked())
            isEditingTitle = true;

        if(ImGui::IsItemHovered())
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    FontManager::Pop();



    ImVec2 btn2_pos = ImVec2(bb.Max.x - button_size, bb.Min.y + (height - button_size) * 0.5f);
    ImVec2 btn1_pos = ImVec2(btn2_pos.x - button_size - spacing, btn2_pos.y);

    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(16, 18, 4, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 50));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 255, 255, 100));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    ImGui::SetCursorScreenPos(btn1_pos);
    FontManager::Push(FontFamily::Regular, FontSize::Smaller);
    if(ImGui::Button(ICON_FA_DOWN_LEFT_AND_UP_RIGHT_TO_CENTER, ImVec2(button_size, button_size)))
    {
        isEditingTitle = false;
        // TODO: action for expand/contract
    }
    FontManager::Pop();
    if(ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    FontManager::Push(FontFamily::Regular, FontSize::Small);
    ImGui::SetCursorScreenPos(btn2_pos);
    if(ImGui::Button(ICON_FA_ELLIPSIS, ImVec2(button_size, button_size)))
    {
        // TODO: action for menu
    }
    FontManager::Pop();
    if(ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar();
}

bool RenderCardListFooter()
{
    const float dpiScale = FontManager::GetDpiScale();
    ImGui::BeginChild("##Footer", ImVec2(ImGui::GetContentRegionAvail().x, 60.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(16, 18, 4, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 50));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 255, 255, 100));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    const float width = 256.0f * dpiScale;
    float x_center = (ImGui::GetContentRegionAvail().x - width) * 0.5f;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x_center);
    ImGui::Button(ICON_FA_PLUS " Add Card", { width, 40.0f });
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();
    ImGui::EndChild();

    return false;
}

void DragDropManager::DrawTooltipOfDraggedItem()
{
    const ImGuiPayload* global_payload = ImGui::GetDragDropPayload();
    bool payload_active = (global_payload && global_payload->IsDataType("CARD_PAYLOAD"));

    if(payload_active && global_payload->IsDataType("CARD_PAYLOAD"))
    {
        const DragDropPayload* d = (const DragDropPayload*)global_payload->Data;
        std::vector<Card>* source_list
            = (d->source_list_id == 0) ? &Get().mWindowACards : &Get().mWindowBCards;
        if(source_list && !source_list->empty() && d->card_index < (int)source_list->size())
        {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.7f);
            ImGui::BeginTooltip();
            std::string preview_id = std::string("preview_tooltip");
            CardComponent(
                preview_id.c_str(),
                source_list->at(d->card_index).title.c_str(),
                source_list->at(d->card_index).badges
            );
            ImGui::EndTooltip();
            ImGui::PopStyleVar();
        }
    }
}

void DragDropManager::CardList(const char* title, int list_id, std::vector<Card>& cards)
{
    const float dpiScale = FontManager::GetDpiScale();
    std::vector<Dropzone>& aDropZones = Get().mDropZones;
    if(list_id == 0)
        aDropZones.clear();

    DragOperation& aDragOperation = Get().mDragOperation;

    // Window setup
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(16, 18, 4, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin(
        title,
        nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize
    );

    ListHeader(title);

    static bool isHeightGreater = false;
    static float lastHeight = 0.0f;

    const float yprev = ImGui::GetCursorPosY();
    const float height = std::min(lastHeight == 0.0f ? -1.0f : lastHeight, 700.0f);

    if(isHeightGreater)
    {
        ImGui::BeginChild(
            (std::string("CardContainer_") + title).c_str(),
            {0,0},
            ImGuiChildFlags_AutoResizeY
        );
    }

    const ImGuiPayload* global_payload = ImGui::GetDragDropPayload();
    bool payload_active = (global_payload && global_payload->IsDataType("CARD_PAYLOAD"));

    Dropzone* current_drop = Get().mCurrentDropZonePtr;

    for(size_t i = 0; i <= cards.size(); ++i)
    {
        bool isCurrentCardDragging = false;
        if(payload_active && global_payload->IsDataType("CARD_PAYLOAD"))
        {
            const DragDropPayload* d = (const DragDropPayload*)global_payload->Data;
            if(d->source_list_id == list_id && d->card_index == (int)i)
                isCurrentCardDragging = true;
        }
        if(isCurrentCardDragging)
            continue;

        // ---- DROPZONE between cards ----

        std::string dropzone_id
            = std::string("dropzone_") + std::to_string(list_id) + "_" + std::to_string(i);
        ImGui::InvisibleButton(dropzone_id.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 1.0f));
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

        // Highlight active dropzone
        if(current_drop && current_drop->list_id == list_id && current_drop->insert_index == (int)i)
        {
            // Render placeholder card (visual feedback)
            if(payload_active && global_payload->IsDataType("CARD_PAYLOAD"))
            {
                const DragDropPayload* d = (const DragDropPayload*)global_payload->Data;
                std::vector<Card>* source_list
                    = (d->source_list_id == 0) ? &Get().mWindowACards : &Get().mWindowBCards;
                if(d->card_index >= 0 && d->card_index < (int)source_list->size())
                {
                    const Card& moving_card = (*source_list)[d->card_index];
                    float x_center = (ImGui::GetContentRegionAvail().x - 256.0f * dpiScale) * 0.5f;
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x_center);
                    CardComponent(
                        "placeholder_card",
                        moving_card.title.c_str(),
                        moving_card.badges,
                        true // render as placeholder
                    );

                    // Reserve space for layout
                    std::string dropzone_id = std::string("dropzonex_") + std::to_string(list_id)
                                              + "_" + std::to_string(i);
                    ImGui::InvisibleButton(
                        dropzone_id.c_str(),
                        ImVec2(ImGui::GetContentRegionAvail().x, 1.0f)
                    );
                }
            }
        }

        // ---- CARD RENDER ----
        if(i < cards.size())
        {
            std::string card_id
                = std::string("card_") + std::to_string(list_id) + "_" + std::to_string(i);
            float x_center = (ImGui::GetContentRegionAvail().x - 256.0f * dpiScale) * 0.5f;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x_center);
            CardComponent(card_id.c_str(), cards[i].title.c_str(), cards[i].badges);

            if(ImGui::IsItemHovered())
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

            if(ImGui::BeginDragDropSource(
                   ImGuiDragDropFlags_SourceNoPreviewTooltip
                   | ImGuiDragDropFlags_AcceptNoPreviewTooltip
                   | ImGuiDragDropFlags_AcceptNoDrawDefaultRect
               ))
            {
                DragDropPayload d = { list_id, (int)i };
                ImGui::SetDragDropPayload("CARD_PAYLOAD", &d, sizeof(d));

                ImGui::EndDragDropSource();
            }
        }
    }

    if(isHeightGreater)
    {
        ImGui::EndChild();
    }
    lastHeight = ImGui::GetCursorPosY() - yprev;
    isHeightGreater = lastHeight > 700.0f;

    RenderCardListFooter();
    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);
}



void DragDropManager::RenderExperimentalLayout()
{
    const float dpiScale = FontManager::GetDpiScale();

    FontManager::Push(FontFamily::Medium, FontSize::Regular);

    ImGui::SetNextWindowPos({ 50, 70 }, ImGuiCond_Once);
    ImGui::SetNextWindowSize({ 280 * dpiScale, 800 }, ImGuiCond_Once);
    CardList("Super Long Title is wrapping", 0, Get().mWindowACards);
    ImGui::SetNextWindowSize({ 280 * dpiScale, 800 }, ImGuiCond_Once);
    ImGui::SetNextWindowPos({ 500, 70 }, ImGuiCond_Once);
    CardList("Doing", 1, Get().mWindowBCards);
    FontManager::Pop();

    DrawTooltipOfDraggedItem();


    Get().mCurrentDropZonePtr = GetCurrentDropzonePtr();


    DragOperation& aDragOperation = Get().mDragOperation;
    if(aDragOperation.IsPending())
    {
        PerformDropOperation();
    }
}

void DragDropManager::PerformDropOperation()
{
    DragOperation& aDragOperation = Get().mDragOperation;
    std::vector<Card>* source_list
        = (aDragOperation.source_list_id == 0) ? &Get().mWindowACards : &Get().mWindowBCards;
    std::vector<Card>* target_list
        = (aDragOperation.target_list_id == 0) ? &Get().mWindowACards : &Get().mWindowBCards;

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


Dropzone* DragDropManager::GetCurrentDropzonePtr()
{
    DragOperation& aDragOperation = Get().mDragOperation;
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