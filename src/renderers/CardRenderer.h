#pragma once
#include "Card.h"
#include "imgui.h"

namespace Stride
{
    struct CardStyle
    {
        float width = 256.0f;
        float padding = 12.0f;
        float badgePadding = 5.0f;
        float badgeHeight = 20.0f;
        float badgeSpacing = 5.0f;
        float cornerRadius = 8.0f;

        ImU32 backgroundColor = IM_COL32(34, 39, 43, 255);
        ImU32 hoverBorderColor = IM_COL32(255, 255, 255, 255);
        ImU32 normalBorderColor = IM_COL32(255, 255, 255, 0);
    };

    class CardRenderer
    {
      public:
        static void SetStyle(const CardStyle& style);
        static const CardStyle& GetStyle();

        // Render a card - returns true if clicked
        static bool Render(const Card& card, const char* uniqueId, bool isDragging = false);

        // Calculate height without rendering
        static float CalculateHeight(const Card& card);

      private:
        static CardStyle sStyle;

        static float CalculateBadgeRowsHeight(
            const Card& card,
            float maxWidth,
            float badgePadding,
            float badgeHeight,
            float badgeSpacing
        );
    };
}
