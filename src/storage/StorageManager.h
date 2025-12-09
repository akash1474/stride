#pragma once
#include "storage/Storage.h"
#include <utility>
#include <vector>
#include <string>
#include <ctime>

class StorageManager
{
public:
    // =========================================================
    // ✅ STATIC PUBLIC API (USE LIKE: StorageManager::GetBoard(id))
    // =========================================================

    // ---------- BOARDS ----------
    static int CreateBoard(Storage::BoardData b) {
        return Get().CreateBoardInternal(std::move(b));
    }

    static std::vector<Storage::BoardData> GetAllBoards() {
        return Get().GetAllBoardsInternal();
    }

    static Storage::BoardData GetBoard(int id) {
        return Get().GetBoardInternal(id);
    }

    static void UpdateBoard(Storage::BoardData b) {
        Get().UpdateBoardInternal(std::move(b));
    }

    static void DeleteBoard(int id) {
        Get().DeleteBoardInternal(id);
    }

    // ---------- LISTS ----------
    static int CreateList(Storage::ListData l) {
        return Get().CreateListInternal(std::move(l));
    }

    static std::vector<Storage::ListData> GetListsInBoard(int boardId) {
        return Get().GetListsInBoardInternal(boardId);
    }

    static void UpdateList(Storage::ListData l) {
        Get().UpdateListInternal(std::move(l));
    }

    static void ReorderList(int listId, double prevPos, double nextPos) {
        Get().ReorderListInternal(listId, prevPos, nextPos);
    }

    // ---------- CARDS ----------
    static int CreateCard(Storage::CardData c) {
        return Get().CreateCardInternal(std::move(c));
    }

    static Storage::CardData GetCard(int id) {
        return Get().GetCardInternal(id);
    }

    static std::vector<Storage::CardData> GetCardsInList(int listId) {
        return Get().GetCardsInListInternal(listId);
    }

    static void UpdateCard(Storage::CardData c) {
        Get().UpdateCardInternal(std::move(c));
    }

    static void DeleteCard(int id) {
        Get().DeleteCardInternal(id);
    }

    static void MoveCardToList(int cardId, int newListId, double newPos) {
        Get().MoveCardToListInternal(cardId, newListId, newPos);
    }

    static void ReorderCard(int cardId, double prevPos, double nextPos) {
        Get().ReorderCardInternal(cardId, prevPos, nextPos);
    }

    static void ArchiveCard(int cardId) {
        Get().ArchiveCardInternal(cardId);
    }

    static void UnarchiveCard(int cardId) {
        Get().UnarchiveCardInternal(cardId);
    }

    // ---------- BADGES ----------
    static int CreateBadge(const Storage::BadgeData& b) {
        return Get().CreateBadgeInternal(b);
    }

    static std::vector<Storage::BadgeData> GetBadgesInBoard(int boardId) {
        return Get().GetBadgesInBoardInternal(boardId);
    }

    static void AddBadgeToCard(int cardId, int badgeId) {
        Get().AddBadgeToCardInternal(cardId, badgeId);
    }

    static void RemoveBadgeFromCard(int cardId, int badgeId) {
        Get().RemoveBadgeFromCardInternal(cardId, badgeId);
    }

    static std::vector<Storage::BadgeData> GetBadgesForCard(int cardId) {
        return Get().GetBadgesForCardInternal(cardId);
    }

    // ---------- CHECKLIST ITEMS (FLATTENED) ----------
    static int CreateChecklistItem(const Storage::ChecklistItemData& i) {
        return Get().CreateChecklistItemInternal(i);
    }

    static std::vector<Storage::ChecklistItemData> GetChecklistItemsForCard(int cardId) {
        return Get().GetChecklistItemsForCardInternal(cardId);
    }

    static void UpdateChecklistItem(const Storage::ChecklistItemData& i) {
        Get().UpdateChecklistItemInternal(i);
    }

    static void DeleteChecklistItem(int id) {
        Get().DeleteChecklistItemInternal(id);
    }

    // ---------- COMMENTS ----------
    static int AddComment(Storage::CommentData c) {
        return Get().AddCommentInternal(std::move(c));
    }

    static std::vector<Storage::CommentData> GetCommentsForCard(int cardId) {
        return Get().GetCommentsForCardInternal(cardId);
    }

    // ---------- SEARCH ----------
    static std::vector<Storage::CardData> SearchCards(int boardId, const std::string& text) {
        return Get().SearchCardsInternal(boardId, text);
    }

private:
    // =========================================================
    // ✅ SINGLETON CORE
    // =========================================================

    static StorageManager& Get()
    {
        static StorageManager instance("stride.db");
        return instance;
    }

    StorageManager(const StorageManager&) = delete;
    StorageManager& operator=(const StorageManager&) = delete;

    StorageManager(const std::string& path)
        : mStorage(Storage::SetupStorageDatabaseModels(path))
    {
        mStorage.sync_schema();
    }

private:
    decltype(Storage::SetupStorageDatabaseModels("")) mStorage;

    double Mid(double a, double b) { return (a + b) * 0.5; }
    int64_t Now() { return static_cast<int64_t>(time(nullptr)); }

    // =========================================================
    // ✅ INTERNAL IMPLEMENTATION
    // =========================================================

    // ----- BOARDS -----
    int CreateBoardInternal(Storage::BoardData b)
    {
        b.created_at = Now();
        b.updated_at = Now();
        return mStorage.insert(b);
    }

    std::vector<Storage::BoardData> GetAllBoardsInternal()
    {
        using namespace sqlite_orm;
        return mStorage.get_all<Storage::BoardData>(order_by(&Storage::BoardData::id));
    }

    Storage::BoardData GetBoardInternal(int id)
    {
        return mStorage.get<Storage::BoardData>(id);
    }

    void UpdateBoardInternal(Storage::BoardData b)
    {
        b.updated_at = Now();
        mStorage.update(b);
    }

    void DeleteBoardInternal(int id)
    {
        mStorage.remove<Storage::BoardData>(id);
    }

    // ----- LISTS -----
    int CreateListInternal(Storage::ListData l)
    {
        l.created_at = Now();
        l.updated_at = Now();
        return mStorage.insert(l);
    }

    std::vector<Storage::ListData> GetListsInBoardInternal(int boardId)
    {
        using namespace sqlite_orm;
        return mStorage.get_all<Storage::ListData>(
            where(c(&Storage::ListData::board_id) == boardId),
            order_by(&Storage::ListData::position)
        );
    }

    void UpdateListInternal(Storage::ListData l)
    {
        l.updated_at = Now();
        mStorage.update(l);
    }

    void ReorderListInternal(int listId, double prevPos, double nextPos)
    {
        auto l = mStorage.get<Storage::ListData>(listId);
        l.position = Mid(prevPos, nextPos);
        l.updated_at = Now();
        mStorage.update(l);
    }

    // ----- CARDS -----
    int CreateCardInternal(Storage::CardData c)
    {
        c.created_at = Now();
        c.updated_at = Now();
        return mStorage.insert(c);
    }

    Storage::CardData GetCardInternal(int id)
    {
        return mStorage.get<Storage::CardData>(id);
    }

    std::vector<Storage::CardData> GetCardsInListInternal(int listId)
    {
        using namespace sqlite_orm;
        return mStorage.get_all<Storage::CardData>(
            where(c(&Storage::CardData::list_id) == listId &&
                  c(&Storage::CardData::archived) == false),
            order_by(&Storage::CardData::position)
        );
    }

    void UpdateCardInternal(Storage::CardData c)
    {
        c.updated_at = Now();
        mStorage.update(c);
    }

    void DeleteCardInternal(int id)
    {
        mStorage.remove<Storage::CardData>(id);
    }

    void MoveCardToListInternal(int cardId, int newListId, double newPos)
    {
        auto c = mStorage.get<Storage::CardData>(cardId);
        c.list_id = newListId;
        c.position = newPos;
        c.updated_at = Now();
        mStorage.update(c);
    }

    void ReorderCardInternal(int cardId, double prevPos, double nextPos)
    {
        auto c = mStorage.get<Storage::CardData>(cardId);
        c.position = Mid(prevPos, nextPos);
        c.updated_at = Now();
        mStorage.update(c);
    }

    void ArchiveCardInternal(int cardId)
    {
        auto c = mStorage.get<Storage::CardData>(cardId);
        c.archived = true;
        mStorage.update(c);
    }

    void UnarchiveCardInternal(int cardId)
    {
        auto c = mStorage.get<Storage::CardData>(cardId);
        c.archived = false;
        mStorage.update(c);
    }

    // ----- BADGES -----
    int CreateBadgeInternal(const Storage::BadgeData& b)
    {
        return mStorage.insert(b);
    }

    std::vector<Storage::BadgeData> GetBadgesInBoardInternal(int boardId)
    {
        using namespace sqlite_orm;
        return mStorage.get_all<Storage::BadgeData>(
            where(c(&Storage::BadgeData::board_id) == boardId)
        );
    }

    void AddBadgeToCardInternal(int cardId, int badgeId)
    {
        mStorage.insert(Storage::CardBadgeData{ cardId, badgeId });
    }

    void RemoveBadgeFromCardInternal(int cardId, int badgeId)
    {
        mStorage.remove<Storage::CardBadgeData>(std::make_tuple(cardId, badgeId));
    }

    std::vector<Storage::BadgeData> GetBadgesForCardInternal(int cardId)
    {
        using namespace sqlite_orm;
        return mStorage.select(
            object<Storage::BadgeData>(),
            inner_join<Storage::CardBadgeData>(
                on(c(&Storage::CardBadgeData::badge_id) == &Storage::BadgeData::id)
            ),
            where(c(&Storage::CardBadgeData::card_id) == cardId)
        );
    }

    // ----- CHECKLIST ITEMS (FLATTENED) -----
    int CreateChecklistItemInternal(const Storage::ChecklistItemData& i)
    {
        return mStorage.insert(i);
    }

    std::vector<Storage::ChecklistItemData> GetChecklistItemsForCardInternal(int cardId)
    {
        using namespace sqlite_orm;
        return mStorage.get_all<Storage::ChecklistItemData>(
            where(c(&Storage::ChecklistItemData::card_id) == cardId),
            order_by(&Storage::ChecklistItemData::position)
        );
    }

    void UpdateChecklistItemInternal(const Storage::ChecklistItemData& i)
    {
        mStorage.update(i);
    }

    void DeleteChecklistItemInternal(int id)
    {
        mStorage.remove<Storage::ChecklistItemData>(id);
    }

    // ----- COMMENTS -----
    int AddCommentInternal(Storage::CommentData c)
    {
        c.created_at = Now();
        return mStorage.insert(c);
    }

    std::vector<Storage::CommentData> GetCommentsForCardInternal(int cardId)
    {
        using namespace sqlite_orm;
        return mStorage.get_all<Storage::CommentData>(
            where(c(&Storage::CommentData::card_id) == cardId),
            order_by(&Storage::CommentData::created_at)
        );
    }

    // ----- SEARCH -----
    std::vector<Storage::CardData> SearchCardsInternal(int boardId, const std::string& text)
    {
        using namespace sqlite_orm;
        std::string q = "%" + text + "%";

        return mStorage.get_all<Storage::CardData>(
            where(
                c(&Storage::CardData::board_id) == boardId &&
                (like(&Storage::CardData::title, q) ||
                 like(&Storage::CardData::description, q))
            )
        );
    }
};
