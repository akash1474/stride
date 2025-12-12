#include "pch.h"
#include "CardList.h"
#include "managers/DragDropManager.h"
#include "managers/FontManager.h"
#include "managers/DragDropTypes.h"
#include "FontAwesome6.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Utils.h"
#include <string>
#include <algorithm>

#include "BadgeColors.h"
#include "Components.h"



CardList::CardList(std::string title, const std::vector<Card>&& aCards)
    : mTitle(title), mCards(aCards)
{
    strcpy_s(mTitleBuffer, mTitle.c_str());
    mUniqueID = genUID();
}

void CardList::ListHeader()
{
    const float dpiScale = FontManager::GetDpiScale();
    const float x_center = (ImGui::GetContentRegionAvail().x - 256.0f * dpiScale) * 0.5f;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x_center);
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if(window->SkipItems)
        return;

    const float height = 40.0f * dpiScale;
    const float width = 256.0f * dpiScale;
    const float padding_x = 10.0f * dpiScale;
    // --- Buttons on right ---
    float button_size = 30.0f * dpiScale;
    float spacing = 6.0f * dpiScale;

    const ImGuiStyle& style = ImGui::GetStyle();

    const ImGuiID id = window->GetID(mUniqueID.c_str());

    ImVec2 size(width, height);

    ImRect bb(
        window->DC.CursorPos,
        ImVec2(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y + size.y)
    );
    ImGui::ItemSize(size);
    if(!ImGui::ItemAdd(bb, id))
        return;

    // --- Background ---
    // Background is handled by the parent CardList child window with ChildRounding.
    // We don't need to draw it manually here.

    // --- Layout ---
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(191, 193, 196, 255));
    ImVec2 text_pos = { bb.Min.x + padding_x,
                        bb.Min.y
                            + (height - ImGui::GetTextLineHeight()
                               - (mIsEditingTitle ? (style.FramePadding.y * 2.0f) : 0.0f))
                                  * 0.5f };
    FontManager::Push(FontFamily::SemiBold, FontSize::Regular);
    if(mIsEditingTitle)
    {
        ImGui::SetCursorScreenPos(text_pos);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(255, 255, 255, 0));
        ImGui::SetNextItemWidth(width - padding_x - (button_size * 2.0f) - spacing - 5.0f);
        ImGui::InputText(
            (std::string("##titleEditor") + mUniqueID).c_str(),
            mTitleBuffer,
            IM_ARRAYSIZE(mTitleBuffer)
        );
        ImGui::PopStyleColor();
        if(ImGui::IsItemFocused())
        {
            ImDrawList* fg = ImGui::GetForegroundDrawList();
            fg->AddRect(
                { ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y - 2.0f },
                { ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y + 2.0f },
                IM_COL32(255, 255, 255, 255),
                2.0f
            );
        }
    }
    else
    {
        ImGui::RenderText(text_pos, mTitle.c_str(), nullptr);
        if(ImGui::IsItemClicked())
            mIsEditingTitle = true;

        if(ImGui::IsItemHovered())
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    FontManager::Pop();



    ImVec2 btn2_pos = ImVec2(bb.Max.x - button_size, bb.Min.y + (height - button_size) * 0.5f);
    ImVec2 btn1_pos = ImVec2(btn2_pos.x - button_size - spacing, btn2_pos.y);

    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(16, 18, 4, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 50));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 255, 255, 100));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    ImGui::SetCursorScreenPos(btn1_pos);
    FontManager::Push(FontFamily::Regular, FontSize::Smaller);
    if(ImGui::Button(ICON_FA_DOWN_LEFT_AND_UP_RIGHT_TO_CENTER, ImVec2(button_size, button_size)))
    {
        mIsEditingTitle = false;
        // TODO: action for expand/contract
    }
    FontManager::Pop();
    if(ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    FontManager::Push(FontFamily::Regular, FontSize::Small);
    ImGui::SetCursorScreenPos(btn2_pos);
    if(ImGui::Button(ICON_FA_ELLIPSIS, ImVec2(button_size, button_size)))
    {
        // TODO: action for menu
    }
    FontManager::Pop();
    if(ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar();
}

void CardList::RenderCardPopup()
{
    const float dpiScale = FontManager::GetDpiScale();

    // Limit height to 85% of screen to allow scrolling if content is too large
    float maxHeight = ImGui::GetMainViewport()->WorkSize.y * 0.9f;
    ImGui::SetNextWindowSizeConstraints(
        ImVec2(500 * dpiScale, 0),
        ImVec2(500 * dpiScale, maxHeight)
    );

    // Custom style for popup
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20 * dpiScale, 20 * dpiScale));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(28, 30, 34, 255));
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(50, 50, 55, 255));

    std::string popupId = std::string("Card Popup##") + mUniqueID;
    if(ImGui::BeginPopupModal(
           popupId.c_str(),
           NULL,
           ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
       ))
    {
        FontManager::Push(FontFamily::Bold, FontSize::Large);
        ImGui::Text(mEditingCardIndex == -1 ? "Create New Card" : "Edit Card");
        FontManager::Pop();

        ImGui::Spacing();
        ImGui::Spacing();

        FontManager::Push(FontFamily::Regular, FontSize::Regular);
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Card Title");

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(16, 18, 20, 255));
        ImGui::SetNextItemWidth(-1);

        // Focus input on first appear
        if(ImGui::IsWindowAppearing())
            ImGui::SetKeyboardFocusHere();

        bool enterPressed = ImGui::InputText(
            "##CardTitle",
            mCardTitleBuffer,
            sizeof(mCardTitleBuffer),
            ImGuiInputTextFlags_EnterReturnsTrue
        );
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        FontManager::Pop();

        ImGui::Spacing();

        // --- Description Section ---
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Description");

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(16, 18, 20, 255));
        ImGui::SetNextItemWidth(-1);

        ImGui::InputTextMultiline(
            "##CardDescription",
            mCardDescriptionBuffer,
            sizeof(mCardDescriptionBuffer),
            ImVec2(-1, 150 * dpiScale),
            ImGuiInputTextFlags_None | ImGuiInputTextFlags_WordWrap
        );
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        ImGui::Spacing();
        ImGui::Spacing();

        // --- Badge Selection ---
        FontManager::Push(FontFamily::Regular, FontSize::Regular);
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Badges");

        // Selected Badges
        if(!mCardBadges.empty())
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));
            bool isFirst = true;
            for(auto it = mCardBadges.begin(); it != mCardBadges.end();)
            {
                // Calculate estimated width
                ImVec2 text_size = ImGui::CalcTextSize(it->c_str());
                float padding_x = 8.0f * dpiScale;
                float icon_spacing = 6.0f * dpiScale;
                ImVec2 icon_size = ImGui::CalcTextSize(ICON_FA_XMARK);
                float width = text_size.x + (padding_x * 2.0f) + icon_size.x + icon_spacing;

                if(!isFirst)
                {
                    float last_x2 = ImGui::GetItemRectMax().x;
                    float next_x2 = last_x2 + ImGui::GetStyle().ItemSpacing.x + width;
                    float window_max_x
                        = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

                    if(next_x2 < window_max_x)
                        ImGui::SameLine();
                }

                BadgeColors::BadgeStyle style = BadgeColors::GetBadgeStyleForText(*it);
                if(Components::Badge(it->c_str(), style, true))
                {
                    it = mCardBadges.erase(it);
                    // If we erased, we still rendered this frame, so next item is not first
                    isFirst = false;
                }
                else
                {
                    ++it;
                    isFirst = false;
                }
            }
            ImGui::NewLine();
            ImGui::PopStyleVar();
        }

        // Available Badges
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Add Badge:");
        const char* availableBadges[] = { "Bug", "Feature", "Urgent",        "Design",
                                          "Dev", "Test",    "High Priority", "Low Priority" };

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));
        bool isFirst = true;
        for(const char* badge : availableBadges)
        {
            // Check if already selected
            bool selected = false;
            for(const auto& b : mCardBadges)
                if(b == badge)
                    selected = true;
            if(selected)
                continue;

            // Calculate estimated width (removable=false)
            ImVec2 text_size = ImGui::CalcTextSize(badge);
            float padding_x = 8.0f * dpiScale;
            float width = text_size.x + (padding_x * 2.0f);

            if(!isFirst)
            {
                float last_x2 = ImGui::GetItemRectMax().x;
                float next_x2 = last_x2 + ImGui::GetStyle().ItemSpacing.x + width;
                float window_max_x = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

                if(next_x2 < window_max_x)
                    ImGui::SameLine();
            }

            BadgeColors::BadgeStyle style = BadgeColors::GetBadgeStyleForText(badge);
            if(Components::Badge(badge, style, false))
            {
                mCardBadges.push_back(badge);
            }
            isFirst = false;
        }
        FontManager::Pop();
        ImGui::NewLine();
        ImGui::PopStyleVar();


        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // --- Checklist Section ---
        // --- Checklist Section ---
        ImGui::Text("Checklist");

        int completed = 0;
        for(const auto& item : mTempChecklist)
            if(item.isChecked)
                completed++;
        float progress
            = mTempChecklist.empty() ? 0.0f : (float)completed / (float)mTempChecklist.size();

        ImGui::SameLine();
        char progressOverlay[32];
        sprintf_s(progressOverlay, "%d/%d", completed, (int)mTempChecklist.size());

        float availWidth = ImGui::GetContentRegionAvail().x;
        float textWidth = ImGui::CalcTextSize(progressOverlay).x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availWidth - textWidth);
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), progressOverlay);

        ImGui::Spacing();

        Components::ProgressBar(progress, ImGui::GetContentRegionAvail().x, 4.0f * dpiScale);
        ImGui::Spacing();

        // Checklist Items
        ImGui::PushStyleVar(
            ImGuiStyleVar_ItemSpacing,
            ImVec2(0, 8 * dpiScale)
        ); // Spacing between items
        for(int i = 0; i < (int)mTempChecklist.size(); ++i)
        {
            ImGui::PushID(i);
            auto& item = mTempChecklist[i];
            bool deleted = false;

            Components::ChecklistItem(
                item.text.c_str(),
                &item.isChecked,
                &deleted,
                ImGui::GetContentRegionAvail().x
            );

            if(deleted)
            {
                mTempChecklist.erase(mTempChecklist.begin() + i);
                i--;
            }

            ImGui::PopID();
        }
        ImGui::PopStyleVar();

        // Add New Item
        ImGui::Spacing();

        if(Components::InputWithIcon(
               "##newItem",
               ICON_FA_PLUS,
               "Add an item...",
               mChecklistInputBuffer,
               sizeof(mChecklistInputBuffer),
               ImGui::GetContentRegionAvail().x
           ))
        {
            if(strlen(mChecklistInputBuffer) > 0)
            {
                mTempChecklist.push_back({ mChecklistInputBuffer, false });
                memset(mChecklistInputBuffer, 0, sizeof(mChecklistInputBuffer));
                ImGui::SetKeyboardFocusHere(-1); // Keep focus
            }
            else
            {
                ImGui::SetKeyboardFocusHere(1); // Focus the input next to it
            }
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        // Buttons
        float width = ImGui::GetContentRegionAvail().x;
        float btnWidth = (width - 10.0f * dpiScale) * 0.5f;

        FontManager::Push(FontFamily::SemiBold, FontSize::Regular);

        // Cancel Button
        if(Components::StyledButton(
               "Cancel",
               ImVec2(btnWidth, 35 * dpiScale),
               IM_COL32(45, 45, 50, 255),
               IM_COL32(60, 60, 65, 255)
           )
           || ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        // Save/Create Button
        const char* btnText = mEditingCardIndex == -1 ? "Create" : "Save";
        if(Components::StyledButton(
               btnText,
               ImVec2(btnWidth, 35 * dpiScale),
               IM_COL32(59, 130, 246, 255),
               IM_COL32(37, 99, 235, 255)
           )
           || enterPressed)
        {
            if(strlen(mCardTitleBuffer) > 0)
            {
                if(mEditingCardIndex == -1)
                {
                    Card newCard(mCardTitleBuffer, mCardDescriptionBuffer, mCardBadges);
                    newCard.mChecklist = mTempChecklist;
                    mCards.push_back(std::move(newCard));
                }
                else if(mEditingCardIndex >= 0 && mEditingCardIndex < (int)mCards.size())
                {
                    mCards[mEditingCardIndex].mTitle = mCardTitleBuffer;
                    mCards[mEditingCardIndex].mDescription = mCardDescriptionBuffer;
                    mCards[mEditingCardIndex].mBadges = mCardBadges;
                    mCards[mEditingCardIndex].mChecklist = mTempChecklist;
                }
                ImGui::CloseCurrentPopup();
            }
        }

        FontManager::Pop();

        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
}

bool CardList::RenderCardListFooter()
{
    const float dpiScale = FontManager::GetDpiScale();
    ImGui::BeginChild("##Footer", ImVec2(ImGui::GetContentRegionAvail().x, 60.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));               // Transparent
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 20)); // Subtle hover
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 255, 255, 40));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 255)); // Grey text
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    const float width = 256.0f * dpiScale;
    float x_center = (ImGui::GetContentRegionAvail().x - width) * 0.5f;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x_center);

    // Align text to left
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
    if(ImGui::Button(ICON_FA_PLUS "  Add a card", { width, 35.0f }))
    {
        mShowCardPopup = true;
        ResetCardListState();
    }
    ImGui::PopStyleVar(); // ButtonTextAlign
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar();
    ImGui::EndChild();

    return false;
}

void CardList::Render(int list_id, ImVec2 size)
{
    const float dpiScale = FontManager::GetDpiScale();
    std::vector<Dropzone>& aDropZones = DragDropManager::GetDropZones();
    if(list_id == 0)
    {
        aDropZones.clear();
        DragDropManager::ClearListBounds();
    }

    DragOperation& aDragOperation = DragDropManager::GetDragOperation();

    // Window setup
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(16, 18, 4, 255));
    // Push WindowBg to match ChildBg so ListHeader (which uses WindowBg) blends in.
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(16, 18, 4, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    // Use BeginChild instead of Begin for layout within the board
    // IMPORTANT: NoScrollbar here to prevent double scrollbars.
    // The inner content will handle scrolling.
    ImGui::BeginChild(
        (std::string("CardList_") + mUniqueID).c_str(),
        size,
        false, // No border
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
    );

    // Register list bounds for drag and drop logic
    ImRect listRect(
        ImGui::GetWindowPos(),
        ImVec2(
            ImGui::GetWindowPos().x + ImGui::GetWindowSize().x,
            ImGui::GetWindowPos().y + ImGui::GetWindowSize().y
        )
    );
    DragDropManager::RegisterListBounds(list_id, listRect);

    // Make inner children transparent so they don't cover the rounded corners of the parent
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));

    ListHeader();

    // Calculate remaining height for cards
    // Header is approx 40 + padding. Footer is 60.
    // We can use GetCursorPosY to find where we are after Header.
    float yAfterHeader = ImGui::GetCursorPosY();
    float footerHeight = 60.0f; // From RenderCardListFooter
    float spacing = ImGui::GetStyle().ItemSpacing.y;
    // Subtract spacing for before footer
    float availableHeight = size.y - yAfterHeader - footerHeight + spacing;

    // Ensure we have some minimum height
    if(availableHeight < 100.0f)
        availableHeight = 100.0f;

    ImGui::BeginChild(
        (std::string("CardContainer_") + mTitle).c_str(),
        ImVec2(0, availableHeight),
        false,
        ImGuiWindowFlags_None // Allow scrolling here
    );

    const ImGuiPayload* global_payload = ImGui::GetDragDropPayload();
    bool payload_active = (global_payload && global_payload->IsDataType("CARD_PAYLOAD"));

    Dropzone* current_drop = DragDropManager::GetCurrentDropZonePtr();

    for(size_t i = 0; i <= mCards.size(); ++i)
    {
        bool isCurrentCardDragging = false;
        if(payload_active && global_payload->IsDataType("CARD_PAYLOAD"))
        {
            const DragDropPayload* d = (const DragDropPayload*)global_payload->Data;
            if(d->source_list_id == list_id && d->card_index == (int)i)
                isCurrentCardDragging = true;
        }
        if(isCurrentCardDragging)
            continue;

        // ---- DROPZONE between cards ----

        std::string dropzone_id
            = std::string("dropzone_") + std::to_string(list_id) + "_" + std::to_string(i);
        ImGui::InvisibleButton(dropzone_id.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 1.0f));
        ImRect zone_rect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
        aDropZones.push_back({ zone_rect, list_id, (int)i });

        if(ImGui::BeginDragDropTarget())
        {
            if(const ImGuiPayload* p = ImGui::AcceptDragDropPayload("CARD_PAYLOAD"))
            {
                const DragDropPayload* d = (const DragDropPayload*)p->Data;
                aDragOperation = { d->source_list_id, d->card_index, list_id, (int)i };
            }
            ImGui::EndDragDropTarget();
        }

        // Highlight active dropzone
        if(current_drop && current_drop->list_id == list_id && current_drop->insert_index == (int)i)
        {
            // Render placeholder card (visual feedback)
            if(payload_active && global_payload->IsDataType("CARD_PAYLOAD"))
            {
                const DragDropPayload* d = (const DragDropPayload*)global_payload->Data;
                Card* moving_card = DragDropManager::GetCard(d->source_list_id, d->card_index);
                if(moving_card)
                {
                    float x_center = (ImGui::GetContentRegionAvail().x - 256.0f * dpiScale) * 0.5f;
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x_center);
                    Card::Render(
                        "placeholder_card",
                        moving_card->mTitle.c_str(),
                        moving_card->mBadges,
                        !moving_card->mDescription.empty(),
                        (int)std::count_if(
                            moving_card->mChecklist.begin(),
                            moving_card->mChecklist.end(),
                            [](const auto& i) { return i.isChecked; }
                        ),
                        (int)moving_card->mChecklist.size(),
                        true // render as placeholder
                    );

                    // Reserve space for layout
                    std::string dropzone_id = std::string("dropzonex_") + std::to_string(list_id)
                                              + "_" + std::to_string(i);
                    ImGui::InvisibleButton(
                        dropzone_id.c_str(),
                        ImVec2(ImGui::GetContentRegionAvail().x, 1.0f)
                    );
                }
            }
        }

        // ---- CARD RENDER ----
        if(i < mCards.size())
        {
            std::string card_id
                = std::string("card_") + std::to_string(list_id) + "_" + std::to_string(i);
            float x_center = (ImGui::GetContentRegionAvail().x - 256.0f * dpiScale) * 0.5f;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x_center);
            int completed = 0;
            for(const auto& item : mCards[i].mChecklist)
                if(item.isChecked)
                    completed++;
            Card::Render(
                card_id.c_str(),
                mCards[i].mTitle.c_str(),
                mCards[i].mBadges,
                !mCards[i].mDescription.empty(),
                completed,
                (int)mCards[i].mChecklist.size()
            );

            if(ImGui::IsItemHovered())
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

            if(ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                // Only open if we didn't drag
                if(ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.0f).x == 0.0f
                   && ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.0f).y == 0.0f)
                {
                    mShowCardPopup = true;
                    mEditingCardIndex = (int)i;
                    strcpy_s(mCardTitleBuffer, mCards[i].mTitle.c_str());
                    strcpy_s(mCardDescriptionBuffer, mCards[i].mDescription.c_str());
                    mCardBadges = mCards[i].mBadges;
                    mTempChecklist = mCards[i].mChecklist;
                    memset(mChecklistInputBuffer, 0, sizeof(mChecklistInputBuffer));
                }
            }

            if(ImGui::BeginDragDropSource(
                   ImGuiDragDropFlags_SourceNoPreviewTooltip
                   | ImGuiDragDropFlags_AcceptNoPreviewTooltip
                   | ImGuiDragDropFlags_AcceptNoDrawDefaultRect
               ))
            {
                DragDropPayload d = { list_id, (int)i };
                ImGui::SetDragDropPayload("CARD_PAYLOAD", &d, sizeof(d));

                ImGui::EndDragDropSource();
            }
        }
    }

    ImGui::EndChild(); // End CardContainer

    bool tOpenNewCardList = ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly)
                            && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
    if(tOpenNewCardList)
    {
        ResetCardListState();
    }

    RenderCardListFooter();
    if(mShowCardPopup || tOpenNewCardList)
    {
        ImGui::OpenPopup((std::string("Card Popup##") + mUniqueID).c_str());
        mShowCardPopup = false;
    }
    RenderCardPopup();

    ImGui::PopStyleColor(); // Pop transparent ChildBg
    ImGui::EndChild();      // End CardList

    ImGui::PopStyleColor(2); // ChildBg, WindowBg
    ImGui::PopStyleVar(3);
}
void CardList::ResetCardListState()
{
    mEditingCardIndex = -1;
    memset(mCardTitleBuffer, 0, sizeof(mCardTitleBuffer));
    memset(mCardDescriptionBuffer, 0, sizeof(mCardDescriptionBuffer));
    mCardBadges.clear();
    mTempChecklist.clear();
    memset(mChecklistInputBuffer, 0, sizeof(mChecklistInputBuffer));
}
