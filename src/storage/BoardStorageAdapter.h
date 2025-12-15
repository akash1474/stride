#pragma once
#include "managers/BoardData.h"
#include "Card.h"
#include "CardList.h"
#include "storage/Storage.h"
#include "storage/StorageManager.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace Stride
{
    /**
     * @brief Adapter layer converting between domain models and storage models.
     *
     * BoardStorageAdapter provides bidirectional conversion between:
     * - Domain models (Stride::BoardData, Card, CardList) used by the UI
     * - Storage models (Storage::BoardData, CardData, ListData) used by SQLite
     *
     * Key Responsibilities:
     * - ID mapping: Domain string IDs ↔ Storage integer IDs
     * - Structure flattening: Nested domain objects → Flat relational tables
     * - Position management: Array indices → Fractional positioning
     * - Timestamp handling: Domain timestamps ↔ Unix epochs
     *
     * The adapter maintains ID mappings to ensure referential integrity between
     * the in-memory domain models and persisted database records.
     *
     * Usage:
     * @code
     * // Save a board
     * BoardStorageAdapter adapter;
     * int dbId = adapter.SaveFullBoard(domainBoard);
     *
     * // Load a board
     * BoardData board = adapter.LoadFullBoard(dbId);
     * @endcode
     *
     * @note This adapter is stateless for individual conversions but maintains
     *       ID mappings during batch operations.
     * @see BoardRepository, StorageManager
     */
    class BoardStorageAdapter
    {
      public:
        BoardStorageAdapter() = default;

        // ============================================================
        // HIGH-LEVEL OPERATIONS (Recommended API)
        // ============================================================

        /**
         * @brief Load a complete board with all lists and cards from database.
         * @param boardId Database ID of the board
         * @return Fully populated BoardData with nested lists and cards
         * @throws std::runtime_error if board not found
         */
        static BoardData LoadFullBoard(int boardId);

        /**
         * @brief Save a complete board with all lists and cards to database.
         * @param board Domain board to persist
         * @return Database ID of the saved board (new or existing)
         *
         * This performs a full save:
         * - Inserts/updates board record
         * - Inserts/updates all lists
         * - Inserts/updates all cards with checklists and badges
         */
        static int SaveFullBoard(const BoardData& board);

        /**
         * @brief Update only the board metadata (title, timestamps, etc).
         * @param board Domain board with updated metadata
         */
        static void UpdateBoardMetadata(const BoardData& board);

        /**
         * @brief Delete a board and all associated data from database.
         * @param boardId Database ID of the board to delete
         * @note Cascading deletes will remove all lists, cards, badges, etc.
         */
        static void DeleteBoard(int boardId);

        // ============================================================
        // ENTITY CREATION (Returns objects with database-generated IDs)
        // ============================================================

        /**
         * @brief Create a new board in the database.
         * @param title Board title
         * @param description Board description (optional)
         * @param backgroundImage Background image path (optional)
         * @return BoardData with database-generated ID
         */
        static BoardData CreateBoard(
            const std::string& title,
            const std::string& description = "",
            const std::string& backgroundImage = ""
        );

        /**
         * @brief Create a new list in the database.
         * @param boardId Parent board ID (prefixed string)
         * @param title List title
         * @param position Position for ordering
         * @return CardList with database-generated ID
         */
        static CardList
        CreateList(const std::string& boardId, const std::string& title, int position);

        /**
         * @brief Create a new card in the database.
         * @param listId Parent list ID (prefixed string)
         * @param title Card title
         * @param description Card description
         * @param position Position for ordering
         * @return Card with database-generated ID
         */
        static Card CreateCard(
            const std::string& listId,
            const std::string& title,
            const std::string& description,
            int position
        );

        /**
         * @brief Create a new checklist item in the database.
         * @param cardId Parent card ID (prefixed string)
         * @param title Item title
         * @param isCompleted Completion status
         * @param position Position for ordering
         * @return ChecklistItem with database-generated ID
         */
        static ChecklistItem CreateChecklistItem(
            const std::string& cardId,
            const std::string& title,
            bool isCompleted,
            int position
        );

        /**
         * @brief Create a new badge and attach it to a card.
         * @param cardId Parent card ID (prefixed string)
         * @param text Badge text/label
         * @param colorName Color scheme name
         * @return Badge text (badges are identified by text, not IDs)
         */
        static std::string CreateBadge(
            const std::string& cardId,
            const std::string& text,
            const std::string& colorName
        );

        // ============================================================
        // LOW-LEVEL CONVERSION (For Advanced Use)
        // ============================================================

        // Domain → Storage conversion
        static Storage::BoardData ToStorageBoard(const BoardData& board);
        static Storage::ListData ToStorageList(const CardList& list, int boardId, int position);
        static Storage::CardData
        ToStorageCard(const Card& card, int listId, int boardId, int position);
        static Storage::ChecklistItemData
        ToStorageChecklistItem(const ChecklistItem& item, int cardId, int position);

        // Storage → Domain conversion
        static BoardData FromStorage(
            const Storage::BoardData& storageBoard,
            const std::vector<Storage::ListData>& storageLists,
            const std::vector<Storage::CardData>& storageCards,
            const std::vector<Storage::ChecklistItemData>& storageChecklist,
            const std::vector<Storage::BadgeData>& storageBadges,
            const std::vector<Storage::CardBadgeData>& cardBadgeLinks
        );

        // ============================================================
        // ID CONVERSION (Prefixed String IDs)
        // ============================================================

        /**
         * @brief Create a prefixed domain ID from database integer ID.
         * @param dbId Database integer ID
         * @param prefix Entity type prefix ("board", "list", "card", "item")
         * @return Prefixed string ID (e.g., "card_123")
         */
        static std::string MakeId(int dbId, const std::string& prefix);

        /**
         * @brief Parse database integer ID from prefixed domain ID.
         * @param stringId Prefixed domain ID (e.g., "card_123")
         * @return Database integer ID, or 0 if invalid format
         */
        static int ParseId(const std::string& stringId);

        /**
         * @brief Check if a string ID has a specific prefix.
         * @param stringId Domain ID to check
         * @param prefix Expected prefix (e.g., "card")
         * @return true if ID starts with prefix_
         */
        static bool HasPrefix(const std::string& stringId, const std::string& prefix);

      private:
        // Helper functions
        static int CalculatePosition(size_t index, size_t total);
        static CardList FromStorageList(
            const Storage::ListData& storageList,
            const std::vector<Storage::CardData>& cardsInList,
            const std::vector<Storage::ChecklistItemData>& allChecklist,
            const std::vector<Storage::BadgeData>& allBadges,
            const std::vector<Storage::CardBadgeData>& cardBadgeLinks
        );
        static Card FromStorageCard(
            const Storage::CardData& storageCard,
            const std::vector<Storage::ChecklistItemData>& checklistItems,
            const std::vector<std::string>& badgeNames
        );
        static ChecklistItem FromStorageChecklistItem(const Storage::ChecklistItemData& item);

        // Badge helpers
        static std::vector<std::string> GetBadgeNamesForCard(
            int cardId,
            const std::vector<Storage::BadgeData>& allBadges,
            const std::vector<Storage::CardBadgeData>& cardBadgeLinks
        );
    };
}
