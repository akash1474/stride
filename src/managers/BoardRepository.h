#pragma once
#include "BoardData.h"
#include <vector>
#include <string>
#include <optional>
#include <functional>

namespace Stride
{
    /**
     * @brief Data access layer for managing BoardData persistence and lifecycle.
     *
     * BoardRepository implements the Repository pattern, providing CRUD operations
     * and data management for boards. It serves as the single source of truth for
     * all board data in the application.
     *
     * Key Responsibilities:
     * - Creating new boards with unique IDs
     * - Storing and retrieving boards by ID
     * - Deleting boards
     * - Event notifications for data changes (Observer pattern)
     * - Future: Persistence to disk/database
     *
     * The repository maintains an in-memory collection of boards and notifies
     * registered observers when boards are created, modified, or deleted.
     *
     *
     * @note Thread safety is not currently implemented - all operations should
     *       be performed on the main thread.
     * @see BoardData, BoardManager, BoardViewController
     */
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
