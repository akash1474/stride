#include "pch.h"
#include "BoardManager.h"
#include "renderers/CardListRenderer.h"
#include "imgui_internal.h"
#include "managers/DragDropManager.h"
#include "managers/FontManager.h"
#include "imgui.h"

using Stride::Card;
using Stride::CardListRenderer;

using Stride::Card;
using Stride::CardListRenderer;

CardListUIState& BoardManager::GetListUIState(const std::string& listId)
{
    return mListUIStates[listId];
}

CardEditorState& BoardManager::GetEditorState(const std::string& listId)
{
    return mEditorStates[listId];
}

void BoardManager::Setup()
{
    // Create Default Board
    BoardData& board = CreateBoard("TxEdit - A Minimal IDE");

    // Default Data (Migrated from Board.cpp)
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

    board.mCardLists.emplace_back("Super Long Title is wrapping", std::move(mWindowACards));
    board.mCardLists.emplace_back("Doing", std::move(mWindowBCards));
    board.mCardLists.emplace_back("Done", std::vector<Card>{});

    SetActiveBoard(board.mID);
}

BoardData& BoardManager::CreateBoard(const std::string& title)
{
    // Simple UID generation
    static int idCounter = 0;
    std::string id = "board_" + std::to_string(++idCounter);

    mBoards.emplace_back(id, title);
    return mBoards.back();
}

void BoardManager::SetActiveBoard(const std::string& id)
{
    mActiveBoardID = id;
    // Reset UI state when switching
    mIsAddingList = false;
    memset(mNewListTitleBuffer, 0, sizeof(mNewListTitleBuffer));
}

BoardData* BoardManager::GetActiveBoard() { return GetBoard(mActiveBoardID); }

BoardData* BoardManager::GetBoard(const std::string& id)
{
    for(auto& board : mBoards)
    {
        if(board.mID == id)
            return &board;
    }
    return nullptr;
}

void BoardManager::AddList(const std::string& title)
{
    BoardData* board = GetActiveBoard();
    if(board)
    {
        board->mCardLists.emplace_back(title, std::vector<Card>{});
    }
}

void BoardManager::Render()
{
    BoardData* activeBoard = GetActiveBoard();
    if(!activeBoard)
        return;

    // --- Render Logic from Board.cpp ---
    const float dpiScale = FontManager::GetDpiScale();

    // --- Main Board Window ---
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(30, 32, 36, 255)); // Lighter background
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin(
        "Board",
        nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus
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
    ImGui::Text("%s", activeBoard->mTitle.c_str());
    FontManager::Pop();


    // Settings Button / Board Switcher
    float buttonSize = 30 * dpiScale;
    float rightOffset = 15 * dpiScale;
    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - buttonSize - rightOffset, 5 * dpiScale));

    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
    if(ImGui::Button(ICON_FA_ELLIPSIS, ImVec2(buttonSize, buttonSize)))
    {
        ImGui::OpenPopup("BoardSettingsPopup");
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 8.0f * dpiScale);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 12.0f * dpiScale));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 8.0f * dpiScale));
    ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(28, 30, 36, 255));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(255, 255, 255, 20));
    ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(28, 30, 36, 255));

    if(ImGui::BeginPopup("BoardSettingsPopup"))
    {
        FontManager::Push(FontFamily::SemiBold, FontSize::Small);
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "SWITCH BOARD");
        FontManager::Pop();

        ImGui::Spacing();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 10.0f));
        for(const auto& b : mBoards)
        {
            bool isSelected = (b.mID == mActiveBoardID);
            std::string label = std::string(isSelected ? ICON_FA_CHECK "  " : "   ") + b.mTitle;

            if(ImGui::Selectable(label.c_str(), isSelected, ImGuiSelectableFlags_SpanAvailWidth))
            {
                SetActiveBoard(b.mID);
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if(ImGui::Selectable(
               ICON_FA_PLUS "  Create New Board",
               ImGuiSelectableFlags_SpanAvailWidth
           ))
        {
            mIsCreatingBoard = true;
            memset(mNewBoardTitleBuffer, 0, sizeof(mNewBoardTitleBuffer));
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if(ImGui::Selectable(ICON_FA_GEAR "  Settings", ImGuiSelectableFlags_SpanAvailWidth)) {}
        if(ImGui::Selectable(ICON_FA_CIRCLE_INFO "  About", ImGuiSelectableFlags_SpanAvailWidth)) {}

        ImGui::PopStyleVar();

        ImGui::EndPopup();
    }
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(3);

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

    std::vector<CardList>& cardLists = activeBoard->mCardLists;

    for(int i = 0; i < (int)cardLists.size(); ++i)
    {
        if(i > 0)
        {
            ImGui::SameLine();
            ImGui::SetCursorPosX(startX + (i * (cardListWidth + spacingX)));
        }

        // Get UI states for this list
        CardListUIState& uiState = GetListUIState(cardLists[i].id);
        CardEditorState& editorState = GetEditorState(cardLists[i].id);

        // Pass fixed width and full available height
        CardListRenderer::Render(
            cardLists[i],
            uiState,
            editorState,
            i,
            ImVec2(cardListWidth, fullHeight - 20.0f * dpiScale)
        );
    }

    // --- Add List UI ---
    ImGui::SameLine();
    ImGui::SetCursorPosX(startX + (cardLists.size() * (cardListWidth + spacingX)));
    ImGui::SetCursorPosY(startY);

    if(!mIsAddingList)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 255, 255, 20));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 40));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 255, 255, 60));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f * dpiScale);

        if(ImGui::Button(ICON_FA_PLUS "  Add another list", ImVec2(cardListWidth, 35 * dpiScale)))
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
        ImGui::BeginChild(
            "AddListPanel",
            ImVec2(cardListWidth, 80 * dpiScale),
            true,
            ImGuiWindowFlags_None
        );

        ImGui::PushItemWidth(-1);
        if(ImGui::IsWindowAppearing())
            ImGui::SetKeyboardFocusHere();
        bool enterPressed = ImGui::InputTextWithHint(
            "##NewListTitle",
            "Enter list name...",
            mNewListTitleBuffer,
            sizeof(mNewListTitleBuffer),
            ImGuiInputTextFlags_EnterReturnsTrue
        );
        ImGui::PopItemWidth();

        ImGui::Spacing();

        // Add List Button
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(59, 130, 246, 255)); // Blue
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(37, 99, 235, 255));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f * dpiScale);
        if(ImGui::Button("Add list") || enterPressed)
        {
            if(strlen(mNewListTitleBuffer) > 0)
            {
                AddList(mNewListTitleBuffer);
                mIsAddingList = false;
            }
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);

        ImGui::SameLine();

        // Cancel Button
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 20));
        if(ImGui::Button(ICON_FA_XMARK) || ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            mIsAddingList = false;
        }
        ImGui::PopStyleColor(2);

        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    ImGui::EndChild(); // End BoardContent

    FontManager::Push(FontFamily::Regular, FontSize::Regular);

    DragDropManager::DrawTooltipOfDraggedItem();
    DragDropManager::UpdateDropZone();

    DragOperation& aDragOperation = DragDropManager::GetDragOperation();
    if(aDragOperation.IsPending())
    {
        DragDropManager::PerformDropOperation();
    }
    FontManager::Pop();

    // --- Create Board Popup ---
    if(mIsCreatingBoard)
    {
        ImGui::OpenPopup("Create Board");
        mIsCreatingBoard = false;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20 * dpiScale, 20 * dpiScale));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
    ImGui::SetNextWindowSize(ImVec2(400 * dpiScale, 0));

    if(ImGui::BeginPopupModal(
           "Create Board",
           NULL,
           ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
       ))
    {
        FontManager::Push(FontFamily::Bold, FontSize::Large);
        ImGui::Text("Create New Board");
        FontManager::Pop();

        ImGui::Spacing();
        ImGui::Spacing();

        FontManager::Push(FontFamily::Regular, FontSize::Regular);
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Board Title");

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10 * dpiScale, 10 * dpiScale));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(20, 22, 26, 255));
        ImGui::SetNextItemWidth(-1);

        if(ImGui::IsWindowAppearing())
            ImGui::SetKeyboardFocusHere();
        bool enter = ImGui::InputText(
            "##BoardTitle",
            mNewBoardTitleBuffer,
            sizeof(mNewBoardTitleBuffer),
            ImGuiInputTextFlags_EnterReturnsTrue
        );

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        FontManager::Pop(); // Regular

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        // Buttons
        float width = ImGui::GetContentRegionAvail().x;
        float btnWidth = (width - 10.0f * dpiScale) * 0.5f;

        // Cancel
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(45, 45, 50, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(60, 60, 65, 255));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f * dpiScale);
        if(ImGui::Button("Cancel", ImVec2(btnWidth, 35 * dpiScale))
           || ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            ImGui::CloseCurrentPopup();
            mIsCreatingBoard = false;
        }
        ImGui::PopStyleColor(2);

        ImGui::SameLine();

        // Create
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(59, 130, 246, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(37, 99, 235, 255));
        if(ImGui::Button("Create", ImVec2(btnWidth, 35 * dpiScale)) || enter)
        {
            if(strlen(mNewBoardTitleBuffer) > 0)
            {
                BoardData& newBoard = CreateBoard(mNewBoardTitleBuffer);
                SetActiveBoard(newBoard.mID);
                ImGui::CloseCurrentPopup();
                mIsCreatingBoard = false;
            }
        }
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(); // FrameRounding

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar(2); // WindowPadding, WindowRounding

    ImGui::End();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor();
}
