#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include "BadgeColors.h"
#include "FontAwesome6.h"
#include "managers/FontManager.h"

namespace Components
{
    inline bool Badge(const char* text, const BadgeColors::BadgeStyle& style, bool removable = false)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
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
        if (removable)
            width += icon_size.x + icon_spacing;
            
        float height = text_size.y + (padding_y * 2.0f);

        ImVec2 pos = window->DC.CursorPos;
        ImRect bb(pos, ImVec2(pos.x + width, pos.y + height));
        
        ImGui::ItemSize(bb, imgui_style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id))
            return false;

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

        // Render Background
        ImU32 bg_col = style.bg;
        if (removable && hovered)
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
        if (removable)
        {
            ImVec2 icon_pos = ImVec2(text_pos.x + text_size.x + icon_spacing, text_pos.y);
            
            // Check if mouse is over the icon specifically? Or just the whole badge?
            // User said "removable", usually clicking the X removes it.
            // But clicking the whole badge is easier.
            // Let's make the whole badge clickable for removal for now, or maybe just the X?
            // "create a custom imgui component for it"
            // If I make the whole badge clickable, it acts like a button.
            
            ImU32 icon_col = style.text;
            if (hovered)
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
}
