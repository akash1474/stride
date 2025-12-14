#pragma once
#include <string>
#include <vector>

namespace Stride
{
    struct ChecklistItem
    {
        std::string id;
        std::string text;
        bool isChecked = false;

        ChecklistItem() = default;
        ChecklistItem(std::string aText, bool checked = false);
    };

    struct Card
    {
        // Identity
        std::string id;

        // Content
        std::string title;
        std::string description;

        int position;
        std::string coverImage;
        time_t dueDate;
        bool isCompleted;

        // Metadata
        std::vector<std::string> badges;
        std::vector<ChecklistItem> checklist;

        // Constructors
        Card();
        Card(
            std::string aTitle,
            std::string aDescription = "",
            std::vector<std::string> aBadges = {}
        );

        // Helper methods
        bool HasDescription() const { return !description.empty(); }
        bool HasChecklist() const { return !checklist.empty(); }
        bool HasBadges() const { return !badges.empty(); }
        int GetChecklistCompleted() const;
        int GetChecklistTotal() const { return static_cast<int>(checklist.size()); }
        float GetChecklistProgress() const;

        // Checklist operations
        void AddChecklistItem(const std::string& text);
        void RemoveChecklistItem(const std::string& itemId);
        void ToggleChecklistItem(const std::string& itemId);
        ChecklistItem* FindChecklistItem(const std::string& itemId);

        // Badge operations
        void AddBadge(const std::string& badge);
        void RemoveBadge(const std::string& badge);
        bool HasBadge(const std::string& badge) const;
    };
}
