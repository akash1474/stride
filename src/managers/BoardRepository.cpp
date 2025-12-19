#include "pch.h"
#include "BoardRepository.h"
#include "Utils.h"
#include <algorithm>
#include "storage/BoardStorageAdapter.h"
#include "Log.h"

namespace Stride
{
    BoardData& BoardRepository::Create(const std::string& title)
    {
        BoardData tBoardData = BoardStorageAdapter::CreateBoard(title);
        const std::string id = tBoardData.id;
        mBoards.emplace_back(std::move(tBoardData));
        NotifyCreated(id);
        return mBoards.back();
    }
    
    BoardData* BoardRepository::GetById(const std::string& id)
    {
        auto it = std::find_if(mBoards.begin(), mBoards.end(),
            [&](const BoardData& b) { return b.id == id; });
        return it != mBoards.end() ? &(*it) : nullptr;
    }
    
    const BoardData* BoardRepository::GetById(const std::string& id) const
    {
        auto it = std::find_if(mBoards.begin(), mBoards.end(),
            [&](const BoardData& b) { return b.id == id; });
        return it != mBoards.end() ? &(*it) : nullptr;
    }
    
    bool BoardRepository::Delete(const std::string& id)
    {
        auto it = std::find_if(mBoards.begin(), mBoards.end(),
            [&](const BoardData& b) { return b.id == id; });
        
        if (it != mBoards.end())
        {
            mBoards.erase(it);
            NotifyDeleted(id);
            return true;
        }
        return false;
    }
    
    bool BoardRepository::Exists(const std::string& id) const
    {
        return GetById(id) != nullptr;
    }
    
    std::string BoardRepository::GenerateId()
    {
        return "board_" + genUID(12);
    }
    
    void BoardRepository::OnBoardCreated(BoardChangedCallback cb)
    {
        mOnCreated.push_back(std::move(cb));
    }
    
    void BoardRepository::OnBoardDeleted(BoardChangedCallback cb)
    {
        mOnDeleted.push_back(std::move(cb));
    }
    
    void BoardRepository::OnBoardModified(BoardChangedCallback cb)
    {
        mOnModified.push_back(std::move(cb));
    }
    
    void BoardRepository::NotifyCreated(const std::string& id)
    {
        for (const auto& cb : mOnCreated)
        {
            cb(id);
        }
    }
    
    void BoardRepository::NotifyDeleted(const std::string& id)
    {
        for (const auto& cb : mOnDeleted)
        {
            cb(id);
        }
    }
    
    void BoardRepository::NotifyModified(const std::string& id)
    {
        for (const auto& cb : mOnModified)
        {
            cb(id);
        }
    }
    
    void BoardRepository::LoadAll()
    {
        GL_INFO("Loading all boards from database...");
        
        try
        {
            // Load all boards from storage
            std::vector<BoardData> loadedBoards = BoardStorageAdapter::LoadAllBoards();
            
            // Clear existing boards and replace with loaded ones
            mBoards = std::move(loadedBoards);
            
            GL_INFO("Successfully loaded {} boards into repository", mBoards.size());
            
            // Notify observers for each loaded board
            for(const auto& board : mBoards)
            {
                NotifyCreated(board.id);
            }
        }
        catch(const std::exception& e)
        {
            GL_ERROR("Failed to load boards: {}", e.what());
        }
    }
}
