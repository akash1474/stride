#pragma once

#include <string>
#include <sqlite_orm.h>

namespace Storage
{
    struct BoardData
    {
        int id;
        std::string name;
        std::string description;
        std::string background_image;
        int64_t created_at;
        int64_t updated_at;
    };

    struct ListData
    {
        int id;
        int board_id;
        std::string name;
        int position;
        int64_t created_at;
        int64_t updated_at;
    };

    struct CardData
    {
        int id;
        int list_id;
        int board_id;
        std::string title;
        std::string description;
        int position;
        int64_t created_at;
        int64_t updated_at;
        int64_t due_date;
        bool completed;
        std::string cover_color;
        std::string cover_image;
        bool archived;
    };

    struct BadgeData
    {
        int id;
        int board_id;
        std::string name;
        std::string color;
    };

    struct CardBadgeData
    {
        int card_id;
        int badge_id;
    };

    struct ChecklistItemData
    {
        int id;
        int card_id; // Flattened: Directly linked to card
        std::string content;
        int position;
        bool completed;
    };

    struct CommentData
    {
        int id;
        int card_id;
        std::string author;
        std::string content;
        int64_t created_at;
    };


    // STORAGE FACTORY
    inline auto SetupStorageDatabaseModels(const std::string& path)
    {
        using namespace sqlite_orm;

        return make_storage(
            path,

            // BOARDS
            make_table(
                "boards",
                make_column("id", &BoardData::id, primary_key().autoincrement()),
                make_column("name", &BoardData::name),
                make_column("created_at", &BoardData::created_at),
                make_column("updated_at", &BoardData::updated_at)
            ),

            // LISTS
            make_table(
                "lists",
                make_column("id", &ListData::id, primary_key().autoincrement()),
                make_column("board_id", &ListData::board_id),
                make_column("name", &ListData::name),
                make_column("position", &ListData::position),
                make_column("created_at", &ListData::created_at),
                make_column("updated_at", &ListData::updated_at),

                foreign_key(&ListData::board_id).references(&BoardData::id).on_delete.cascade()
            ),

            // CARDS
            make_table(
                "cards",
                make_column("id", &CardData::id, primary_key().autoincrement()),
                make_column("list_id", &CardData::list_id),
                make_column("board_id", &CardData::board_id),
                make_column("title", &CardData::title),
                make_column("description", &CardData::description),
                make_column("position", &CardData::position),
                make_column("created_at", &CardData::created_at),
                make_column("updated_at", &CardData::updated_at),
                make_column("due_date", &CardData::due_date),
                make_column("completed", &CardData::completed),
                make_column("cover_color", &CardData::cover_color),
                make_column("cover_image", &CardData::cover_image),
                make_column("archived", &CardData::archived),

                foreign_key(&CardData::list_id).references(&ListData::id).on_delete.cascade(),

                foreign_key(&CardData::board_id).references(&BoardData::id).on_delete.cascade()
            ),

            // BADGES (was Labels)
            make_table(
                "badges",
                make_column("id", &BadgeData::id, primary_key().autoincrement()),
                make_column("board_id", &BadgeData::board_id),
                make_column("name", &BadgeData::name),
                make_column("color", &BadgeData::color),

                foreign_key(&BadgeData::board_id).references(&BoardData::id).on_delete.cascade()
            ),

            // CARD ↔ BADGE (MANY TO MANY)
            make_table(
                "card_badges",
                make_column("card_id", &CardBadgeData::card_id),
                make_column("badge_id", &CardBadgeData::badge_id),

                primary_key(&CardBadgeData::card_id, &CardBadgeData::badge_id),

                foreign_key(&CardBadgeData::card_id).references(&CardData::id).on_delete.cascade(),

                foreign_key(&CardBadgeData::badge_id).references(&BadgeData::id).on_delete.cascade()
            ),

            // CHECKLIST ITEMS (Flattened)
            make_table(
                "checklist_items",
                make_column("id", &ChecklistItemData::id, primary_key().autoincrement()),

                make_column("card_id", &ChecklistItemData::card_id),
                make_column("content", &ChecklistItemData::content),
                make_column("position", &ChecklistItemData::position),
                make_column("completed", &ChecklistItemData::completed),

                foreign_key(&ChecklistItemData::card_id)
                    .references(&CardData::id)
                    .on_delete.cascade()
            ),

            // COMMENTS
            make_table(
                "comments",
                make_column("id", &CommentData::id, primary_key().autoincrement()),

                make_column("card_id", &CommentData::card_id),
                make_column("author", &CommentData::author),
                make_column("content", &CommentData::content),
                make_column("created_at", &CommentData::created_at),

                foreign_key(&CommentData::card_id).references(&CardData::id).on_delete.cascade()
            )
        );
    }

}
