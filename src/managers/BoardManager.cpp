#include "pch.h"
#include "BoardManager.h"
#include "BoardRepository.h"
#include "BoardViewController.h"

using Stride::BoardData;
using Stride::BoardRepository;
using Stride::BoardViewController;
using Stride::Card;

BoardManager::BoardManager()
    : mRepository(std::make_unique<BoardRepository>())
    , mViewController(std::make_unique<BoardViewController>(*mRepository))
{}

Stride::CardListUIState& BoardManager::GetListUIState(const std::string& listId)
{
    return mViewController->GetListUIState(listId);
}

Stride::CardEditorState& BoardManager::GetEditorState(const std::string& listId)
{
    return mViewController->GetEditorState(listId);
}

void BoardManager::Setup()
{
    // Load all boards from database
    mRepository->LoadAll();
    mViewController->SetViewMode(Stride::ViewMode::Home);
    // CreateSampleData(); // Uncomment for demo/testing
}

void BoardManager::CreateSampleData()
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

    Stride::CardList& list1 = board.AddList("Super Long Title is wrapping");
    list1.cards = std::move(mWindowACards);
    Stride::CardList& list2 = board.AddList("Doing");
    list2.cards = std::move(mWindowBCards);
    board.AddList("Done");

    SetActiveBoard(board.id);
}

BoardData& BoardManager::CreateBoard(const std::string& title)
{
    return mRepository->Create(title);
}

void BoardManager::SetActiveBoard(const std::string& id)
{
    mViewController->SetActiveBoard(id);
}

BoardData* BoardManager::GetActiveBoard() 
{ 
    return mViewController->GetActiveBoard();
}

std::vector<BoardData>& BoardManager::GetBoards()
{
    return mRepository->GetAll();
}

BoardData* BoardManager::GetBoard(const std::string& id)
{
    return mRepository->GetById(id);
}

bool BoardManager::DeleteBoard(const std::string& id)
{
    return mRepository->Delete(id);
}

void BoardManager::AddList(const std::string& title)
{
    BoardData* board = GetActiveBoard();
    if (board)
    {
        board->AddList(title);
    }
}

void BoardManager::Render()
{
    mViewController->Render();
}
