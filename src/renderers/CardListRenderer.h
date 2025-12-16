#pragma once
#include "CardList.h"
#include "Card.h"
#include "imgui.h"
#include <string>
#include <vector>

namespace Stride
{
    struct CardListUIState
    {
        std::string uniqueId;

        // Title editing
        bool isEditingTitle = false;
        char titleBuffer[256] = "";

        // Scroll state
        float scrollY = 0.0f;
        float lastContentHeight = 0.0f;

        CardListUIState();
        void Reset();
    };

    struct CardEditorState
    {
        bool isOpen = false;
        bool isEditing = false;
        std::string editingCardId;

        // Edit buffers
        char titleBuffer[256] = "";
        char descriptionBuffer[1024] = "";
        char badgeInputBuffer[64] = "";
        char checklistInputBuffer[256] = "";
        std::vector<std::string> badges;
        std::vector<ChecklistItem> checklist;

        void OpenForCreate();
        void OpenForEdit(const Card& card);
        void Close();
        void Reset();
    };

    struct CardListStyle
    {
        float width = 280.0f;
        float headerHeight = 40.0f;
        float footerHeight = 60.0f;
        float cardSpacing = 8.0f;
        float cornerRadius = 10.0f;

        ImU32 backgroundColor = IM_COL32(16, 18, 4, 255);
        ImU32 headerTextColor = IM_COL32(191, 193, 196, 255);
    };

    class CardListRenderer
    {
      public:
        static void SetStyle(const CardListStyle& style);
        static const CardListStyle& GetStyle();

        // Set available badges for card editor
        static void SetAvailableBadges(const std::vector<std::string>& badges);
        static const std::vector<std::string>& GetAvailableBadges();

        // Render complete list
        static void Render(
            CardList& data,
            CardListUIState& uiState,
            CardEditorState& editorState,
            int listIndex,
            ImVec2 size
        );

      private:
        static CardListStyle sStyle;
        static std::vector<std::string> sAvailableBadges;

        static void RenderHeader(CardList& data, CardListUIState& uiState, int listIndex);
        static void RenderCards(CardList& data, CardEditorState& editorState, int listIndex);
        static void RenderFooter(CardList& data, CardEditorState& editorState);
        static void RenderCardPopup(CardList& data, CardEditorState& editorState);
        static void ResetCardListState(CardList& data, CardEditorState& editorState);
    };
}
