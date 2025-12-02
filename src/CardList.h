#pragma once
#include "string"
#include "Card.h"
#include "imgui.h"


class CardList{
    public:
    std::vector<Card> mCards;
    std::string mTitle;

    CardList(std::string title, const std::vector<Card>&& aCards);

    void Render(int list_id, ImVec2 size);

private:
    char mTitleBuffer[256] = "";
    bool mIsEditingTitle = false;
    std::string mUniqueID;
    float mLastContentHeight = 0.0f;

    // Card Popup State
    bool mShowCardPopup = false;
    int mEditingCardIndex = -1; // -1 means creating new card
    char mCardTitleBuffer[256] = "";
    char mCardDescriptionBuffer[1024] = "";
    char mBadgeInputBuffer[64] = "";
    std::vector<std::string> mCardBadges;
    
    // Checklist state
    std::vector<Card::ChecklistItem> mTempChecklist;
    char mChecklistInputBuffer[256] = "";

    void RenderCardPopup();

    void ListHeader();
    bool RenderCardListFooter();
};
