#include "pch.h"
#include "BoardViewController.h"
#include "renderers/CardListRenderer.h"
#include "managers/DragDropManager.h"
#include "managers/FontManager.h"
#include "utilities/ColorPalette.h"
#include "storage/BoardStorageAdapter.h"
#include "FontAwesome6.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace Stride
{
    BoardViewController::BoardViewController(BoardRepository& repository) : mRepository(repository)
    {}

    void BoardViewController::SetActiveBoard(const std::string& id)
    {
        mActiveBoardId = id;
        mUIState.Reset();
        mCurrentViewMode = ViewMode::Board;
    }

    BoardData* BoardViewController::GetActiveBoard() { return mRepository.GetById(mActiveBoardId); }

    const BoardData* BoardViewController::GetActiveBoard() const
    {
        return mRepository.GetById(mActiveBoardId);
    }

    CardListUIState& BoardViewController::GetListUIState(const std::string& listId)
    {
        return mListUIStates[listId];
    }

    CardEditorState& BoardViewController::GetEditorState(const std::string& listId)
    {
        return mEditorStates[listId];
    }

    void BoardViewController::Render()
    {
        // Setup main window
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(30, 32, 36, 255));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        ImGui::Begin(
            "Board",
            nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
                | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus
        );

        // Check if repository is empty - show starter page
        if(mRepository.IsEmpty())
        {
            RenderStarterPage();
            RenderCreateBoardPopup();
            ImGui::End();
            ImGui::PopStyleVar(3);
            ImGui::PopStyleColor();
            return;
        }

        // Switch between Home and Board views
        if(mCurrentViewMode == ViewMode::Home)
        {
            RenderHomePage();
            RenderCreateBoardPopup();
            ImGui::End();
            ImGui::PopStyleVar(3);
            ImGui::PopStyleColor();
            return;
        }

        // Board view
        RenderBoardView();

        ImGui::End();
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor();
    }

    void BoardViewController::RenderBoardView()
    {

        BoardData* activeBoard = GetActiveBoard();
        if(!activeBoard)
        {
            return;
        }

        RenderNavBar();
        RenderBoardContent();
        RenderCreateBoardPopup();

        // Handle drag-drop - pass board data
        FontManager::Push(FontFamily::Regular, FontSize::Regular);
        DragDropManager::DrawTooltipOfDraggedCard(activeBoard);
        DragDropManager::UpdateDropZone();

        DragOperation& op = DragDropManager::GetDragOperation();
        if(op.IsPending())
        {
            DragDropManager::PerformDropOperation(activeBoard);
        }
        FontManager::Pop();
    }

    void BoardViewController::RenderNavBar()
    {
        BoardData* activeBoard = GetActiveBoard();
        if(!activeBoard)
            return;

        const float dpiScale = FontManager::GetDpiScale();
        float headerHeight = 40.0f * dpiScale;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 100));
        ImGui::BeginChild(
            "TopNavBar",
            ImVec2(0, headerHeight),
            false,
            ImGuiWindowFlags_NoScrollbar
        );

        FontManager::Push(FontFamily::Bold, FontSize::Regular);
        float centerY = (headerHeight - ImGui::GetTextLineHeight()) * 0.5f;
        ImGui::SetCursorPos(ImVec2(15.0f * dpiScale, centerY));
        
        // Home button
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 20));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 255, 255, 40));
        if(ImGui::Button(ICON_FA_CHECK " Stride"))
        {
            mCurrentViewMode = ViewMode::Home;
        }
        ImGui::PopStyleColor(3);

        float logoWidth = ImGui::GetItemRectSize().x;
        float spacing = 5.0f * dpiScale;

        ImGui::SetCursorPos(ImVec2(15.0f * dpiScale + logoWidth + spacing, centerY));
        ImGui::Text("%s", activeBoard->title.c_str());
        FontManager::Pop();

        RenderBoardSwitcher();

        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    void BoardViewController::RenderBoardSwitcher()
    {
        BoardData* activeBoard = GetActiveBoard();
        if(!activeBoard)
            return;

        const float dpiScale = FontManager::GetDpiScale();
        float buttonSize = 30 * dpiScale;
        float rightOffset = 15 * dpiScale;
        ImGui::SetCursorPos(
            ImVec2(ImGui::GetWindowWidth() - buttonSize - rightOffset, 5 * dpiScale)
        );

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
            for(const auto& b : mRepository.GetAll())
            {
                bool isSelected = (b.id == mActiveBoardId);
                std::string label = std::string(isSelected ? ICON_FA_CHECK "  " : "   ") + b.title;

                if(ImGui::Selectable(
                       label.c_str(),
                       isSelected,
                       ImGuiSelectableFlags_SpanAvailWidth
                   ))
                {
                    SetActiveBoard(b.id);
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
                mUIState.isCreatingBoard = true;
                memset(mUIState.newBoardTitleBuffer, 0, sizeof(mUIState.newBoardTitleBuffer));
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if(ImGui::Selectable(ICON_FA_GEAR "  Settings", ImGuiSelectableFlags_SpanAvailWidth)) {}
            if(ImGui::Selectable(
                   ICON_FA_CIRCLE_INFO "  About",
                   ImGuiSelectableFlags_SpanAvailWidth
               ))
            {}

            ImGui::PopStyleVar();
            ImGui::EndPopup();
        }
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(3);
    }

    void BoardViewController::RenderStarterPage()
    {
        const float dpiScale = FontManager::GetDpiScale();
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 windowSize = viewport->Size;
        
        // Subtle background glow (draw using foreground draw list to ensure visibility)
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 glowCenter = ImVec2(viewport->Pos.x + windowSize.x * 0.5f, viewport->Pos.y + windowSize.y * 0.5f);
        
        // Draw multiple circles with decreasing alpha for smooth gradient effect
        for(int i = 0; i <= 5; ++i)
        {
            float radius = (600.0f * dpiScale) * (i / 5.0f);
            ImU32 alpha = static_cast<ImU32>(10 * (i / 5.0f));
            drawList->AddCircleFilled(glowCenter, radius, IM_COL32(59, 130, 246, alpha), 64);
        }
        
        // Center content vertically and horizontally
        float contentWidth = 600.0f * dpiScale;
        float contentHeight = 200.0f * dpiScale;
        float startX = (windowSize.x - contentWidth) * 0.5f;
        float startY = (windowSize.y - contentHeight) * 0.5f;
        
        ImGui::SetCursorPos(ImVec2(startX, startY));
        ImGui::BeginChild("StarterContent", ImVec2(contentWidth, contentHeight), false, ImGuiWindowFlags_NoScrollbar);
        
        // App branding - Icon
        FontManager::Push(FontFamily::Bold, FontSize::Large);
        float iconSize = ImGui::CalcTextSize(ICON_FA_CHECK).x;
        float textWidth = ImGui::CalcTextSize("Stride").x;
        float totalWidth = iconSize + 10.0f * dpiScale + textWidth;
        float centerX = (contentWidth - totalWidth) * 0.5f;
        
        ImGui::SetCursorPosX(centerX);
        ImGui::PushStyleColor(ImGuiCol_Text, ColorPalette::Blue::Shade500);
        ImGui::Text(ICON_FA_CHECK);
        ImGui::PopStyleColor();
        
        ImGui::SameLine(0, 10.0f * dpiScale);
        ImGui::PushStyleColor(ImGuiCol_Text, ColorPalette::Slate::Shade50);
        ImGui::Text("Stride");
        ImGui::PopStyleColor();
        FontManager::Pop();
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        // Subtitle
        FontManager::Push(FontFamily::Regular, FontSize::Regular);
        const char* subtitle = "Organize your work visually";
        float subtitleWidth = ImGui::CalcTextSize(subtitle).x;
        ImGui::SetCursorPosX((contentWidth - subtitleWidth) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, ColorPalette::Slate::Shade400);
        ImGui::Text("%s", subtitle);
        ImGui::PopStyleColor();
        FontManager::Pop();
        
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        
        // Feature pills
        FontManager::Push(FontFamily::Regular, FontSize::Small);
        const char* features[] = {
            ICON_FA_TABLE_COLUMNS " Boards",
            ICON_FA_NOTE_STICKY " Cards",
            ICON_FA_HAND " Drag & Drop"
        };
        
        float totalPillsWidth = 0.0f;
        float pillSpacing = 8.0f * dpiScale;
        for(int i = 0; i < 3; ++i)
        {
            totalPillsWidth += ImGui::CalcTextSize(features[i]).x + 16.0f * dpiScale;
            if(i < 2) totalPillsWidth += pillSpacing;
        }
        
        ImGui::SetCursorPosX((contentWidth - totalPillsWidth) * 0.5f);
        
        for(int i = 0; i < 3; ++i)
        {
            if(i > 0) ImGui::SameLine(0, pillSpacing);
            
            ImGui::PushStyleColor(ImGuiCol_Button, ColorPalette::Slate::Shade800);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorPalette::Slate::Shade800);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorPalette::Slate::Shade800);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 14.0f * dpiScale);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f * dpiScale, 6.0f * dpiScale));
            
            ImGui::PushStyleColor(ImGuiCol_Text, ColorPalette::Slate::Shade400);
            ImGui::Button(features[i]);
            ImGui::PopStyleColor();
            
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);
        }
        FontManager::Pop();
        
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        
        // Primary action button
        float buttonWidth = 280.0f * dpiScale;
        float buttonHeight = 50.0f * dpiScale;
        ImGui::SetCursorPosX((contentWidth - buttonWidth) * 0.5f);
        
        ImGui::PushStyleColor(ImGuiCol_Button, ColorPalette::Blue::Shade500);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorPalette::Blue::Shade600);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorPalette::Blue::Shade700);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f * dpiScale);
        
        FontManager::Push(FontFamily::SemiBold, FontSize::Regular);
        bool buttonClicked = ImGui::Button(ICON_FA_PLUS "  Create Your First Board", ImVec2(buttonWidth, buttonHeight));
        FontManager::Pop();
        
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        // Keyboard shortcut hint
        FontManager::Push(FontFamily::Regular, FontSize::Small);
        const char* hintText = "or press ";
        const char* keyText = "Ctrl+N";
        const char* hintEnd = " to start";
        
        float hintWidth = ImGui::CalcTextSize(hintText).x;
        float keyWidth = ImGui::CalcTextSize(keyText).x + 16.0f * dpiScale;
        float endWidth = ImGui::CalcTextSize(hintEnd).x;
        float totalHintWidth = hintWidth + keyWidth + endWidth;
        
        ImGui::SetCursorPosX((contentWidth - totalHintWidth) * 0.5f);
        
        ImGui::PushStyleColor(ImGuiCol_Text, ColorPalette::Slate::Shade600);
        ImGui::Text("%s", hintText);
        ImGui::PopStyleColor();
        
        ImGui::SameLine(0, 0);
        
        // Styled keyboard badge
        ImGui::PushStyleColor(ImGuiCol_Button, ColorPalette::Slate::Shade800);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorPalette::Slate::Shade800);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorPalette::Slate::Shade800);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f * dpiScale);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f * dpiScale, 4.0f * dpiScale));
        
        ImGui::PushStyleColor(ImGuiCol_Text, ColorPalette::Slate::Shade300);
        ImGui::Button(keyText);
        ImGui::PopStyleColor();
        
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);
        
        ImGui::SameLine(0, 0);
        
        ImGui::PushStyleColor(ImGuiCol_Text, ColorPalette::Slate::Shade600);
        ImGui::Text("%s", hintEnd);
        ImGui::PopStyleColor();
        FontManager::Pop();
        
        ImGui::EndChild();
        
        // Handle keyboard shortcut
        if(ImGui::IsKeyPressed(ImGuiKey_N) && ImGui::GetIO().KeyCtrl)
        {
            buttonClicked = true;
        }
        
        // Open create board dialog
        if(buttonClicked)
        {
            mUIState.isCreatingBoard = true;
            memset(mUIState.newBoardTitleBuffer, 0, sizeof(mUIState.newBoardTitleBuffer));
        }
    }

    void BoardViewController::RenderHomePage()
    {
        const float dpiScale = FontManager::GetDpiScale();
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 windowSize = viewport->Size;
        
        // Header with app branding
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
        ImGui::BeginChild("HomeHeader", ImVec2(0, 80.0f * dpiScale), false, ImGuiWindowFlags_NoScrollbar);
        
        FontManager::Push(FontFamily::Bold, FontSize::Large);
        float centerY = (80.0f * dpiScale - ImGui::GetTextLineHeight()) * 0.5f;
        ImGui::SetCursorPos(ImVec2(40.0f * dpiScale, centerY));
        ImGui::PushStyleColor(ImGuiCol_Text, ColorPalette::Blue::Shade500);
        ImGui::Text(ICON_FA_CHECK);
        ImGui::PopStyleColor();
        
        ImGui::SameLine(0, 10.0f * dpiScale);
        ImGui::PushStyleColor(ImGuiCol_Text, ColorPalette::Slate::Shade50);
        ImGui::Text("Stride");
        ImGui::PopStyleColor();
        
        ImGui::SameLine(0, 30.0f * dpiScale);
        FontManager::Pop();
        FontManager::Push(FontFamily::Regular, FontSize::Regular);
        ImGui::PushStyleColor(ImGuiCol_Text, ColorPalette::Slate::Shade400);
        ImGui::SetCursorPosY(centerY + 5.0f * dpiScale);
        ImGui::Text("Your Boards");
        ImGui::PopStyleColor();
        FontManager::Pop();
        
        ImGui::EndChild();
        ImGui::PopStyleColor();
        
        // Board grid content
        ImGui::BeginChild("HomeBoardGrid", ImVec2(0, 0), false);
        
        const auto& boards = mRepository.GetAll();
        float cardWidth = 280.0f * dpiScale;
        float cardHeight = 120.0f * dpiScale;
        float spacing = 20.0f * dpiScale;
        float startX = 40.0f * dpiScale;
        float startY = 20.0f * dpiScale;
        
        int columns = std::max(1, (int)((windowSize.x - 80.0f * dpiScale) / (cardWidth + spacing)));
        
        ImGui::SetCursorPos(ImVec2(startX, startY));
        
        int col = 0;
        int row = 0;
        for(const auto& board : boards)
        {
            if(col > 0)
            {
                ImGui::SameLine(0, spacing);
            }
            
            ImGui::BeginGroup();
            
            // Board card
            ImGui::PushStyleColor(ImGuiCol_Button, ColorPalette::Slate::Shade800);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorPalette::Slate::Shade700);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorPalette::Slate::Shade600);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f * dpiScale);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            
            if(ImGui::Button(("##board_" + board.id).c_str(), ImVec2(cardWidth, cardHeight)))
            {
                SetActiveBoard(board.id);
            }
            
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);
            
            // Card content overlay
            ImVec2 cardMin = ImGui::GetItemRectMin();
            ImVec2 cardMax = ImGui::GetItemRectMax();
            
            ImGui::SetCursorScreenPos(ImVec2(cardMin.x + 16.0f * dpiScale, cardMin.y + 16.0f * dpiScale));
            ImGui::BeginGroup();
            
            // Board title
            FontManager::Push(FontFamily::SemiBold, FontSize::Regular);
            ImGui::PushStyleColor(ImGuiCol_Text, ColorPalette::Slate::Shade50);
            ImGui::Text("%s", board.title.c_str());
            ImGui::PopStyleColor();
            FontManager::Pop();
            
            ImGui::Spacing();
            
            // Board stats
            FontManager::Push(FontFamily::Regular, FontSize::Small);
            ImGui::PushStyleColor(ImGuiCol_Text, ColorPalette::Slate::Shade400);
            ImGui::Text("%zu lists, %zu cards", board.lists.size(), board.GetTotalCardCount());
            ImGui::PopStyleColor();
            FontManager::Pop();
            
            ImGui::EndGroup();
            ImGui::EndGroup();
            
            col++;
            if(col >= columns)
            {
                col = 0;
                row++;
                ImGui::SetCursorPos(ImVec2(startX, startY + (row * (cardHeight + spacing))));
            }
        }
        
        // Create new board card
        if(col > 0)
        {
            ImGui::SameLine(0, spacing);
        }
        else
        {
            // If starting a new row for the create button
            ImGui::SetCursorPos(ImVec2(startX, startY + (row * (cardHeight + spacing))));
        }
        
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 255, 255, 20));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 40));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 255, 255, 60));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f * dpiScale);
        
        FontManager::Push(FontFamily::Regular, FontSize::Regular);
        if(ImGui::Button(ICON_FA_PLUS "  Create New Board", ImVec2(cardWidth, cardHeight)))
        {
            mUIState.isCreatingBoard = true;
            memset(mUIState.newBoardTitleBuffer, 0, sizeof(mUIState.newBoardTitleBuffer));
        }
        FontManager::Pop();
        
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
        
        ImGui::EndChild();
    }

    void BoardViewController::RenderBoardContent()
    {
        BoardData* activeBoard = GetActiveBoard();
        if(!activeBoard)
            return;

        const float dpiScale = FontManager::GetDpiScale();

        // Adjust height for Top Bar
        ImGui::SetCursorPosY(50 * dpiScale);
        ImGui::BeginChild(
            "BoardContent",
            ImVec2(0, 0),
            false,
            ImGuiWindowFlags_HorizontalScrollbar
        );

        float startX = 20.0f * dpiScale;
        float startY = 10.0f * dpiScale;
        float cardListWidth = 280.0f * dpiScale;
        float spacingX = 12.0f * dpiScale;

        ImGui::SetCursorPos(ImVec2(startX, startY));
        float fullHeight = ImGui::GetContentRegionAvail().y;

        std::vector<CardList>& cardLists = activeBoard->lists;

        // Clear list dropzones at the start
        if(!cardLists.empty())
        {
            DragDropManager::GetListDropZones().clear();
        }

        // Check if we're dragging a list
        const ImGuiPayload* payload = ImGui::GetDragDropPayload();
        int draggingListIndex = -1;
        if(payload && payload->IsDataType("LIST_PAYLOAD"))
        {
            const ListDragDropPayload* dragPayload = (const ListDragDropPayload*)payload->Data;
            draggingListIndex = dragPayload->list_index;
        }

        // Render all lists (dragged one shows as ghost at current preview position)
        for(int i = 0; i < (int)cardLists.size(); ++i)
        {
            if(i > 0)
            {
                ImGui::SameLine();
            }
            ImGui::SetCursorPosX(startX + (i * (cardListWidth + spacingX)));

            // Register dropzone before this position
            float dropzoneX = startX + (i * (cardListWidth + spacingX)) - (spacingX / 2.0f);
            ImRect dropzoneRect(
                ImVec2(dropzoneX, startY),
                ImVec2(dropzoneX + spacingX, startY + fullHeight - 20.0f * dpiScale)
            );
            DragDropManager::GetListDropZones().push_back({ dropzoneRect, i });

            CardListUIState& uiState = GetListUIState(cardLists[i].id);
            CardEditorState& editorState = GetEditorState(cardLists[i].id);

            // If this is the list being dragged, render as semi-transparent ghost
            bool isBeingDragged
                = draggingListIndex != -1 && cardLists[i].id == cardLists[draggingListIndex].id;

            if(isBeingDragged)
            {
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.7f);
            }
            CardListRenderer::Render(
                cardLists[i],
                uiState,
                editorState,
                i,
                ImVec2(cardListWidth, fullHeight - 20.0f * dpiScale)
            );
            if(isBeingDragged)
            {
                ImGui::PopStyleVar();
            }
        }

        // Register dropzone after the last list
        if(!cardLists.empty())
        {
            float dropzoneX
                = startX + (cardLists.size() * (cardListWidth + spacingX)) - (spacingX / 2.0f);
            ImRect dropzoneRect(
                ImVec2(dropzoneX, startY),
                ImVec2(dropzoneX + spacingX, startY + fullHeight - 20.0f * dpiScale)
            );
            DragDropManager::GetListDropZones().push_back({ dropzoneRect, (int)cardLists.size() });
        }

        // Update list preview position in real-time and update dropzone
        DragDropManager::UpdateListDropZone();
        DragDropManager::UpdateListPreviewPosition(activeBoard);

        // Perform list drop operation first (before checking if drag ended)
        // This will finalize the position if drop was successful
        DragDropManager::PerformListDropOperation(activeBoard);

        // Check if drag ended without a drop (cancelled/escaped)
        // Only reset if no pending operation and no payload
        if(!ImGui::GetDragDropPayload() && !DragDropManager::GetListDragOperation().IsPending())
        {
            DragDropManager::ResetListPreviewPosition(activeBoard);
        }

        // Add List UI
        ImGui::SameLine();
        ImGui::SetCursorPosX(startX + (cardLists.size() * (cardListWidth + spacingX)));
        ImGui::SetCursorPosY(startY);

        if(!mUIState.isAddingList)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 255, 255, 20));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 40));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 255, 255, 60));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f * dpiScale);

            if(ImGui::Button(
                   ICON_FA_PLUS "  Add another list",
                   ImVec2(cardListWidth, 35 * dpiScale)
               ))
            {
                mUIState.isAddingList = true;
                memset(mUIState.newListTitleBuffer, 0, sizeof(mUIState.newListTitleBuffer));
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
                mUIState.newListTitleBuffer,
                sizeof(mUIState.newListTitleBuffer),
                ImGuiInputTextFlags_EnterReturnsTrue
            );
            ImGui::PopItemWidth();

            ImGui::Spacing();

            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(59, 130, 246, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(37, 99, 235, 255));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f * dpiScale);
            if(ImGui::Button("Add list") || enterPressed)
            {
                if(strlen(mUIState.newListTitleBuffer) > 0)
                {
                    CreateList(mUIState.newListTitleBuffer);
                    mUIState.isAddingList = false;
                }
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(2);

            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 20));
            if(ImGui::Button(ICON_FA_XMARK) || ImGui::IsKeyPressed(ImGuiKey_Escape))
            {
                mUIState.isAddingList = false;
            }
            ImGui::PopStyleColor(2);

            ImGui::EndChild();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }

        ImGui::EndChild();
    }

    void BoardViewController::RenderCreateBoardPopup()
    {
        const float dpiScale = FontManager::GetDpiScale();

        if(mUIState.isCreatingBoard)
        {
            ImGui::OpenPopup("Create Board");
            mUIState.isCreatingBoard = false;
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
                mUIState.newBoardTitleBuffer,
                sizeof(mUIState.newBoardTitleBuffer),
                ImGuiInputTextFlags_EnterReturnsTrue
            );

            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            FontManager::Pop();

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();

            float width = ImGui::GetContentRegionAvail().x;
            float btnWidth = (width - 10.0f * dpiScale) * 0.5f;

            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(45, 45, 50, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(60, 60, 65, 255));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f * dpiScale);
            if(ImGui::Button("Cancel", ImVec2(btnWidth, 35 * dpiScale))
               || ImGui::IsKeyPressed(ImGuiKey_Escape))
            {
                ImGui::CloseCurrentPopup();
                mUIState.isCreatingBoard = false;
            }
            ImGui::PopStyleColor(2);

            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(59, 130, 246, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(37, 99, 235, 255));
            if(ImGui::Button("Create", ImVec2(btnWidth, 35 * dpiScale)) || enter)
            {
                if(strlen(mUIState.newBoardTitleBuffer) > 0)
                {
                    CreateBoard(mUIState.newBoardTitleBuffer);
                    ImGui::CloseCurrentPopup();
                    mUIState.isCreatingBoard = false;
                }
            }
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar();

            ImGui::EndPopup();
        }
        ImGui::PopStyleVar(2);
    }

    void BoardViewController::RenderDeleteConfirmPopup()
    {
        // TODO: Implement delete confirmation dialog
    }

    void BoardViewController::CreateList(const std::string& title)
    {
        if(auto* board = GetActiveBoard())
        {
            // Get the position for the new list (append to end)
            int position = static_cast<int>(board->lists.size());
            
            // Create the list in the database and get back a list with DB-generated ID
            CardList newList = BoardStorageAdapter::CreateList(board->id, title, position);
            
            // Add the list to the board's in-memory collection
            board->lists.push_back(std::move(newList));
        }
    }

    void BoardViewController::CreateBoard(const std::string& title)
    {
        auto& board = mRepository.Create(title);
        SetActiveBoard(board.id);
    }

    void BoardViewController::DeleteBoard(const std::string& id)
    {
        if(mRepository.Delete(id))
        {
            // Switch to another board if we deleted the active one
            if(mActiveBoardId == id && !mRepository.IsEmpty())
            {
                SetActiveBoard(mRepository.GetAll().front().id);
            }
        }
    }
}
