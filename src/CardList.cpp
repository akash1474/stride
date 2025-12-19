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

    void CardList::InsertCard(Card card, size_t insert_index)
    {
        if(insert_index >= cards.size()) {
            cards.push_back(std::move(card));
        }
        else
        {
            cards.insert(cards.begin() + insert_index, std::move(card));
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
        if(fromIndex >= cards.size())
            return;

        // toIndex must be within [0, cards.size()-1] for valid move position
        if(toIndex >= cards.size())
            return;

        if(fromIndex == toIndex)
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

    void CardList::UpdateCardPositions()
    {
        for(size_t i = 0; i < cards.size(); ++i)
        {
            cards[i].position = static_cast<int>(i);
        }
    }
}