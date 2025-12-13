#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "BoardData.h"
#include "BoardRepository.h"
#include "BoardViewController.h"

using Stride::CardEditorState;
using Stride::CardListUIState;

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
    CardListUIState& GetListUIState(const std::string& listId);
    CardEditorState& GetEditorState(const std::string& listId);
    
    // Access to internals (for advanced use)
    Stride::BoardRepository& GetRepository() { return *mRepository; }
    Stride::BoardViewController& GetViewController() { return *mViewController; }

  private:
    BoardManager();
    
    void CreateSampleData();

    std::unique_ptr<Stride::BoardRepository> mRepository;
    std::unique_ptr<Stride::BoardViewController> mViewController;
};
