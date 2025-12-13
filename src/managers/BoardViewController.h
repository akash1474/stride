#pragma once
#include "BoardRepository.h"
#include "renderers/CardListRenderer.h"
#include <string>
#include <unordered_map>

namespace Stride
{
    struct BoardViewUIState
    {
        // Add list UI
        bool isAddingList = false;
        char newListTitleBuffer[512] = "";
        
        // Create board popup
        bool isCreatingBoard = false;
        char newBoardTitleBuffer[512] = "";
        
        // Delete confirmation
        bool showDeleteConfirm = false;
        std::string boardToDelete;
        
        void Reset()
        {
            isAddingList = false;
            memset(newListTitleBuffer, 0, sizeof(newListTitleBuffer));
            isCreatingBoard = false;
            memset(newBoardTitleBuffer, 0, sizeof(newBoardTitleBuffer));
            showDeleteConfirm = false;
            boardToDelete.clear();
        }
    };

    class BoardViewController
    {
    public:
        explicit BoardViewController(BoardRepository& repository);
        
        // Active board management
        void SetActiveBoard(const std::string& id);
        std::string GetActiveBoardId() const { return mActiveBoardId; }
        BoardData* GetActiveBoard();
        const BoardData* GetActiveBoard() const;
        
        // Render
        void Render();
        
        // UI State access
        BoardViewUIState& GetUIState() { return mUIState; }
        
        // Get UI state for a card list
        CardListUIState& GetListUIState(const std::string& listId);
        CardEditorState& GetEditorState(const std::string& listId);
        
    private:
        BoardRepository& mRepository;
        std::string mActiveBoardId;
        BoardViewUIState mUIState;
        
        // UI state storage for card lists
        std::unordered_map<std::string, CardListUIState> mListUIStates;
        std::unordered_map<std::string, CardEditorState> mEditorStates;
        
        // Render components
        void RenderNavBar();
        void RenderBoardContent();
        void RenderBoardSwitcher();
        void RenderCreateBoardPopup();
        void RenderDeleteConfirmPopup();
        
        // Actions
        void CreateList(const std::string& title);
        void CreateBoard(const std::string& title);
        void DeleteBoard(const std::string& id);
    };
}
