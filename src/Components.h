#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include "BadgeColors.h"
#include "FontAwesome6.h"
#include "managers/FontManager.h"

namespace Components
{
    inline bool
    Badge(const char* text, const BadgeColors::BadgeStyle& style, bool removable = false)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if(window->SkipItems)
            return false;

        const float dpiScale = FontManager::GetDpiScale();
        const ImGuiStyle& imgui_style = ImGui::GetStyle();
        const ImGuiID id = window->GetID(text);

        const float padding_x = 8.0f * dpiScale;
        const float padding_y = 4.0f * dpiScale;
        const float icon_spacing = 6.0f * dpiScale;

        ImVec2 text_size = ImGui::CalcTextSize(text);
        ImVec2 icon_size = removable ? ImGui::CalcTextSize(ICON_FA_XMARK) : ImVec2(0, 0);

        float width = text_size.x + (padding_x * 2.0f);
        if(removable)
            width += icon_size.x + icon_spacing;

        float height = text_size.y + (padding_y * 2.0f);

        ImVec2 pos = window->DC.CursorPos;
        ImRect bb(pos, ImVec2(pos.x + width, pos.y + height));

        ImGui::ItemSize(bb, imgui_style.FramePadding.y);
        if(!ImGui::ItemAdd(bb, id))
            return false;

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

        // Render Background
        ImU32 bg_col = style.bg;
        if(removable && hovered)
        {
            // Slightly darken/lighten on hover if removable
            // Manual darkening: multiply alpha or rgb
            // Let's use the style.bg but maybe slightly modified?
            // Actually, let's just keep it simple or use the button behavior
        }

        window->DrawList->AddRectFilled(bb.Min, bb.Max, bg_col, height * 0.2f); // Pill shape

        // Render Text
        ImVec2 text_pos = ImVec2(bb.Min.x + padding_x, bb.Min.y + padding_y);
        ImGui::PushStyleColor(ImGuiCol_Text, style.text);
        ImGui::RenderText(text_pos, text);
        ImGui::PopStyleColor();

        // Render Icon
        if(removable)
        {
            ImVec2 icon_pos = ImVec2(text_pos.x + text_size.x + icon_spacing, text_pos.y);

            // Check if mouse is over the icon specifically? Or just the whole badge?
            // User said "removable", usually clicking the X removes it.
            // But clicking the whole badge is easier.
            // Let's make the whole badge clickable for removal for now, or maybe just the X?
            // "create a custom imgui component for it"
            // If I make the whole badge clickable, it acts like a button.

            ImU32 icon_col = style.text;
            if(hovered)
            {
                // Make icon slightly more opaque or different color?
                // For now, same color.
            }

            ImGui::PushStyleColor(ImGuiCol_Text, icon_col);
            ImGui::RenderText(icon_pos, ICON_FA_XMARK);
            ImGui::PopStyleColor();
        }

        return pressed;
    }
    inline void ProgressBar(float progress, float width, float height)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if(window->SkipItems)
            return;

        ImVec2 barPos = window->DC.CursorPos;

        // Background track
        window->DrawList->AddRectFilled(
            barPos,
            ImVec2(barPos.x + width, barPos.y + height),
            IM_COL32(45, 47, 50, 255),
            height * 0.5f
        );

        // Progress fill
        if(progress > 0.0f)
        {
            // Clamp progress
            float p = progress > 1.0f ? 1.0f : (progress < 0.0f ? 0.0f : progress);
            window->DrawList->AddRectFilled(
                barPos,
                ImVec2(barPos.x + (width * p), barPos.y + height),
                IM_COL32(34, 197, 94, 255),
                height * 0.5f
            );
        }

        ImGui::Dummy(ImVec2(width, height));
    }

    inline bool ChecklistItem(const char* label, bool* checked, bool* deleted, float width)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if(window->SkipItems)
            return false;

        const float dpiScale = FontManager::GetDpiScale();
        const float checkboxSize = 20.0f * dpiScale;
        const float deleteBtnSize = 20.0f * dpiScale;
        const float itemPadding = 8.0f * dpiScale;
        const float innerSpacing = 10.0f * dpiScale;

        // Calculate text width
        const float textWidth
            = width - checkboxSize - deleteBtnSize - (innerSpacing * 2.0f) - (itemPadding * 2.0f);

        // Calculate height based on text
        ImVec2 textSize = ImGui::CalcTextSize(label, nullptr, false, textWidth);
        float rowHeight
            = (textSize.y > checkboxSize ? textSize.y : checkboxSize) + (itemPadding * 2.0f);

        ImVec2 cursorPos = window->DC.CursorPos;
        ImRect rowRect(cursorPos, ImVec2(cursorPos.x + width, cursorPos.y + rowHeight));

        // Interaction for the whole row (hover effect)
        bool hovered = ImGui::IsMouseHoveringRect(rowRect.Min, rowRect.Max);

        // Draw Background
        if(hovered)
        {
            window->DrawList
                ->AddRectFilled(rowRect.Min, rowRect.Max, IM_COL32(255, 255, 255, 10), 4.0f);
        }

        // --- Checkbox ---
        ImVec2 checkPos
            = ImVec2(cursorPos.x + itemPadding, cursorPos.y + (rowHeight - checkboxSize) * 0.5f);
        ImRect checkRect(checkPos, ImVec2(checkPos.x + checkboxSize, checkPos.y + checkboxSize));

        // Handle Checkbox Click
        // We use the label as the ID for the checkbox so tests can find it.
        ImGuiID checkID = window->GetID(label);
        bool checkHovered, checkHeld;
        bool checkPressed = ImGui::ButtonBehavior(checkRect, checkID, &checkHovered, &checkHeld);

        bool toggled = false;
        if(checkPressed)
        {
            *checked = !(*checked);
            toggled = true;
        }

        // Register item so test engine can find it
        // Note: We don't use ItemSize here because we handle layout manually, but ItemAdd is
        // needed.
        ImGui::ItemAdd(checkRect, checkID);

        // Draw Checkbox
        ImU32 checkBorderCol = *checked ? IM_COL32(34, 197, 94, 255) : IM_COL32(100, 100, 100, 255);
        ImU32 checkBgCol = *checked ? IM_COL32(34, 197, 94, 255) : IM_COL32(0, 0, 0, 0);

        window->DrawList->AddRectFilled(checkRect.Min, checkRect.Max, checkBgCol, 4.0f);
        window->DrawList->AddRect(checkRect.Min, checkRect.Max, checkBorderCol, 4.0f, 0, 1.5f);

        if(*checked)
        {
            ImGui::RenderCheckMark(
                window->DrawList,
                ImVec2(checkRect.Min.x + 3 * dpiScale, checkRect.Min.y + 3 * dpiScale),
                IM_COL32(255, 255, 255, 255),
                checkboxSize - 6.0f * dpiScale
            );
        }

        // --- Text ---
        ImVec2 textPos = ImVec2(checkRect.Max.x + innerSpacing, cursorPos.y + itemPadding);
        if(*checked)
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 255));

        ImGui::RenderTextWrapped(textPos, label, nullptr, textWidth);

        if(*checked)
            ImGui::PopStyleColor();

        // --- Delete Button ---
        if(hovered)
        {
            ImVec2 delPos = ImVec2(
                rowRect.Max.x - itemPadding - deleteBtnSize,
                cursorPos.y + (rowHeight - deleteBtnSize) * 0.5f
            );
            ImGui::SetCursorScreenPos(delPos);
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 50, 50, 50));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 50, 50, 100));
            if(ImGui::Button(ICON_FA_XMARK, ImVec2(deleteBtnSize, deleteBtnSize)))
            {
                *deleted = true;
            }
            ImGui::PopStyleColor(3);
        }

        // Advance cursor
        ImGui::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + rowHeight));

        return toggled;
    }

    inline bool InputWithIcon(
        const char* id,
        const char* icon,
        const char* hint,
        char* buffer,
        size_t buffer_size,
        float width
    )
    {
        const float dpiScale = FontManager::GetDpiScale();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10 * dpiScale, 8 * dpiScale));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(255, 255, 255, 10));

        float iconSize = 30.0f * dpiScale;
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));

        bool iconClicked = ImGui::Button(icon, ImVec2(iconSize, 35 * dpiScale));

        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::SetNextItemWidth(width - iconSize - ImGui::GetStyle().ItemSpacing.x);

        bool enterPressed = ImGui::InputTextWithHint(
            id,
            hint,
            buffer,
            buffer_size,
            ImGuiInputTextFlags_EnterReturnsTrue
        );

        ImGui::PopStyleColor(); // FrameBg
        ImGui::PopStyleVar(2);  // FramePadding, FrameRounding

        return iconClicked || enterPressed;
    }

    inline bool StyledButton(const char* label, const ImVec2& size, ImU32 color, ImU32 hoverColor)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

        bool pressed = ImGui::Button(label, size);

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);

        return pressed;
    }
}
