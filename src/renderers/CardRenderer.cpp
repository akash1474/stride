#include "pch.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "BadgeColors.h"
#include "managers/FontManager.h"
#include "external/FontAwesome6.h"
#include "renderers/CardRenderer.h"

namespace Stride
{
    // Static member initialization
    CardStyle CardRenderer::sStyle;

    void CardRenderer::SetStyle(const CardStyle& style) { sStyle = style; }

    const CardStyle& CardRenderer::GetStyle() { return sStyle; }

    float CardRenderer::CalculateBadgeRowsHeight(
        const Card& card,
        float maxWidth,
        float badgePadding,
        float badgeHeight,
        float badgeSpacing
    )
    {
        float badge_row_width = 0.0f;
        float badge_total_height = badgeHeight;

        for(const std::string& badge : card.badges)
        {
            ImVec2 badge_size = ImGui::CalcTextSize(badge.c_str());
            badge_size.x += badgePadding * 2;
            if(badge_row_width + badge_size.x > maxWidth)
            {
                badge_total_height += badgeHeight + badgeSpacing;
                badge_row_width = badge_size.x;
            }
            else
            {
                badge_row_width += badge_size.x + badgeSpacing;
            }
        }

        // Add checklist badge
        if(card.HasChecklist())
        {
            std::string check_str = std::string(ICON_FA_SQUARE_CHECK " ")
                                    + std::to_string(card.GetChecklistCompleted()) + "/"
                                    + std::to_string(card.GetChecklistTotal());
            ImVec2 badge_size = ImGui::CalcTextSize(check_str.c_str());
            badge_size.x += badgePadding * 2;

            if(badge_row_width + badge_size.x > maxWidth)
            {
                badge_total_height += badgeHeight + badgeSpacing;
                badge_row_width = badge_size.x;
            }
            else
            {
                badge_row_width += badge_size.x + badgeSpacing;
            }
        }

        // Add description badge
        if(card.HasDescription())
        {
            const char* desc_text = ICON_FA_ALIGN_LEFT " Description";
            ImVec2 badge_size = ImGui::CalcTextSize(desc_text);
            badge_size.x += badgePadding * 2;

            if(badge_row_width + badge_size.x > maxWidth)
            {
                badge_total_height += badgeHeight + badgeSpacing;
            }
        }

        return badge_total_height;
    }

    float CardRenderer::CalculateHeight(const Card& card)
    {
        const float dpiScale = FontManager::GetDpiScale();
        const float padding = sStyle.padding * dpiScale;
        const float badge_padding = sStyle.badgePadding * dpiScale;
        const float badge_height = sStyle.badgeHeight * dpiScale;
        const float badge_spacing = sStyle.badgeSpacing * dpiScale;
        const float card_width = sStyle.width * dpiScale;

        ImVec2 title_size
            = ImGui::CalcTextSize(card.title.c_str(), nullptr, true, card_width - (padding * 2.0f));
        float card_height = padding + title_size.y;

        if(!card.badges.empty() || card.HasDescription() || card.HasChecklist())
        {
            float badge_total_height = CalculateBadgeRowsHeight(
                card,
                card_width - (padding * 2.0f),
                badge_padding,
                badge_height,
                badge_spacing
            );
            card_height += badge_total_height + padding * 0.5f;
        }

        card_height += padding;
        return card_height;
    }

    bool CardRenderer::Render(const Card& card, const char* uniqueId, bool isDragging)
    {
        const float dpiScale = FontManager::GetDpiScale();
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if(window->SkipItems)
            return false;

        const ImGuiStyle& style = ImGui::GetStyle();

        const ImGuiID id = window->GetID(uniqueId);
        const ImVec2 pos = window->DC.CursorPos;

        const float padding = sStyle.padding * dpiScale;
        const float badge_padding = sStyle.badgePadding * dpiScale;
        const float badge_height = sStyle.badgeHeight * dpiScale;
        const float badge_spacing = sStyle.badgeSpacing * dpiScale;
        const float card_width = sStyle.width * dpiScale;

        ImVec2 title_size
            = ImGui::CalcTextSize(card.title.c_str(), nullptr, true, card_width - (padding * 2.0f));
        float title_height = title_size.y;

        float badge_total_height = 0.0f;
        if(!card.badges.empty() || card.HasDescription() || card.HasChecklist())
        {
            badge_total_height = CalculateBadgeRowsHeight(
                card,
                card_width - (padding * 2.0f),
                badge_padding,
                badge_height,
                badge_spacing
            );
        }

        float card_height = padding + title_height;
        if(!card.badges.empty() || card.HasDescription() || card.HasChecklist())
        {
            card_height += badge_total_height + padding * 0.5f;
        }
        card_height += padding;

        const ImVec2 card_size(card_width, card_height);
        const ImRect bb(pos, ImVec2(pos.x + card_size.x, pos.y + card_size.y));

        ImGui::ItemSize(bb, style.FramePadding.y);
        if(!ImGui::ItemAdd(bb, id))
            return false;

        bool is_held, is_hovered;
        ImGui::ButtonBehavior(bb, id, &is_hovered, &is_held);

        ImU32 border_color = is_hovered ? sStyle.hoverBorderColor : sStyle.normalBorderColor;

        window->DrawList
            ->AddRectFilled(bb.Min, bb.Max, sStyle.backgroundColor, sStyle.cornerRadius);
        window->DrawList->AddRect(bb.Min, bb.Max, border_color, 5.0f, 0, 2.0f);

        if(isDragging)
            return false;

        // Content Layout & Rendering
        ImVec2 text_pos = ImVec2(bb.Min.x + padding, bb.Min.y + padding);

        // Title
        ImGui::RenderTextWrapped(
            text_pos,
            card.title.c_str(),
            nullptr,
            card_width - (padding * 2.0f)
        );
        text_pos.y += title_height;

        if(!card.badges.empty() || card.HasDescription() || card.HasChecklist())
        {
            text_pos.y += padding * 0.5f;
        }
        ImVec2 badge_pos = text_pos;

        // Render badges
        for(const std::string& badge : card.badges)
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

        // Checklist Badge
        if(card.HasChecklist())
        {
            int checklistCompleted = card.GetChecklistCompleted();
            int checklistTotal = card.GetChecklistTotal();
            std::string check_str = std::string(ICON_FA_SQUARE_CHECK " ")
                                    + std::to_string(checklistCompleted) + "/"
                                    + std::to_string(checklistTotal);

            BadgeColors::BadgeStyle badgeColor;
            badgeColor.bg = IM_COL32(34, 197, 94, 255);
            badgeColor.text = IM_COL32(255, 255, 255, 255);

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

        // Description Badge
        if(card.HasDescription())
        {
            const char* desc_text = ICON_FA_ALIGN_LEFT " Description";
            BadgeColors::BadgeStyle badgeColor;
            badgeColor.bg = IM_COL32(45, 45, 50, 255);
            badgeColor.text = IM_COL32(180, 180, 180, 255);

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

        return is_hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    }
}
