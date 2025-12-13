#include "pch.h"
#include "BadgeColors.h"
#include "utilities/ColorPalette.h"
#include <unordered_map>

namespace BadgeColors
{
    namespace
    {
        const std::unordered_map<std::string, BadgeStyle> kLightModeColors = {
            { "red",     { ColorPalette::Red::Shade100,     ColorPalette::Red::Shade800 } },
            { "orange",  { ColorPalette::Orange::Shade100,  ColorPalette::Orange::Shade800 } },
            { "amber",   { ColorPalette::Amber::Shade100,   ColorPalette::Amber::Shade800 } },
            { "yellow",  { ColorPalette::Yellow::Shade100,  ColorPalette::Yellow::Shade800 } },
            { "lime",    { ColorPalette::Lime::Shade100,    ColorPalette::Lime::Shade800 } },
            { "green",   { ColorPalette::Green::Shade100,   ColorPalette::Green::Shade700 } },
            { "emerald", { ColorPalette::Emerald::Shade100, ColorPalette::Emerald::Shade700 } },
            { "teal",    { ColorPalette::Teal::Shade100,    ColorPalette::Teal::Shade700 } },
            { "cyan",    { ColorPalette::Cyan::Shade100,    ColorPalette::Cyan::Shade700 } },
            { "sky",     { ColorPalette::Sky::Shade100,     ColorPalette::Sky::Shade700 } },
            { "blue",    { ColorPalette::Blue::Shade100,    ColorPalette::Blue::Shade800 } },
            { "indigo",  { ColorPalette::Indigo::Shade100,  ColorPalette::Indigo::Shade800 } },
            { "violet",  { ColorPalette::Violet::Shade100,  ColorPalette::Violet::Shade800 } },
            { "purple",  { ColorPalette::Purple::Shade100,  ColorPalette::Purple::Shade800 } },
            { "fuchsia", { ColorPalette::Fuchsia::Shade100, ColorPalette::Fuchsia::Shade800 } },
            { "pink",    { ColorPalette::Pink::Shade100,    ColorPalette::Pink::Shade800 } },
            { "rose",    { ColorPalette::Rose::Shade100,    ColorPalette::Rose::Shade800 } },
            { "zinc",    { ColorPalette::Zinc::Shade100,    ColorPalette::Zinc::Shade800 } }
        };

        const std::unordered_map<std::string, BadgeStyle> kDarkModeColors = {
            { "red",     { ColorPalette::Red::Shade950,     ColorPalette::Red::Shade300 } },
            { "orange",  { ColorPalette::Orange::Shade950,  ColorPalette::Orange::Shade300 } },
            { "amber",   { ColorPalette::Amber::Shade950,   ColorPalette::Amber::Shade300 } },
            { "yellow",  { ColorPalette::Yellow::Shade950,  ColorPalette::Yellow::Shade400 } },
            { "lime",    { ColorPalette::Lime::Shade950,    ColorPalette::Lime::Shade300 } },
            { "green",   { ColorPalette::Green::Shade950,   ColorPalette::Green::Shade300 } },
            { "emerald", { ColorPalette::Emerald::Shade950, ColorPalette::Emerald::Shade300 } },
            { "teal",    { ColorPalette::Teal::Shade950,    ColorPalette::Teal::Shade300 } },
            { "cyan",    { ColorPalette::Cyan::Shade950,    ColorPalette::Cyan::Shade200 } },
            { "sky",     { ColorPalette::Sky::Shade950,     ColorPalette::Sky::Shade300 } },
            { "blue",    { ColorPalette::Blue::Shade950,    ColorPalette::Blue::Shade300 } },
            { "indigo",  { ColorPalette::Indigo::Shade950,  ColorPalette::Indigo::Shade200 } },
            { "violet",  { ColorPalette::Violet::Shade950,  ColorPalette::Violet::Shade200 } },
            { "purple",  { ColorPalette::Purple::Shade950,  ColorPalette::Purple::Shade200 } },
            { "fuchsia", { ColorPalette::Fuchsia::Shade100, ColorPalette::Fuchsia::Shade950 } },
            { "pink",    { ColorPalette::Pink::Shade950,    ColorPalette::Pink::Shade200 } },
            { "rose",    { ColorPalette::Rose::Shade950,    ColorPalette::Rose::Shade200 } },
            { "zinc",    { ColorPalette::Zinc::Shade900,    ColorPalette::Zinc::Shade300 } }
        };

        const std::vector<std::string> kColorNames = {
            "red", "orange", "amber",  "yellow", "lime",   "green",   "emerald", "teal", "cyan",
            "sky", "blue",   "indigo", "violet", "purple", "fuchsia", "pink",    "rose", "zinc"
        };
    }

    BadgeStyle Get(const std::string& name, bool darkMode)
    {
        const auto& map = darkMode ? kDarkModeColors : kLightModeColors;
        auto it = map.find(name);
        if (it != map.end())
            return it->second;

        // Default neutral gray badge
        return darkMode
            ? BadgeStyle{ ColorPalette::Zinc::Shade800, ColorPalette::Zinc::Shade300 }
            : BadgeStyle{ ColorPalette::Zinc::Shade100, ColorPalette::Zinc::Shade800 };
    }

    BadgeStyle GetBadgeStyleForText(const std::string& text, bool darkMode)
    {
        unsigned long long hash = std::hash<std::string>{}(text);
        const std::string& colorName = kColorNames[hash % kColorNames.size()];
        return Get(colorName, darkMode);
    }

    std::string GetColorNameForText(const std::string& text)
    {
        unsigned long long hash = std::hash<std::string>{}(text);
        return kColorNames[hash % kColorNames.size()];
    }

    const std::vector<std::string>& GetAvailableColors()
    {
        return kColorNames;
    }
}
