#include "pch.h"
#include "Board.h"
#include "managers/DragDropManager.h"
#include "managers/FontManager.h"
#include "imgui.h"

void Board::Setup()
{
    std::vector<Card> mWindowACards = {
        { "Card A1 has a long title that needs to wrap properly",
          "This is a longer card description meant to test text wrapping inside the card layout. "
          "It should correctly wrap and not overflow beyond the card boundary. Make sure to check "
          "alignment.",
          { "UI", "Feature", "High Priority", "In Progress" } },
        { "Card A2", "Short description for testing minimal content rendering.", { "Bug", "Low" } },
        { "Card A3",
          "This card has no badges and helps ensure layout spacing remains consistent.",
          {} },
        { "Card A4",
          "Card with many badges to test line breaks within the badge layout area.",
          { "Backend", "Refactor", "Urgent", "Core", "Stable", "Code Review", "V2.1" } },
        { "Card A5",
          "Multiline\nText\nCheck — this description explicitly uses line breaks to verify "
          "rendering behavior.",
          { "Testing", "Formatting" } }
    };

    std::vector<Card> mWindowBCards = {
        { "Card B1",
          "This card belongs to window B. It should be draggable to window A and maintain state.",
          { "UX", "Design" } },
        { "Card B2",
          "A long description to simulate overflow behavior. If dragging multiple times, ensure "
          "the visual "
          "feedback stays correct and data structures remain consistent.",
          { "Drag", "Drop", "Review", "Feature" } },
        { u8"Card B3 ŝ 🔥",
          "Stress test with emoji 🚀🔥💡 and UTF-8 characters to ensure proper rendering and "
          "encoding handling.",
          { "Unicode", "Emoji", "Test" } },
        { "Card B4", "Card with minimal description.", { "Minor", "Note" } },
        { "Card B5",
          "Empty badges list to confirm rendering spacing below description still looks good.",
          {} },
        { "Card B5 No Description", "", {} },
        { "Card B5 No Description", "", { "Unicode", "Emoji", "Test" } }
    };

    mCardLists.emplace_back("Super Long Title is wrapping", std::move(mWindowACards));
    mCardLists.emplace_back("Doing", std::move(mWindowBCards));
    mCardLists.emplace_back("Done", std::vector<Card>{});
}

void Board::Render()
{
    const float dpiScale = FontManager::GetDpiScale();

    FontManager::Push(FontFamily::Regular, FontSize::Regular);

    float startX = 50.0f;
    float startY = 70.0f;
    float spacingX = 450.0f * dpiScale; // Increased spacing

    for(int i = 0; i < mCardLists.size(); ++i)
    {
        ImGui::SetNextWindowPos({ startX + (i * spacingX), startY }, ImGuiCond_Once);
        ImGui::SetNextWindowSize({ 280 * dpiScale, 800 }, ImGuiCond_Once);
        mCardLists[i].Render(i);
    }

    FontManager::Pop();

    DragDropManager::DrawTooltipOfDraggedItem();

    DragDropManager::UpdateDropZone();

    DragOperation& aDragOperation = DragDropManager::GetDragOperation();
    if(aDragOperation.IsPending())
    {
        DragDropManager::PerformDropOperation();
    }
}
