#include "pch.h"
#include "BoardStorageAdapter.h"
#include "Log.h"
#include <algorithm>
#include <stdexcept>

namespace Stride
{
    // ============================================================
    // HIGH-LEVEL OPERATIONS
    // ============================================================

    BoardData BoardStorageAdapter::LoadFullBoard(int boardId)
    {
        try
        {
            // Load board metadata
            auto storageBoard = StorageManager::GetBoard(boardId);

            // Load all lists for this board
            auto storageLists = StorageManager::GetListsInBoard(boardId);

            // Load all cards for this board (more efficient than per-list queries)
            std::vector<Storage::CardData> allCards;
            for(const auto& list : storageLists)
            {
                auto listCards = StorageManager::GetCardsInList(list.id);
                allCards.insert(allCards.end(), listCards.begin(), listCards.end());
            }

            // Load all checklist items
            std::vector<Storage::ChecklistItemData> allChecklist;
            for(const auto& card : allCards)
            {
                auto items = StorageManager::GetChecklistItemsForCard(card.id);
                allChecklist.insert(allChecklist.end(), items.begin(), items.end());
            }

            // Load badges
            auto badges = StorageManager::GetBadgesInBoard(boardId);
            std::vector<Storage::CardBadgeData> cardBadgeLinks;
            for(const auto& card : allCards)
            {
                auto cardBadges = StorageManager::GetBadgesForCard(card.id);
                for(const auto& badge : cardBadges)
                {
                    Storage::CardBadgeData link;
                    link.card_id = card.id;
                    link.badge_id = badge.id;
                    cardBadgeLinks.push_back(link);
                }
            }

            // Convert to domain model
            auto board = FromStorage(
                storageBoard,
                storageLists,
                allCards,
                allChecklist,
                badges,
                cardBadgeLinks
            );

            GL_INFO(
                "Loaded board '{}' with {} lists and {} cards",
                board.title,
                board.lists.size(),
                board.GetTotalCardCount()
            );

            return board;
        }
        catch(const std::exception& e)
        {
            GL_ERROR("Failed to load board {}: {}", boardId, e.what());
            throw;
        }
    }

    // ============================================================
    // ENTITY CREATION
    // ============================================================

    BoardData BoardStorageAdapter::CreateBoard(
        const std::string& title,
        const std::string& description,
        const std::string& backgroundImage
    )
    {
        // Create storage object
        Storage::BoardData storageBoard;
        storageBoard.name = title;
        // storageBoard.description = description;
        // storageBoard.background_image = backgroundImage;
        storageBoard.created_at = std::time(nullptr);
        storageBoard.updated_at = std::time(nullptr);

        // Insert and get the auto-generated ID
        int newBoardId = StorageManager::CreateBoard(storageBoard);

        // Create domain object with prefixed ID
        BoardData board;
        board.id = MakeId(newBoardId, "board");
        board.title = title;
        // board.description = description;
        board.backgroundImage = backgroundImage;
        board.createdAt = storageBoard.created_at;
        board.updatedAt = storageBoard.updated_at;
        board.lists.clear();

        GL_INFO("Created new board '{}' with ID: {}", title, board.id);
        return board;
    }

    CardList BoardStorageAdapter::CreateList(
        const std::string& boardId,
        const std::string& title,
        int position
    )
    {
        // Convert board ID from string to int
        int dbBoardId = ParseId(boardId);
        if(dbBoardId == 0)
        {
            throw std::invalid_argument("Invalid board ID: " + boardId);
        }

        // Create storage object
        Storage::ListData storageList;
        storageList.board_id = dbBoardId;
        storageList.name = title;
        storageList.position = position;
        storageList.created_at = std::time(nullptr);
        storageList.updated_at = std::time(nullptr);

        // Insert and get the auto-generated ID
        int newListId = StorageManager::CreateList(storageList);

        // Create domain object with prefixed ID
        CardList list;
        list.id = MakeId(newListId, "list");
        list.title = title;
        list.position = position;
        list.cards.clear();

        GL_INFO("Created new list '{}' with ID: {} in board: {}", title, list.id, boardId);
        return list;
    }

    Card BoardStorageAdapter::CreateCard(
        const std::string& listId,
        const std::string& title,
        const std::string& description,
        int position
    )
    {
        // Convert list ID from string to int
        int dbListId = ParseId(listId);
        if(dbListId == 0)
        {
            throw std::invalid_argument("Invalid list ID: " + listId);
        }

        // Get board ID from list
        auto list = StorageManager::GetList(dbListId);

        // Create storage object
        Storage::CardData storageCard;
        storageCard.list_id = dbListId;
        storageCard.board_id = list.board_id;
        storageCard.title = title;
        storageCard.description = description;
        storageCard.position = position;
        storageCard.cover_image = "";
        storageCard.due_date = 0;
        storageCard.completed = false;
        storageCard.created_at = std::time(nullptr);
        storageCard.updated_at = std::time(nullptr);

        // Insert and get the auto-generated ID
        int newCardId = StorageManager::CreateCard(storageCard);

        // Create domain object with prefixed ID
        Card card;
        card.id = MakeId(newCardId, "card");
        card.title = title;
        card.description = description;
        card.position = position;
        card.coverImage = "";
        card.dueDate = 0;
        card.isCompleted = false;
        card.badges.clear();
        card.checklist.clear();

        GL_INFO("Created new card '{}' with ID: {} in list: {}", title, card.id, listId);
        return card;
    }

    ChecklistItem BoardStorageAdapter::CreateChecklistItem(
        const std::string& cardId,
        const std::string& title,
        bool isCompleted,
        int position
    )
    {
        // Convert card ID from string to int
        int dbCardId = ParseId(cardId);
        if(dbCardId == 0)
        {
            throw std::invalid_argument("Invalid card ID: " + cardId);
        }

        // Create storage object
        Storage::ChecklistItemData storageItem;
        storageItem.card_id = dbCardId;
        storageItem.content = title;
        storageItem.completed = isCompleted;
        storageItem.position = position;

        // Insert and get the auto-generated ID
        int newItemId = StorageManager::CreateChecklistItem(storageItem);

        // Create domain object with prefixed ID
        ChecklistItem item;
        item.id = MakeId(newItemId, "item");
        item.text = title;
        item.isChecked = isCompleted;

        GL_INFO("Created new checklist item '{}' with ID: {} in card: {}", title, item.id, cardId);
        return item;
    }

    std::string BoardStorageAdapter::CreateBadge(
        const std::string& cardId,
        const std::string& text,
        const std::string& colorName
    )
    {
        // Convert card ID from string to int
        int dbCardId = ParseId(cardId);
        if(dbCardId == 0)
        {
            throw std::invalid_argument("Invalid card ID: " + cardId);
        }

        // Get card to find board ID
        auto card = StorageManager::GetCard(dbCardId);

        // Create or get badge
        Storage::BadgeData storageBadge;
        storageBadge.board_id = card.board_id;
        storageBadge.name = text;
        storageBadge.color = colorName;

        // Try to find existing badge with same text in this board
        auto existingBadges = StorageManager::GetBadgesInBoard(card.board_id);
        int badgeId = 0;
        for(const auto& badge : existingBadges)
        {
            if(badge.name == text)
            {
                badgeId = badge.id;
                break;
            }
        }

        // Create badge if it doesn't exist
        if(badgeId == 0)
        {
            badgeId = StorageManager::CreateBadge(storageBadge);
        }

        // Link badge to card
        StorageManager::AddBadgeToCard(dbCardId, badgeId);

        GL_INFO("Added badge '{}' to card: {}", text, cardId);
        return text;
    }

    // ============================================================
    // HIGH-LEVEL OPERATIONS (CONTINUED)
    // ============================================================

    int BoardStorageAdapter::SaveFullBoard(const BoardData& board)
    {
        try
        {
            int boardId = ParseId(board.id);

            // Convert board metadata
            auto storageBoard = ToStorageBoard(board);

            // Create or update board
            if(boardId == 0)
            {
                boardId = StorageManager::CreateBoard(storageBoard);
                GL_INFO("Created new board '{}' with DB ID {}", board.title, boardId);
            }
            else
            {
                storageBoard.id = boardId;
                StorageManager::UpdateBoard(storageBoard);
                GL_INFO("Updated board '{}' (DB ID {})", board.title, boardId);
            }

            // Save lists and cards
            for(size_t listIdx = 0; listIdx < board.lists.size(); ++listIdx)
            {
                const auto& list = board.lists[listIdx];
                int listPosition = listIdx;

                // Check if list exists in DB
                int listId = ParseId(list.id);
                auto storageList = ToStorageList(list, boardId, listPosition);

                if(listId == 0)
                {
                    listId = StorageManager::CreateList(storageList);
                }
                else
                {
                    storageList.id = listId;
                    StorageManager::UpdateList(storageList);
                }

                // Save cards in this list
                for(size_t cardIdx = 0; cardIdx < list.cards.size(); ++cardIdx)
                {
                    const auto& card = list.cards[cardIdx];
                    int cardPosition = cardIdx;

                    int cardId = ParseId(card.id);
                    auto storageCard = ToStorageCard(card, listId, boardId, cardPosition);

                    if(cardId == 0)
                    {
                        cardId = StorageManager::CreateCard(storageCard);
                    }
                    else
                    {
                        storageCard.id = cardId;
                        StorageManager::UpdateCard(storageCard);
                    }

                    // Save checklist items
                    // First, clear existing checklist items
                    auto existingItems = StorageManager::GetChecklistItemsForCard(cardId);
                    for(const auto& item : existingItems)
                    {
                        StorageManager::DeleteChecklistItem(item.id);
                    }

                    // Then save new checklist items
                    for(size_t itemIdx = 0; itemIdx < card.checklist.size(); ++itemIdx)
                    {
                        const auto& item = card.checklist[itemIdx];
                        int itemPosition = itemIdx;
                        auto storageItem = ToStorageChecklistItem(item, cardId, itemPosition);
                        StorageManager::CreateChecklistItem(storageItem);
                    }

                    // Save badges (simplified - just recreate associations)
                    // First remove existing badge associations
                    auto existingBadges = StorageManager::GetBadgesForCard(cardId);
                    for(const auto& badge : existingBadges)
                    {
                        StorageManager::RemoveBadgeFromCard(cardId, badge.id);
                    }

                    // Then add new badge associations
                    for(const auto& badgeName : card.badges)
                    {
                        // Find or create badge
                        auto boardBadges = StorageManager::GetBadgesInBoard(boardId);
                        int badgeId = 0;
                        for(const auto& badge : boardBadges)
                        {
                            if(badge.name == badgeName)
                            {
                                badgeId = badge.id;
                                break;
                            }
                        }

                        if(badgeId == 0)
                        {
                            // Create new badge
                            Storage::BadgeData newBadge;
                            newBadge.board_id = boardId;
                            newBadge.name = badgeName;
                            newBadge.color = "blue"; // Default color
                            badgeId = StorageManager::CreateBadge(newBadge);
                        }

                        StorageManager::AddBadgeToCard(cardId, badgeId);
                    }
                }
            }

            GL_INFO(
                "Successfully saved board '{}' with {} lists and {} cards",
                board.title,
                board.lists.size(),
                board.GetTotalCardCount()
            );

            return boardId;
        }
        catch(const std::exception& e)
        {
            GL_ERROR("Failed to save board '{}': {}", board.title, e.what());
            throw;
        }
    }

    void BoardStorageAdapter::UpdateBoardMetadata(const BoardData& board)
    {
        int boardId = ParseId(board.id);
        if(boardId == 0)
        {
            GL_WARN("Cannot update board metadata - invalid board ID: {}", board.id);
            return;
        }

        auto storageBoard = ToStorageBoard(board);
        storageBoard.id = boardId;
        StorageManager::UpdateBoard(storageBoard);
    }

    void BoardStorageAdapter::DeleteBoard(int boardId)
    {
        StorageManager::DeleteBoard(boardId);
        GL_INFO("Deleted board with DB ID {}", boardId);
    }

    // ============================================================
    // DOMAIN → STORAGE CONVERSION
    // ============================================================

    Storage::BoardData BoardStorageAdapter::ToStorageBoard(const BoardData& board)
    {
        Storage::BoardData storage;
        storage.id = ParseId(board.id); // Will be 0 for new boards
        storage.name = board.title;
        storage.created_at = board.createdAt;
        storage.updated_at = board.updatedAt;
        return storage;
    }

    Storage::ListData
    BoardStorageAdapter::ToStorageList(const CardList& list, int boardId, int position)
    {
        Storage::ListData storage;
        storage.id = ParseId(list.id);
        storage.board_id = boardId;
        storage.name = list.title;
        // Use provided position parameter (usually calculated), or list.position if non-zero
        storage.position = position;
        storage.created_at = 0; // TODO: Add timestamps to CardList
        storage.updated_at = 0;
        return storage;
    }

    Storage::CardData
    BoardStorageAdapter::ToStorageCard(const Card& card, int listId, int boardId, int position)
    {
        Storage::CardData storage;
        storage.id = ParseId(card.id);
        storage.list_id = listId;
        storage.board_id = boardId;
        storage.title = card.title;
        storage.description = card.description;
        // Use provided position parameter (usually calculated), or card.position if non-zero
        storage.position = position;
        storage.created_at = 0; // TODO: Add timestamps to Card
        storage.updated_at = 0;
        storage.due_date = 0;
        storage.completed = false;
        storage.cover_color = "";
        storage.cover_image = "";
        storage.archived = false;
        return storage;
    }

    Storage::ChecklistItemData
    BoardStorageAdapter::ToStorageChecklistItem(const ChecklistItem& item, int cardId, int position)
    {
        Storage::ChecklistItemData storage;
        storage.card_id = cardId;
        storage.content = item.text;
        storage.position = position;
        storage.completed = item.isChecked;
        return storage;
    }

    // ============================================================
    // STORAGE → DOMAIN CONVERSION
    // ============================================================

    BoardData BoardStorageAdapter::FromStorage(
        const Storage::BoardData& storageBoard,
        const std::vector<Storage::ListData>& storageLists,
        const std::vector<Storage::CardData>& storageCards,
        const std::vector<Storage::ChecklistItemData>& storageChecklist,
        const std::vector<Storage::BadgeData>& storageBadges,
        const std::vector<Storage::CardBadgeData>& cardBadgeLinks
    )
    {
        BoardData board;
        board.id = MakeId(storageBoard.id, "board");
        board.title = storageBoard.name;
        board.createdAt = storageBoard.created_at;
        board.updatedAt = storageBoard.updated_at;

        // Sort lists by position
        auto sortedLists = storageLists;
        std::sort(sortedLists.begin(), sortedLists.end(), [](const auto& a, const auto& b) {
            return a.position < b.position;
        });

        // Convert lists
        for(const auto& storageList : sortedLists)
        {
            // Filter cards for this list
            std::vector<Storage::CardData> cardsInList;
            for(const auto& card : storageCards)
            {
                if(card.list_id == storageList.id)
                {
                    cardsInList.push_back(card);
                }
            }

            auto list = FromStorageList(
                storageList,
                cardsInList,
                storageChecklist,
                storageBadges,
                cardBadgeLinks
            );
            board.lists.push_back(std::move(list));
        }

        return board;
    }

    CardList BoardStorageAdapter::FromStorageList(
        const Storage::ListData& storageList,
        const std::vector<Storage::CardData>& cardsInList,
        const std::vector<Storage::ChecklistItemData>& allChecklist,
        const std::vector<Storage::BadgeData>& allBadges,
        const std::vector<Storage::CardBadgeData>& cardBadgeLinks
    )
    {
        CardList list;
        list.id = MakeId(storageList.id, "list");
        list.title = storageList.name;
        list.position = storageList.position;

        // Sort cards by position
        auto sortedCards = cardsInList;
        std::sort(sortedCards.begin(), sortedCards.end(), [](const auto& a, const auto& b) {
            return a.position < b.position;
        });

        // Convert cards
        for(const auto& storageCard : sortedCards)
        {
            // Filter checklist items for this card
            std::vector<Storage::ChecklistItemData> checklistItems;
            for(const auto& item : allChecklist)
            {
                if(item.card_id == storageCard.id)
                {
                    checklistItems.push_back(item);
                }
            }

            // Get badge names for this card
            auto badgeNames = GetBadgeNamesForCard(storageCard.id, allBadges, cardBadgeLinks);

            auto card = FromStorageCard(storageCard, checklistItems, badgeNames);
            list.cards.push_back(std::move(card));
        }

        return list;
    }

    Card BoardStorageAdapter::FromStorageCard(
        const Storage::CardData& storageCard,
        const std::vector<Storage::ChecklistItemData>& checklistItems,
        const std::vector<std::string>& badgeNames
    )
    {
        Card card;
        card.id = MakeId(storageCard.id, "card");
        card.title = storageCard.title;
        card.description = storageCard.description;
        card.position = storageCard.position;
        card.badges = badgeNames;

        // Sort checklist items by position
        auto sortedItems = checklistItems;
        std::sort(sortedItems.begin(), sortedItems.end(), [](const auto& a, const auto& b) {
            return a.position < b.position;
        });

        // Convert checklist items
        for(const auto& item : sortedItems)
        {
            card.checklist.push_back(FromStorageChecklistItem(item));
        }

        return card;
    }

    ChecklistItem
    BoardStorageAdapter::FromStorageChecklistItem(const Storage::ChecklistItemData& item)
    {
        ChecklistItem checklistItem;
        checklistItem.id = MakeId(item.id, "item");
        checklistItem.text = item.content;
        checklistItem.isChecked = item.completed;
        return checklistItem;
    }

    // ============================================================
    // ID CONVERSION (Prefixed String IDs)
    // ============================================================

    std::string BoardStorageAdapter::MakeId(int dbId, const std::string& prefix)
    {
        return prefix + "_" + std::to_string(dbId);
    }

    int BoardStorageAdapter::ParseId(const std::string& stringId)
    {
        if(stringId.empty())
            return 0;

        size_t pos = stringId.find('_');
        if(pos == std::string::npos)
        {
            // Try parsing as plain number (for backward compatibility)
            try
            {
                return std::stoi(stringId);
            }
            catch(...)
            {
                return 0;
            }
        }

        try
        {
            return std::stoi(stringId.substr(pos + 1));
        }
        catch(...)
        {
            return 0;
        }
    }

    bool BoardStorageAdapter::HasPrefix(const std::string& stringId, const std::string& prefix)
    {
        std::string expectedPrefix = prefix + "_";
        return stringId.size() > expectedPrefix.size()
               && stringId.compare(0, expectedPrefix.size(), expectedPrefix) == 0;
    }

    // ============================================================
    // HELPER FUNCTIONS
    // ============================================================

    std::vector<std::string> BoardStorageAdapter::GetBadgeNamesForCard(
        int cardId,
        const std::vector<Storage::BadgeData>& allBadges,
        const std::vector<Storage::CardBadgeData>& cardBadgeLinks
    )
    {
        std::vector<std::string> badgeNames;

        // Find all badge IDs for this card
        std::vector<int> badgeIds;
        for(const auto& link : cardBadgeLinks)
        {
            if(link.card_id == cardId)
            {
                badgeIds.push_back(link.badge_id);
            }
        }

        // Look up badge names
        for(int badgeId : badgeIds)
        {
            for(const auto& badge : allBadges)
            {
                if(badge.id == badgeId)
                {
                    badgeNames.push_back(badge.name);
                    break;
                }
            }
        }

        return badgeNames;
    }
}
