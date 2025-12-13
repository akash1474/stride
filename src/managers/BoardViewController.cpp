#include "pch.h"
#include "BoardViewController.h"
#include "renderers/CardListRenderer.h"
#include "managers/DragDropManager.h"
#include "managers/FontManager.h"
#include "FontAwesome6.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace Stride
{
    BoardViewController::BoardViewController(BoardRepository& repository)
        : mRepository(repository)
    {}
    
    void BoardViewController::SetActiveBoard(const std::string& id)
    {
        mActiveBoardId = id;
        mUIState.Reset();
    }
    
    BoardData* BoardViewController::GetActiveBoard()
    {
        return mRepository.GetById(mActiveBoardId);
    }
    
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
        BoardData* activeBoard = GetActiveBoard();
        if (!activeBoard) return;
        
        const float dpiScale = FontManager::GetDpiScale();
        
        // Setup main window
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        
        ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(30, 32, 36, 255));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        
        ImGui::Begin("Board", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBringToFrontOnFocus);
        
        RenderNavBar();
        RenderBoardContent();
        RenderCreateBoardPopup();
        
        // Handle drag-drop - pass board data
        FontManager::Push(FontFamily::Regular, FontSize::Regular);
        DragDropManager::DrawTooltipOfDraggedItem(activeBoard);
        DragDropManager::UpdateDropZone();
        
        DragOperation& op = DragDropManager::GetDragOperation();
        if (op.IsPending())
        {
            DragDropManager::PerformDropOperation(activeBoard);
        }
        FontManager::Pop();
        
        ImGui::End();
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor();
    }
    
    void BoardViewController::RenderNavBar()
    {
        BoardData* activeBoard = GetActiveBoard();
        if (!activeBoard) return;
        
        const float dpiScale = FontManager::GetDpiScale();
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
        ImGui::Text("%s", activeBoard->title.c_str());
        FontManager::Pop();
        
        RenderBoardSwitcher();
        
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }
    
    void BoardViewController::RenderBoardSwitcher()
    {
        BoardData* activeBoard = GetActiveBoard();
        if (!activeBoard) return;
        
        const float dpiScale = FontManager::GetDpiScale();
        float buttonSize = 30 * dpiScale;
        float rightOffset = 15 * dpiScale;
        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - buttonSize - rightOffset, 5 * dpiScale));
        
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
        if (ImGui::Button(ICON_FA_ELLIPSIS, ImVec2(buttonSize, buttonSize)))
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
        
        if (ImGui::BeginPopup("BoardSettingsPopup"))
        {
            FontManager::Push(FontFamily::SemiBold, FontSize::Small);
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "SWITCH BOARD");
            FontManager::Pop();
            
            ImGui::Spacing();
            
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 10.0f));
            for (const auto& b : mRepository.GetAll())
            {
                bool isSelected = (b.id == mActiveBoardId);
                std::string label = std::string(isSelected ? ICON_FA_CHECK "  " : "   ") + b.title;
                
                if (ImGui::Selectable(label.c_str(), isSelected, ImGuiSelectableFlags_SpanAvailWidth))
                {
                    SetActiveBoard(b.id);
                }
            }
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            if (ImGui::Selectable(
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
            
            if (ImGui::Selectable(ICON_FA_GEAR "  Settings", ImGuiSelectableFlags_SpanAvailWidth)) {}
            if (ImGui::Selectable(ICON_FA_CIRCLE_INFO "  About", ImGuiSelectableFlags_SpanAvailWidth)) {}
            
            ImGui::PopStyleVar();
            ImGui::EndPopup();
        }
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(3);
    }
    
    void BoardViewController::RenderBoardContent()
    {
        BoardData* activeBoard = GetActiveBoard();
        if (!activeBoard) return;
        
        const float dpiScale = FontManager::GetDpiScale();
        
        // Adjust height for Top Bar
        ImGui::SetCursorPosY(50 * dpiScale);
        ImGui::BeginChild("BoardContent", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
        
        float startX = 20.0f * dpiScale;
        float startY = 10.0f * dpiScale;
        float cardListWidth = 280.0f * dpiScale;
        float spacingX = 12.0f * dpiScale;
        
        ImGui::SetCursorPos(ImVec2(startX, startY));
        float fullHeight = ImGui::GetContentRegionAvail().y;
        
        std::vector<CardList>& cardLists = activeBoard->lists;
        
        for (int i = 0; i < (int)cardLists.size(); ++i)
        {
            if (i > 0)
            {
                ImGui::SameLine();
                ImGui::SetCursorPosX(startX + (i * (cardListWidth + spacingX)));
            }
            
            CardListUIState& uiState = GetListUIState(cardLists[i].id);
            CardEditorState& editorState = GetEditorState(cardLists[i].id);
            
            CardListRenderer::Render(
                cardLists[i],
                uiState,
                editorState,
                i,
                ImVec2(cardListWidth, fullHeight - 20.0f * dpiScale)
            );
        }
        
        // Add List UI
        ImGui::SameLine();
        ImGui::SetCursorPosX(startX + (cardLists.size() * (cardListWidth + spacingX)));
        ImGui::SetCursorPosY(startY);
        
        if (!mUIState.isAddingList)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 255, 255, 20));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 40));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 255, 255, 60));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f * dpiScale);
            
            if (ImGui::Button(ICON_FA_PLUS "  Add another list", ImVec2(cardListWidth, 35 * dpiScale)))
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
            if (ImGui::IsWindowAppearing())
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
            if (ImGui::Button("Add list") || enterPressed)
            {
                if (strlen(mUIState.newListTitleBuffer) > 0)
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
            if (ImGui::Button(ICON_FA_XMARK) || ImGui::IsKeyPressed(ImGuiKey_Escape))
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
        
        if (mUIState.isCreatingBoard)
        {
            ImGui::OpenPopup("Create Board");
            mUIState.isCreatingBoard = false;
        }
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20 * dpiScale, 20 * dpiScale));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
        ImGui::SetNextWindowSize(ImVec2(400 * dpiScale, 0));
        
        if (ImGui::BeginPopupModal(
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
            
            if (ImGui::IsWindowAppearing())
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
            if (ImGui::Button("Cancel", ImVec2(btnWidth, 35 * dpiScale))
                || ImGui::IsKeyPressed(ImGuiKey_Escape))
            {
                ImGui::CloseCurrentPopup();
                mUIState.isCreatingBoard = false;
            }
            ImGui::PopStyleColor(2);
            
            ImGui::SameLine();
            
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(59, 130, 246, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(37, 99, 235, 255));
            if (ImGui::Button("Create", ImVec2(btnWidth, 35 * dpiScale)) || enter)
            {
                if (strlen(mUIState.newBoardTitleBuffer) > 0)
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
        if (auto* board = GetActiveBoard())
        {
            board->AddList(title);
        }
    }
    
    void BoardViewController::CreateBoard(const std::string& title)
    {
        auto& board = mRepository.Create(title);
        SetActiveBoard(board.id);
    }
    
    void BoardViewController::DeleteBoard(const std::string& id)
    {
        if (mRepository.Delete(id))
        {
            // Switch to another board if we deleted the active one
            if (mActiveBoardId == id && !mRepository.IsEmpty())
            {
                SetActiveBoard(mRepository.GetAll().front().id);
            }
        }
    }
}
