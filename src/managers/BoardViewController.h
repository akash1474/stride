#pragma once
#include "BoardRepository.h"
#include "renderers/CardListRenderer.h"
#include <string>
#include <unordered_map>

namespace Stride
{
    /**
     * @brief UI state container for board-level view operations.
     *
     * Manages transient UI state for the board view interface, including:
     * - Add list form state (visibility, input buffer)
     * - Create board popup state
     * - Delete confirmation dialog state
     *
     * This is a simple data structure with no business logic, used by
     * BoardViewController to track UI interaction state across frames.
     *
     * @note All string buffers are fixed-size for ImGui compatibility.
     */
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

    /**
     * @brief View controller managing board UI rendering and user interactions.
     *
     * BoardViewController implements the Model-View-Controller pattern, acting as
     * the Controller layer between BoardRepository (Model) and ImGui rendering (View).
     *
     * Key Responsibilities:
     * - Rendering the complete board UI (nav bar, lists, cards, popups)
     * - Managing active board selection
     * - Handling user input and translating to repository operations
     * - Maintaining per-list UI state (scroll position, editor state, etc.)
     * - Coordinating drag-drop operations via DragDropManager
     *
     * The controller maintains UI state separately from data models, ensuring
     * clean separation of concerns. Each CardList has associated UI state
     * (CardListUIState) stored in the view controller.
     *
     * Usage:
     * @code
     * BoardRepository repo;
     * BoardViewController controller(repo);
     * controller.SetActiveBoard(boardId);
     * controller.Render();  // Call each frame
     * @endcode
     *
     * @note This class does NOT own the repository - it only holds a reference.
     * @see BoardRepository, BoardData, CardListRenderer, DragDropManager
     */
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
