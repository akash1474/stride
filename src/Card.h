#pragma once
#pragma once
#include <string>
#include <vector>


class Card{
    public:
    struct ChecklistItem {
        std::string text;
        bool isChecked = false;
    };

    std::string mTitle;
    std::string mDescription;
    std::vector<std::string> mBadges;
    std::vector<ChecklistItem> mChecklist;
    
    Card()=default;
    Card(std::string aTitle, std::string aDescription, std::vector<std::string> aBadges)
        : mTitle(std::move(aTitle)), mDescription(std::move(aDescription)), mBadges(std::move(aBadges)) {}

    static void Render(const char* unique_id, const char* title, const std::vector<std::string>& badges, bool hasDescription = false, int checklistCompleted = 0, int checklistTotal = 0, bool isDragging = false);
};
