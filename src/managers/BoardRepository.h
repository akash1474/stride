#pragma once
#include "BoardData.h"
#include <vector>
#include <string>
#include <optional>
#include <functional>

namespace Stride
{
    class BoardRepository
    {
    public:
        // CRUD operations
        BoardData& Create(const std::string& title);
        BoardData* GetById(const std::string& id);
        const BoardData* GetById(const std::string& id) const;
        std::vector<BoardData>& GetAll() { return mBoards; }
        const std::vector<BoardData>& GetAll() const { return mBoards; }
        bool Delete(const std::string& id);
        
        // Query
        bool Exists(const std::string& id) const;
        size_t Count() const { return mBoards.size(); }
        bool IsEmpty() const { return mBoards.empty(); }
        
        // Events
        using BoardChangedCallback = std::function<void(const std::string& boardId)>;
        void OnBoardCreated(BoardChangedCallback cb);
        void OnBoardDeleted(BoardChangedCallback cb);
        void OnBoardModified(BoardChangedCallback cb);
        
        // Persistence (placeholder for future implementation)
        bool SaveToFile(const std::string& path) const;
        bool LoadFromFile(const std::string& path);
        
    private:
        std::vector<BoardData> mBoards;
        
        std::vector<BoardChangedCallback> mOnCreated;
        std::vector<BoardChangedCallback> mOnDeleted;
        std::vector<BoardChangedCallback> mOnModified;
        
        std::string GenerateId();
        void NotifyCreated(const std::string& id);
        void NotifyDeleted(const std::string& id);
        void NotifyModified(const std::string& id);
    };
}
