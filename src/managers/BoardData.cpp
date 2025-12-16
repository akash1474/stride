#include "pch.h"
#include "BoardData.h"
#include "Utils.h"
#include <algorithm>
#include <chrono>

namespace Stride
{
    static int64_t GetCurrentTimestamp()
    {
        using namespace std::chrono;
        return duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
        ).count();
    }
    
    BoardData::BoardData()
        : id(genUID())
        , createdAt(GetCurrentTimestamp())
        , updatedAt(createdAt)
    {}
    
    BoardData::BoardData(std::string title)
        : id(genUID())
        , title(std::move(title))
        , createdAt(GetCurrentTimestamp())
        , updatedAt(createdAt)
    {}
    
    BoardData::BoardData(std::string id, std::string title)
        : id(std::move(id))
        , title(std::move(title))
        , createdAt(GetCurrentTimestamp())
        , updatedAt(createdAt)
    {}
    
    CardList& BoardData::AddList(const std::string& listTitle)
    {
        lists.emplace_back(listTitle);
        updatedAt = GetCurrentTimestamp();
        return lists.back();
    }
    
    CardList& BoardData::InsertList(const std::string& listTitle, size_t index)
    {
        if (index >= lists.size())
        {
            return AddList(listTitle);
        }
        
        auto it = lists.emplace(lists.begin() + index, listTitle);
        updatedAt = GetCurrentTimestamp();
        return *it;
    }
    
    void BoardData::RemoveList(const std::string& listId)
    {
        lists.erase(
            std::remove_if(lists.begin(), lists.end(),
                [&](const CardList& list) { return list.id == listId; }),
            lists.end()
        );
        updatedAt = GetCurrentTimestamp();
    }
    
    void BoardData::MoveList(size_t fromIndex, size_t toIndex)
    {
        if (fromIndex >= lists.size() || toIndex >= lists.size())
            return;
        
        if (fromIndex < toIndex)
        {
            std::rotate(
                lists.begin() + fromIndex,
                lists.begin() + fromIndex + 1,
                lists.begin() + toIndex + 1
            );
        }
        else
        {
            std::rotate(
                lists.begin() + toIndex,
                lists.begin() + fromIndex,
                lists.begin() + fromIndex + 1
            );
        }
        updatedAt = GetCurrentTimestamp();
    }
    
    CardList* BoardData::FindList(const std::string& listId)
    {
        auto it = std::find_if(lists.begin(), lists.end(),
            [&](const CardList& list) { return list.id == listId; });
        return it != lists.end() ? &(*it) : nullptr;
    }
    
    const CardList* BoardData::FindList(const std::string& listId) const
    {
        auto it = std::find_if(lists.begin(), lists.end(),
            [&](const CardList& list) { return list.id == listId; });
        return it != lists.end() ? &(*it) : nullptr;
    }
    
    std::optional<size_t> BoardData::GetListIndex(const std::string& listId) const
    {
        auto it = std::find_if(lists.begin(), lists.end(),
            [&](const CardList& list) { return list.id == listId; });
        if (it != lists.end())
        {
            return std::distance(lists.begin(), it);
        }
        return std::nullopt;
    }
    
    Card* BoardData::FindCard(const std::string& cardId)
    {
        for (auto& list : lists)
        {
            if (auto* card = list.FindCard(cardId))
            {
                return card;
            }
        }
        return nullptr;
    }
    
    const Card* BoardData::FindCard(const std::string& cardId) const
    {
        for (const auto& list : lists)
        {
            if (auto* card = const_cast<CardList&>(list).FindCard(cardId))
            {
                return card;
            }
        }
        return nullptr;
    }
    
    std::pair<CardList*, Card*> BoardData::FindCardWithList(const std::string& cardId)
    {
        for (auto& list : lists)
        {
            if (auto* card = list.FindCard(cardId))
            {
                return {&list, card};
            }
        }
        return {nullptr, nullptr};
    }
    
    void BoardData::MoveCard(
        const std::string& cardId,
        const std::string& targetListId,
        size_t targetIndex)
    {
        auto [sourceList, card] = FindCardWithList(cardId);
        if (!sourceList || !card) return;
        
        CardList* targetList = FindList(targetListId);
        if (!targetList) return;
        
        // Copy card data
        Card movedCard = std::move(*card);
        
        // Remove from source
        sourceList->RemoveCard(cardId);
        
        // Insert into target
        targetList->InsertCard(std::move(movedCard), targetIndex);
        
        updatedAt = GetCurrentTimestamp();
    }
    
    size_t BoardData::GetTotalCardCount() const
    {
        size_t count = 0;
        for (const auto& list : lists)
        {
            count += list.CardCount();
        }
        return count;
    }

    void BoardData::UpdateListPositions()
    {
        for (size_t i = 0; i < lists.size(); ++i)
        {
            lists[i].position = static_cast<int>(i);
        }
    }
}
