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

    // --- Main Board Window ---
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(30, 32, 36, 255)); // Lighter background
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    
    ImGui::Begin("Board", nullptr, 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoSavedSettings | 
        ImGuiWindowFlags_NoBringToFrontOnFocus
    );

    // --- Top Navigation Bar ---
    float headerHeight = 40.0f * dpiScale;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 100));
    ImGui::BeginChild("TopNavBar", ImVec2(0, headerHeight), false, ImGuiWindowFlags_NoScrollbar);

    FontManager::Push(FontFamily::Bold, FontSize::Regular);
    float centerY = (headerHeight - ImGui::GetTextLineHeight()) * 0.5f;
    ImGui::SetCursorPos(ImVec2(15.0f * dpiScale, centerY));
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.9f), ICON_FA_CHECK " Stride");

    float logoWidth = ImGui::GetItemRectSize().x;
    float spacing = 5.0f * dpiScale;

    ImGui::SetCursorPos(ImVec2(15.0f * dpiScale + logoWidth + spacing, centerY));
    ImGui::Text("%s", Get().mTitle);
    FontManager::Pop();


    // Settings Button
    float buttonSize = 30 * dpiScale;
    float rightOffset = 15 * dpiScale;
    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - buttonSize - rightOffset , 5 * dpiScale));
    
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
    if (ImGui::Button(ICON_FA_ELLIPSIS, ImVec2(buttonSize, buttonSize)))
    {
        ImGui::OpenPopup("BoardSettingsPopup");
    }
    ImGui::PopStyleColor();

    if (ImGui::BeginPopup("BoardSettingsPopup"))
    {
        if (ImGui::MenuItem("Settings")) {}
        if (ImGui::MenuItem("About")) {}
        ImGui::EndPopup();
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();


    // --- Horizontal Scrolling Content ---
    // Adjust height for Top Bar (40) and Header (50)
    ImGui::SetCursorPosY(50 * dpiScale); 
    ImGui::BeginChild("BoardContent", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    float startX = 20.0f * dpiScale;
    float startY = 10.0f * dpiScale; // Reduced top padding inside content
    float cardListWidth = 280.0f * dpiScale;
    float spacingX = 12.0f * dpiScale; // Tighter spacing

    ImGui::SetCursorPos(ImVec2(startX, startY));

    float fullHeight = ImGui::GetContentRegionAvail().y;

    for(int i = 0; i < (int)mCardLists.size(); ++i)
    {
        if (i > 0)
        {
            ImGui::SameLine();
            ImGui::SetCursorPosX(startX + (i * (cardListWidth + spacingX)));
        }
        
        // Pass fixed width and full available height
        mCardLists[i].Render(i, ImVec2(cardListWidth, fullHeight - 20.0f * dpiScale)); // Slight bottom padding
    }

    // --- Add List UI ---
    ImGui::SameLine();
    ImGui::SetCursorPosX(startX + (mCardLists.size() * (cardListWidth + spacingX)));
    ImGui::SetCursorPosY(startY);

    if (!mIsAddingList)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 255, 255, 20));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 40));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 255, 255, 60));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f * dpiScale);
        
        if (ImGui::Button(ICON_FA_PLUS "  Add another list", ImVec2(cardListWidth, 35 * dpiScale)))
        {
            mIsAddingList = true;
            memset(mNewListTitleBuffer, 0, sizeof(mNewListTitleBuffer));
        }
        
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(16, 18, 4, 255));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 2.0f * dpiScale);
        ImGui::BeginChild("AddListPanel", ImVec2(cardListWidth, 80 * dpiScale), true, ImGuiWindowFlags_None);
        
        ImGui::PushItemWidth(-1);
        if (ImGui::IsWindowAppearing()) ImGui::SetKeyboardFocusHere();
        bool enterPressed = ImGui::InputTextWithHint("##NewListTitle", "Enter list name...", mNewListTitleBuffer, sizeof(mNewListTitleBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::PopItemWidth();

        ImGui::Spacing();

        // Add List Button
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(59, 130, 246, 255)); // Blue
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(37, 99, 235, 255));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f * dpiScale);
        if (ImGui::Button("Add list") || enterPressed)
        {
            if (strlen(mNewListTitleBuffer) > 0)
            {
                mCardLists.emplace_back(mNewListTitleBuffer, std::vector<Card>{});
                mIsAddingList = false;
            }
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);

        ImGui::SameLine();

        // Cancel Button
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 20));
        if (ImGui::Button(ICON_FA_XMARK) || ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            mIsAddingList = false;
        }
        ImGui::PopStyleColor(2);

        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    ImGui::EndChild();

    FontManager::Push(FontFamily::Regular, FontSize::Regular);
    // Note: DragDropManager calls were previously outside. 
    // We should ensure they are called. 
    // Since we are inside a child window now, we might need to be careful about coordinates if DragDropManager uses absolute.
    // However, DragDropManager usually handles global state.
    
    DragDropManager::DrawTooltipOfDraggedItem();
    DragDropManager::UpdateDropZone();

    DragOperation& aDragOperation = DragDropManager::GetDragOperation();
    if(aDragOperation.IsPending())
    {
        DragDropManager::PerformDropOperation();
    }
    FontManager::Pop();

    ImGui::End();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor();
}
