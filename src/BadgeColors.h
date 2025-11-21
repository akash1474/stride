#pragma once
#include <imgui.h>
#include <string>
#include <unordered_map>

namespace BadgeColors
{
    struct BadgeStyle {
        ImU32 bg;    // background color
        ImU32 text;  // text color
    };

    inline BadgeStyle Get(const std::string& name, bool darkMode = true)
    {
        static const std::unordered_map<std::string, BadgeStyle> light = {
            { "red",     { IM_COL32(254, 226, 226, 255), IM_COL32(153, 27, 27, 255) } },
            { "orange",  { IM_COL32(255, 237, 213, 255), IM_COL32(154, 52, 18, 255) } },
            { "amber",   { IM_COL32(254, 243, 199, 255), IM_COL32(146, 64, 14, 255) } },
            { "yellow",  { IM_COL32(254, 249, 195, 255), IM_COL32(133, 77, 14, 255) } },
            { "lime",    { IM_COL32(236, 252, 203, 255), IM_COL32(63, 98, 18, 255) } },
            { "green",   { IM_COL32(220, 252, 231, 255), IM_COL32(21, 128, 61, 255) } },
            { "emerald", { IM_COL32(209, 250, 229, 255), IM_COL32(4, 120, 87, 255) } },
            { "teal",    { IM_COL32(204, 251, 241, 255), IM_COL32(15, 118, 110, 255) } },
            { "cyan",    { IM_COL32(207, 250, 254, 255), IM_COL32(14, 116, 144, 255) } },
            { "sky",     { IM_COL32(224, 242, 254, 255), IM_COL32(3, 105, 161, 255) } },
            { "blue",    { IM_COL32(219, 234, 254, 255), IM_COL32(30, 64, 175, 255) } },
            { "indigo",  { IM_COL32(224, 231, 255, 255), IM_COL32(55, 48, 163, 255) } },
            { "violet",  { IM_COL32(237, 233, 254, 255), IM_COL32(91, 33, 182, 255) } },
            { "purple",  { IM_COL32(243, 232, 255, 255), IM_COL32(107, 33, 168, 255) } },
            { "fuchsia", { IM_COL32(253, 244, 255, 255), IM_COL32(134, 25, 143, 255) } },
            { "pink",    { IM_COL32(252, 231, 243, 255), IM_COL32(157, 23, 77, 255) } },
            { "rose",    { IM_COL32(255, 228, 230, 255), IM_COL32(159, 18, 57, 255) } },
            { "zinc",    { IM_COL32(244, 244, 245, 255), IM_COL32(39, 39, 42, 255) } }
        };

        static const std::unordered_map<std::string, BadgeStyle> dark = {
            { "red",     { IM_COL32(69, 10, 10, 255), IM_COL32(252, 165, 165, 255) } },
            { "orange",  { IM_COL32(67, 20, 7, 255), IM_COL32(253, 186, 116, 255) } },
            { "amber",   { IM_COL32(69, 26, 3, 255), IM_COL32(252, 211, 77, 255) } },
            { "yellow",  { IM_COL32(66, 32, 6, 255), IM_COL32(250, 204, 21, 255) } },
            { "lime",    { IM_COL32(26, 46, 5, 255), IM_COL32(190, 242, 100, 255) } },
            { "green",   { IM_COL32(5, 46, 22, 255), IM_COL32(134, 239, 172, 255) } },
            { "emerald", { IM_COL32(2, 44, 34, 255), IM_COL32(110, 231, 183, 255) } },
            { "teal",    { IM_COL32(4, 47, 46, 255), IM_COL32(94, 234, 212, 255) } },
            { "cyan",    { IM_COL32(8, 51, 68, 255), IM_COL32(165, 243, 252, 255) } },
            { "sky",     { IM_COL32(8, 47, 73, 255), IM_COL32(125, 211, 252, 255) } },
            { "blue",    { IM_COL32(23, 37, 84, 255), IM_COL32(147, 197, 253, 255) } },
            { "indigo",  { IM_COL32(31, 41, 89, 255), IM_COL32(199, 210, 254, 255) } },
            { "violet",  { IM_COL32(46, 16, 101, 255), IM_COL32(221, 214, 254, 255) } },
            { "purple",  { IM_COL32(59, 7, 100, 255), IM_COL32(233, 213, 255, 255) } },
            { "fuchsia", { IM_COL32(74, 4, 78, 255), IM_COL32(250, 232, 255, 255) } },
            { "pink",    { IM_COL32(80, 7, 36, 255), IM_COL32(251, 207, 232, 255) } },
            { "rose",    { IM_COL32(76, 5, 25, 255), IM_COL32(254, 205, 211, 255) } },
            { "zinc",    { IM_COL32(24, 24, 27, 255), IM_COL32(212, 212, 216, 255) } }
        };

        const auto& map = darkMode ? dark : light;
        auto it = map.find(name);
        if (it != map.end())
            return it->second;

        // Default neutral gray badge
        return darkMode
            ? BadgeStyle{ IM_COL32(39, 39, 42, 255), IM_COL32(212, 212, 216, 255) }
            : BadgeStyle{ IM_COL32(244, 244, 245, 255), IM_COL32(39, 39, 42, 255) };
    }
    inline BadgeStyle GetBadgeStyleForText(const std::string& text, bool darkMode = true)
    {
        static const std::vector<std::string> color_names
            = { "red", "orange", "amber",  "yellow", "lime",   "green",   "emerald", "teal", "cyan",
                "sky", "blue",   "indigo", "violet", "purple", "fuchsia", "pink",    "rose", "zinc" };

        unsigned long long hash = std::hash<std::string>{}(text);
        const std::string& selected_color_name = color_names[hash % color_names.size()];
        return Get(selected_color_name, darkMode);
    }
}
