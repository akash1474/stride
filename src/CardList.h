#pragma once
#include "string"
#include "Card.h"
#include "imgui.h"
#include <optional>

namespace Stride
{
    struct CardList
    {
        // Identity
        std::string id;

        // Data
        std::string title;
        std::vector<Card> cards;
        int position;

        // Constructors
        CardList();
        CardList(std::string aTitle, std::vector<Card> aCards = {});

        // Card operations
        void AddCard(Card card);
        void InsertCard(Card card, size_t index);
        void RemoveCard(const std::string& cardId);
        void MoveCard(size_t fromIndex, size_t toIndex);
        void UpdateCardPositions(); // Update position field of all cards to match array index

        // Query
        Card* FindCard(const std::string& cardId);
        const Card* FindCard(const std::string& cardId) const;
        std::optional<size_t> GetCardIndex(const std::string& cardId) const;

        bool IsEmpty() const { return cards.empty(); }
        size_t CardCount() const { return cards.size(); }
    };
}
