#include "pch.h"
#include "BoardRepository.h"
#include "Utils.h"
#include <algorithm>

namespace Stride
{
    BoardData& BoardRepository::Create(const std::string& title)
    {
        std::string id = GenerateId();
        mBoards.emplace_back(id, title);
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
    
    bool BoardRepository::SaveToFile(const std::string& path) const
    {
        // TODO: Implement persistence
        return false;
    }
    
    bool BoardRepository::LoadFromFile(const std::string& path)
    {
        // TODO: Implement persistence
        return false;
    }
}
