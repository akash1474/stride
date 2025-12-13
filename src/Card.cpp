#include "pch.h"
#include "Card.h"
#include "Utils.h"
#include <algorithm>

namespace Stride
{
    // ChecklistItem constructor
    ChecklistItem::ChecklistItem(std::string aText, bool checked)
        : id(genUID()), text(std::move(aText)), isChecked(checked)
    {}

    // Card constructors
    Card::Card() : id(genUID()) {}

    Card::Card(std::string aTitle, std::string aDescription, std::vector<std::string> aBadges)
        : id(genUID()),
          title(std::move(aTitle)),
          description(std::move(aDescription)),
          badges(std::move(aBadges))
    {}

    // Helper method implementations
    int Card::GetChecklistCompleted() const
    {
        return static_cast<int>(
            std::count_if(checklist.begin(), checklist.end(), [](const ChecklistItem& item) {
                return item.isChecked;
            })
        );
    }

    float Card::GetChecklistProgress() const
    {
        if(checklist.empty())
            return 0.0f;
        return static_cast<float>(GetChecklistCompleted()) / GetChecklistTotal();
    }

    // Checklist operations
    void Card::AddChecklistItem(const std::string& text) { checklist.emplace_back(text, false); }

    void Card::RemoveChecklistItem(const std::string& itemId)
    {
        checklist.erase(
            std::remove_if(
                checklist.begin(),
                checklist.end(),
                [&](const ChecklistItem& item) { return item.id == itemId; }
            ),
            checklist.end()
        );
    }

    void Card::ToggleChecklistItem(const std::string& itemId)
    {
        if(auto* item = FindChecklistItem(itemId))
        {
            item->isChecked = !item->isChecked;
        }
    }

    ChecklistItem* Card::FindChecklistItem(const std::string& itemId)
    {
        auto it = std::find_if(checklist.begin(), checklist.end(), [&](const ChecklistItem& item) {
            return item.id == itemId;
        });
        return it != checklist.end() ? &(*it) : nullptr;
    }

    // Badge operations
    void Card::AddBadge(const std::string& badge)
    {
        if(!HasBadge(badge))
        {
            badges.push_back(badge);
        }
    }

    void Card::RemoveBadge(const std::string& badge)
    {
        badges.erase(std::remove(badges.begin(), badges.end(), badge), badges.end());
    }

    bool Card::HasBadge(const std::string& badge) const
    {
        return std::find(badges.begin(), badges.end(), badge) != badges.end();
    }
}
