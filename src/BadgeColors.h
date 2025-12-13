#pragma once
#include <imgui.h>
#include <string>
#include <vector>

namespace BadgeColors
{
    struct BadgeStyle {
        ImU32 bg;      // background color
        ImU32 text;    // text color
    };

    // Get badge style by color name
    BadgeStyle Get(const std::string& name, bool darkMode = true);
    
    // Get badge style automatically based on text hash
    BadgeStyle GetBadgeStyleForText(const std::string& text, bool darkMode = true);
    
    // Get the color name that would be assigned to a given text (useful for persistence)
    std::string GetColorNameForText(const std::string& text);
    
    // Get list of all available color names
    const std::vector<std::string>& GetAvailableColors();
}
