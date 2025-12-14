#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "BoardData.h"
#include "BoardRepository.h"
#include "BoardViewController.h"


/**
 * @brief Facade and singleton coordinator for the board management system.
 *
 * BoardManager provides a unified interface for board operations, acting as the main
 * entry point for board-related functionality in the application. It coordinates between:
 * - BoardRepository: Handles data storage and CRUD operations
 * - BoardViewController: Manages UI rendering and user interactions
 *
 * Key Responsibilities:
 * - Application initialization and setup
 * - Delegating board operations to the repository
 * - Delegating UI rendering to the view controller
 * - Maintaining references to the active board
 * - Creating sample/demo data for testing
 *
 * @note This is a singleton - use BoardManager::Get() to access the instance.
 * @see BoardRepository, BoardViewController, BoardData
 */
class BoardManager
{
  public:
    static BoardManager& Get()
    {
        static BoardManager instance;
        return instance;
    }

    void Setup();
    void Render();

    // Board operations (delegates to repository)
    Stride::BoardData& CreateBoard(const std::string& title);
    void SetActiveBoard(const std::string& id);
    Stride::BoardData* GetActiveBoard();
    std::vector<Stride::BoardData>& GetBoards();
    Stride::BoardData* GetBoard(const std::string& id);
    bool DeleteBoard(const std::string& id);

    // List operations (on active board)
    void AddList(const std::string& title);

    // Get UI state for a card list (delegates to view controller)
    Stride::CardListUIState& GetListUIState(const std::string& listId);
    Stride::CardEditorState& GetEditorState(const std::string& listId);

    // Access to internals (for advanced use)
    Stride::BoardRepository& GetRepository() { return *mRepository; }
    Stride::BoardViewController& GetViewController() { return *mViewController; }

  private:
    BoardManager();

    void CreateSampleData();

    std::unique_ptr<Stride::BoardRepository> mRepository;
    std::unique_ptr<Stride::BoardViewController> mViewController;
};
