#pragma once
#include <vector>
#include <string>
#include "BoardData.h"

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

    BoardData& CreateBoard(const std::string& title);
    void SetActiveBoard(const std::string& id);
    BoardData* GetActiveBoard();
    std::vector<BoardData>& GetBoards() { return mBoards; }
    BoardData* GetBoard(const std::string& id);

    // Board Specific Operations (Proxies to Active Board)
    void AddList(const std::string& title);

private:
    BoardManager() = default;

    std::vector<BoardData> mBoards;
    std::string mActiveBoardID;

    // UI State for Active Board
    bool mIsAddingList = false;
    char mNewListTitleBuffer[512] = "";

    // UI State for Board Creation
    bool mIsCreatingBoard = false;
    char mNewBoardTitleBuffer[512] = "";
};
