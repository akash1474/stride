#include "pch.h"
#include "CardList.h"
#include "Utils.h"
#include <algorithm>

namespace Stride
{
    // CardList constructors
    CardList::CardList() : id(genUID()) {}

    CardList::CardList(std::string aTitle, std::vector<Card> aCards)
        : id(genUID()), title(std::move(aTitle)), cards(std::move(aCards))
    {}

    // Card operations
    void CardList::AddCard(Card card) { cards.push_back(std::move(card)); }

    void CardList::InsertCard(Card card, size_t index)
    {
        if(index >= cards.size())
        {
            cards.push_back(std::move(card));
        }
        else
        {
            cards.insert(cards.begin() + index, std::move(card));
        }
    }

    void CardList::RemoveCard(const std::string& cardId)
    {
        cards.erase(
            std::remove_if(
                cards.begin(),
                cards.end(),
                [&](const Card& c) { return c.id == cardId; }
            ),
            cards.end()
        );
    }

    void CardList::MoveCard(size_t fromIndex, size_t toIndex)
    {
        if(fromIndex >= cards.size() || toIndex >= cards.size())
            return;

        if(fromIndex < toIndex)
        {
            std::rotate(
                cards.begin() + fromIndex,
                cards.begin() + fromIndex + 1,
                cards.begin() + toIndex + 1
            );
        }
        else
        {
            std::rotate(
                cards.begin() + toIndex,
                cards.begin() + fromIndex,
                cards.begin() + fromIndex + 1
            );
        }
    }

    Card* CardList::FindCard(const std::string& cardId)
    {
        auto it = std::find_if(cards.begin(), cards.end(), [&](const Card& c) {
            return c.id == cardId;
        });
        return it != cards.end() ? &(*it) : nullptr;
    }

    const Card* CardList::FindCard(const std::string& cardId) const
    {
        auto it = std::find_if(cards.begin(), cards.end(), [&](const Card& c) {
            return c.id == cardId;
        });
        return it != cards.end() ? &(*it) : nullptr;
    }

    std::optional<size_t> CardList::GetCardIndex(const std::string& cardId) const
    {
        auto it = std::find_if(cards.begin(), cards.end(), [&](const Card& c) {
            return c.id == cardId;
        });
        if(it != cards.end())
        {
            return std::distance(cards.begin(), it);
        }
        return std::nullopt;
    }
}