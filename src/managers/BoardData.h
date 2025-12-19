#pragma once
#include "CardList.h"
#include <string>
#include <vector>
#include <optional>
#include <cstdint>

namespace Stride
{
    /**
     * @brief Represents a Kanban-style board containing multiple card lists.
     *
     * BoardData is the core data model for a board in the application. It manages:
     * - Board metadata (id, title, timestamps, archive status)
     * - A collection of CardList objects organized in order
     * - Visual customization (background color/image)
     * - Cross-list operations (moving cards between lists)
     *
     * This is a pure data structure with business logic for managing its content.
     * The BoardRepository handles persistence, while BoardViewController manages UI state.
     *
     * @note Each board has a unique string ID generated at creation time.
     * @see CardList, Card, BoardRepository, BoardViewController
     */
    struct BoardData
    {
        // Identity
        std::string id;
        std::string title;
        std::string description;

        // Content
        std::vector<CardList> lists;

        // Appearance
        std::string backgroundColor;
        std::string backgroundImage;

        // Metadata
        int64_t createdAt = 0;
        int64_t updatedAt = 0;
        bool archived = false;

        // Constructors
        BoardData();
        BoardData(std::string title);
        BoardData(std::string id, std::string title);

        // List operations
        CardList& AddList(const std::string& title);
        CardList& InsertList(const std::string& title, size_t index);
        void RemoveList(const std::string& listId);
        void MoveList(size_t fromIndex, size_t toIndex);
        void UpdateListPositions(); 

        // Query
        CardList* FindList(const std::string& listId);
        const CardList* FindList(const std::string& listId) const;
        std::optional<size_t> GetListIndex(const std::string& listId) const;

        // Card operations (cross-list)
        Card* FindCard(const std::string& cardId);
        const Card* FindCard(const std::string& cardId) const;
        std::pair<CardList*, Card*> FindCardWithList(const std::string& cardId);

        void
        MoveCard(const std::string& cardId, const std::string& targetListId, size_t targetIndex);

        // Statistics
        size_t GetTotalCardCount() const;
        size_t GetListCount() const { return lists.size(); }
        bool IsEmpty() const { return lists.empty(); }

        // Validation
        bool IsValid() const { return !id.empty() && !title.empty(); }
    };
}
