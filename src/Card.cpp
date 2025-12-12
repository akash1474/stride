#include "pch.h"
#include "Card.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "managers/FontManager.h"
#include "BadgeColors.h"
#include "external/FontAwesome6.h"
#include <vector>
#include <string>

void Card::Render(
    const char* unique_id,
    const char* title,
    const std::vector<std::string>& badges,
    bool hasDescription,
    int checklistCompleted,
    int checklistTotal,
    bool isDragging
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

    // Add checklist badge to height calculation
    bool hasChecklist = checklistTotal > 0;
    if(hasChecklist)
    {
        std::string check_str = std::string(ICON_FA_SQUARE_CHECK " ")
                                + std::to_string(checklistCompleted) + "/"
                                + std::to_string(checklistTotal);
        ImVec2 badge_size = ImGui::CalcTextSize(check_str.c_str());
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

    // Add description badge to height calculation
    if(hasDescription)
    {
        const char* desc_text = ICON_FA_ALIGN_LEFT " Description";
        ImVec2 badge_size = ImGui::CalcTextSize(desc_text);
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

    // Add badges if present or description/checklist exists
    if(!badges.empty() || hasDescription || hasChecklist)
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

    window->DrawList->AddRectFilled(
        bb.Min,
        bb.Max,
        IM_COL32(34, 39, 43, 255),
        8.0f
    ); // Lighter background, more rounding
    window->DrawList->AddRect(bb.Min, bb.Max, border_color, 5.0f, 0, 2.0f); // Border

    if(isDragging) // Just render the card not it's elements
        return;

    // --- 5. Content Layout & Rendering ---
    ImVec2 text_pos = ImVec2(bb.Min.x + padding, bb.Min.y + padding);

    // Title
    ImGui::RenderTextWrapped(text_pos, title, nullptr, card_width - (padding * 2.0f));
    text_pos.y += title_height;

    if(!badges.empty() || hasDescription || hasChecklist)
    {
        text_pos.y += padding * 0.5f;
    }
    ImVec2 badge_pos = text_pos;

    for(const std::string& badge : badges)
    {
        BadgeColors::BadgeStyle badgeColor = BadgeColors::GetBadgeStyleForText(badge);
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

    // --- Checklist Badge ---
    if(hasChecklist)
    {
        std::string check_str = std::string(ICON_FA_SQUARE_CHECK " ")
                                + std::to_string(checklistCompleted) + "/"
                                + std::to_string(checklistTotal);

        // Green style for checklist
        BadgeColors::BadgeStyle badgeColor;
        badgeColor.bg = IM_COL32(34, 197, 94, 255);     // Green-500
        badgeColor.text = IM_COL32(255, 255, 255, 255); // White text

        ImVec2 badge_size = ImGui::CalcTextSize(check_str.c_str());
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
        ImGui::RenderText(text_center, check_str.c_str());
        ImGui::PopStyleColor();

        badge_pos.x += badge_size.x + badge_spacing;
    }

    // --- Description Badge ---
    if(hasDescription)
    {
        const char* desc_text = ICON_FA_ALIGN_LEFT " Description";
        // Use a neutral/gray style for description badge
        BadgeColors::BadgeStyle badgeColor;
        badgeColor.bg = IM_COL32(45, 45, 50, 255);      // Dark gray bg
        badgeColor.text = IM_COL32(180, 180, 180, 255); // Light gray text

        ImVec2 badge_size = ImGui::CalcTextSize(desc_text);
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
        ImGui::RenderText(text_center, desc_text);
        ImGui::PopStyleColor();
    }
}
