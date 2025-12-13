#include "pch.h"
#include "CardListRenderer.h"
#include "renderers/CardRenderer.h"
#include "managers/DragDropManager.h"
#include "managers/FontManager.h"
#include "managers/DragDropTypes.h"
#include "FontAwesome6.h"
#include "BadgeColors.h"
#include "Components.h"
#include "Utils.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <string>

namespace Stride
{
    // Static member initialization
    CardListStyle CardListRenderer::sStyle;
    std::vector<std::string> CardListRenderer::sAvailableBadges
        = { "Bug", "Feature", "Urgent", "Design", "Dev", "Test", "High Priority", "Low Priority" };

    // CardListUIState implementation
    CardListUIState::CardListUIState() : uniqueId(genUID()) {}

    void CardListUIState::Reset()
    {
        isEditingTitle = false;
        memset(titleBuffer, 0, sizeof(titleBuffer));
        scrollY = 0.0f;
    }

    // CardEditorState implementation
    void CardEditorState::OpenForCreate()
    {
        isOpen = true;
        isEditing = false;
        editingCardId.clear();
        Reset();
    }

    void CardEditorState::OpenForEdit(const Card& card)
    {
        isOpen = true;
        isEditing = true;
        editingCardId = card.id;

        strcpy_s(titleBuffer, card.title.c_str());
        strcpy_s(descriptionBuffer, card.description.c_str());
        badges = card.badges;
        checklist = card.checklist;
    }

    void CardEditorState::Close()
    {
        isOpen = false;
        Reset();
    }

    void CardEditorState::Reset()
    {
        memset(titleBuffer, 0, sizeof(titleBuffer));
        memset(descriptionBuffer, 0, sizeof(descriptionBuffer));
        memset(badgeInputBuffer, 0, sizeof(badgeInputBuffer));
        memset(checklistInputBuffer, 0, sizeof(checklistInputBuffer));
        badges.clear();
        checklist.clear();
    }

    // CardListRenderer implementation
    void CardListRenderer::SetStyle(const CardListStyle& style) { sStyle = style; }

    const CardListStyle& CardListRenderer::GetStyle() { return sStyle; }

    void CardListRenderer::SetAvailableBadges(const std::vector<std::string>& badges)
    {
        sAvailableBadges = badges;
    }

    const std::vector<std::string>& CardListRenderer::GetAvailableBadges()
    {
        return sAvailableBadges;
    }

    void CardListRenderer::RenderHeader(CardList& data, CardListUIState& uiState)
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
        float button_size = 30.0f * dpiScale;
        float spacing = 6.0f * dpiScale;

        const ImGuiStyle& style = ImGui::GetStyle();
        const ImGuiID id = window->GetID(uiState.uniqueId.c_str());

        ImVec2 size(width, height);
        ImRect bb(
            window->DC.CursorPos,
            ImVec2(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y + size.y)
        );

        ImGui::ItemSize(size);
        if(!ImGui::ItemAdd(bb, id))
            return;

        ImGui::PushStyleColor(ImGuiCol_Text, sStyle.headerTextColor);
        ImVec2 text_pos
            = { bb.Min.x + padding_x,
                bb.Min.y
                    + (height - ImGui::GetTextLineHeight()
                       - (uiState.isEditingTitle ? (style.FramePadding.y * 2.0f) : 0.0f))
                          * 0.5f };
        FontManager::Push(FontFamily::SemiBold, FontSize::Regular);

        if(uiState.isEditingTitle)
        {
            ImGui::SetCursorScreenPos(text_pos);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(255, 255, 255, 0));
            ImGui::SetNextItemWidth(width - padding_x - (button_size * 2.0f) - spacing - 5.0f);
            if(ImGui::InputText(
                   (std::string("##titleEditor") + uiState.uniqueId).c_str(),
                   uiState.titleBuffer,
                   IM_ARRAYSIZE(uiState.titleBuffer),
                   ImGuiInputTextFlags_EnterReturnsTrue
               ))
            {
                data.title = uiState.titleBuffer;
                uiState.isEditingTitle = false;
            }
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
            ImGui::RenderText(text_pos, data.title.c_str(), nullptr);

            if(ImGui::IsItemClicked())
            {
                uiState.isEditingTitle = true;
                strcpy_s(uiState.titleBuffer, data.title.c_str());
            }

            if(ImGui::IsItemHovered())
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
        FontManager::Pop();

        ImVec2 btn2_pos = ImVec2(bb.Max.x - button_size, bb.Min.y + (height - button_size) * 0.5f);
        ImVec2 btn1_pos = ImVec2(btn2_pos.x - button_size - spacing, btn2_pos.y);

        ImGui::PushStyleColor(ImGuiCol_Button, sStyle.backgroundColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 50));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 255, 255, 100));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

        ImGui::SetCursorScreenPos(btn1_pos);
        FontManager::Push(FontFamily::Regular, FontSize::Smaller);
        if(ImGui::Button(
               ICON_FA_DOWN_LEFT_AND_UP_RIGHT_TO_CENTER,
               ImVec2(button_size, button_size)
           ))
        {
            uiState.isEditingTitle = false;
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

    void CardListRenderer::RenderFooter(CardList& data, CardEditorState& editorState)
    {
        const float dpiScale = FontManager::GetDpiScale();
        ImGui::BeginChild("##Footer", ImVec2(ImGui::GetContentRegionAvail().x, 60.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 20));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 255, 255, 40));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 255));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

        const float width = 256.0f * dpiScale;
        float x_center = (ImGui::GetContentRegionAvail().x - width) * 0.5f;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x_center);

        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
        if(ImGui::Button(ICON_FA_PLUS "  Add a card", { width, 35.0f }))
        {
            editorState.OpenForCreate();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
        ImGui::EndChild();
    }

    void CardListRenderer::RenderCardPopup(CardList& data, CardEditorState& editorState)
    {
        const float dpiScale = FontManager::GetDpiScale();

        float maxHeight = ImGui::GetMainViewport()->WorkSize.y * 0.9f;
        ImGui::SetNextWindowSizeConstraints(
            ImVec2(500 * dpiScale, 0),
            ImVec2(500 * dpiScale, maxHeight)
        );

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20 * dpiScale, 20 * dpiScale));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
        ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(28, 30, 34, 255));
        ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(50, 50, 55, 255));

        std::string popupId = std::string("Card Popup##") + data.id;
        if(ImGui::BeginPopupModal(
               popupId.c_str(),
               NULL,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
           ))
        {
            FontManager::Push(FontFamily::Bold, FontSize::Large);
            ImGui::Text(editorState.isEditing ? "Edit Card" : "Create New Card");
            FontManager::Pop();

            ImGui::Spacing();
            ImGui::Spacing();

            FontManager::Push(FontFamily::Regular, FontSize::Regular);
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Card Title");

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(16, 18, 20, 255));
            ImGui::SetNextItemWidth(-1);

            if(ImGui::IsWindowAppearing())
                ImGui::SetKeyboardFocusHere();

            bool enterPressed = ImGui::InputText(
                "##CardTitle",
                editorState.titleBuffer,
                sizeof(editorState.titleBuffer),
                ImGuiInputTextFlags_EnterReturnsTrue
            );
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            FontManager::Pop();

            ImGui::Spacing();

            // Description Section
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Description");

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(16, 18, 20, 255));
            ImGui::SetNextItemWidth(-1);

            ImGui::InputTextMultiline(
                "##CardDescription",
                editorState.descriptionBuffer,
                sizeof(editorState.descriptionBuffer),
                ImVec2(-1, 150 * dpiScale),
                ImGuiInputTextFlags_None | ImGuiInputTextFlags_WordWrap
            );
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();

            ImGui::Spacing();
            ImGui::Spacing();

            // Badge Selection
            FontManager::Push(FontFamily::Regular, FontSize::Regular);
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Badges");

            // Selected Badges
            if(!editorState.badges.empty())
            {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));
                bool isFirst = true;
                for(auto it = editorState.badges.begin(); it != editorState.badges.end();)
                {
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
                        it = editorState.badges.erase(it);
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

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));
            bool isFirst = true;
            for(const auto& badge : sAvailableBadges)
            {
                bool selected
                    = std::find(editorState.badges.begin(), editorState.badges.end(), badge)
                      != editorState.badges.end();
                if(selected)
                    continue;

                ImVec2 text_size = ImGui::CalcTextSize(badge.c_str());
                float padding_x = 8.0f * dpiScale;
                float width = text_size.x + (padding_x * 2.0f);

                if(!isFirst)
                {
                    float last_x2 = ImGui::GetItemRectMax().x;
                    float next_x2 = last_x2 + ImGui::GetStyle().ItemSpacing.x + width;
                    float window_max_x
                        = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

                    if(next_x2 < window_max_x)
                        ImGui::SameLine();
                }

                BadgeColors::BadgeStyle style = BadgeColors::GetBadgeStyleForText(badge);
                if(Components::Badge(badge.c_str(), style, false))
                {
                    editorState.badges.push_back(badge);
                }
                isFirst = false;
            }
            FontManager::Pop();
            ImGui::NewLine();
            ImGui::PopStyleVar();

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Checklist Section
            ImGui::Text("Checklist");

            int completed = 0;
            for(const auto& item : editorState.checklist)
                if(item.isChecked)
                    completed++;
            float progress = editorState.checklist.empty()
                                 ? 0.0f
                                 : (float)completed / (float)editorState.checklist.size();

            ImGui::SameLine();
            char progressOverlay[32];
            sprintf_s(progressOverlay, "%d/%d", completed, (int)editorState.checklist.size());

            float availWidth = ImGui::GetContentRegionAvail().x;
            float textWidth = ImGui::CalcTextSize(progressOverlay).x;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availWidth - textWidth);
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), progressOverlay);

            ImGui::Spacing();
            Components::ProgressBar(progress, ImGui::GetContentRegionAvail().x, 4.0f * dpiScale);
            ImGui::Spacing();

            // Checklist Items
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 8 * dpiScale));
            for(int i = 0; i < (int)editorState.checklist.size(); ++i)
            {
                ImGui::PushID(i);
                auto& item = editorState.checklist[i];
                bool deleted = false;

                Components::ChecklistItem(
                    item.text.c_str(),
                    &item.isChecked,
                    &deleted,
                    ImGui::GetContentRegionAvail().x
                );

                if(deleted)
                {
                    editorState.checklist.erase(editorState.checklist.begin() + i);
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
                   editorState.checklistInputBuffer,
                   sizeof(editorState.checklistInputBuffer),
                   ImGui::GetContentRegionAvail().x
               ))
            {
                if(strlen(editorState.checklistInputBuffer) > 0)
                {
                    ChecklistItem newItem;
                    newItem.text = editorState.checklistInputBuffer;
                    newItem.isChecked = false;
                    editorState.checklist.push_back(newItem);
                    memset(
                        editorState.checklistInputBuffer,
                        0,
                        sizeof(editorState.checklistInputBuffer)
                    );
                    ImGui::SetKeyboardFocusHere(-1);
                }
                else
                {
                    ImGui::SetKeyboardFocusHere(1);
                }
            }

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();

            // Buttons
            float width = ImGui::GetContentRegionAvail().x;
            float btnWidth = (width - 10.0f * dpiScale) * 0.5f;

            FontManager::Push(FontFamily::SemiBold, FontSize::Regular);

            if(Components::StyledButton(
                   "Cancel",
                   ImVec2(btnWidth, 35 * dpiScale),
                   IM_COL32(45, 45, 50, 255),
                   IM_COL32(60, 60, 65, 255)
               )
               || ImGui::IsKeyPressed(ImGuiKey_Escape))
            {
                editorState.Close();
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            const char* btnText = editorState.isEditing ? "Save" : "Create";
            if(Components::StyledButton(
                   btnText,
                   ImVec2(btnWidth, 35 * dpiScale),
                   IM_COL32(59, 130, 246, 255),
                   IM_COL32(37, 99, 235, 255)
               )
               || enterPressed)
            {
                if(strlen(editorState.titleBuffer) > 0)
                {
                    if(editorState.isEditing)
                    {
                        // Update existing card
                        if(Card* card = data.FindCard(editorState.editingCardId))
                        {
                            card->title = editorState.titleBuffer;
                            card->description = editorState.descriptionBuffer;
                            card->badges = editorState.badges;
                            card->checklist = editorState.checklist;
                        }
                    }
                    else
                    {
                        // Create new card
                        Card newCard(
                            editorState.titleBuffer,
                            editorState.descriptionBuffer,
                            editorState.badges
                        );
                        newCard.checklist = editorState.checklist;
                        data.AddCard(std::move(newCard));
                    }
                    editorState.Close();
                    ImGui::CloseCurrentPopup();
                }
            }

            FontManager::Pop();

            ImGui::EndPopup();
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
    }

    void CardListRenderer::RenderCards(CardList& data, CardEditorState& editorState, int listIndex)
    {
        const float dpiScale = FontManager::GetDpiScale();
        const ImGuiPayload* global_payload = ImGui::GetDragDropPayload();
        bool payload_active = (global_payload && global_payload->IsDataType("CARD_PAYLOAD"));

        std::vector<Dropzone>& aDropZones = DragDropManager::GetDropZones();
        DragOperation& aDragOperation = DragDropManager::GetDragOperation();
        Dropzone* current_drop = DragDropManager::GetCurrentDropZonePtr();

        for(size_t i = 0; i <= data.cards.size(); ++i)
        {
            bool isCurrentCardDragging = false;
            if(payload_active && global_payload->IsDataType("CARD_PAYLOAD"))
            {
                const DragDropPayload* d = (const DragDropPayload*)global_payload->Data;
                if(d->source_list_id == listIndex && d->card_index == (int)i)
                    isCurrentCardDragging = true;
            }
            if(isCurrentCardDragging)
                continue;

            // Dropzone between cards
            std::string dropzone_id
                = std::string("dropzone_") + std::to_string(listIndex) + "_" + std::to_string(i);
            ImGui::InvisibleButton(
                dropzone_id.c_str(),
                ImVec2(ImGui::GetContentRegionAvail().x, 1.0f)
            );
            ImRect zone_rect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
            aDropZones.push_back({ zone_rect, listIndex, (int)i });

            if(ImGui::BeginDragDropTarget())
            {
                if(const ImGuiPayload* p = ImGui::AcceptDragDropPayload("CARD_PAYLOAD"))
                {
                    const DragDropPayload* d = (const DragDropPayload*)p->Data;
                    aDragOperation = { d->source_list_id, d->card_index, listIndex, (int)i };
                }
                ImGui::EndDragDropTarget();
            }

            // Highlight active dropzone
            if(current_drop && current_drop->list_id == listIndex
               && current_drop->insert_index == (int)i)
            {
                if(payload_active && global_payload->IsDataType("CARD_PAYLOAD"))
                {
                    const DragDropPayload* d = (const DragDropPayload*)global_payload->Data;
                    Card* moving_card = DragDropManager::GetCard(d->source_list_id, d->card_index);
                    if(moving_card)
                    {
                        float x_center
                            = (ImGui::GetContentRegionAvail().x - 256.0f * dpiScale) * 0.5f;
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x_center);
                        CardRenderer::Render(*moving_card, "placeholder_card", true);

                        std::string dropzone_id_x = std::string("dropzonex_")
                                                    + std::to_string(listIndex) + "_"
                                                    + std::to_string(i);
                        ImGui::InvisibleButton(
                            dropzone_id_x.c_str(),
                            ImVec2(ImGui::GetContentRegionAvail().x, 1.0f)
                        );
                    }
                }
            }

            // Card Render
            if(i < data.cards.size())
            {
                std::string card_id
                    = std::string("card_") + std::to_string(listIndex) + "_" + std::to_string(i);
                float x_center = (ImGui::GetContentRegionAvail().x - 256.0f * dpiScale) * 0.5f;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x_center);
                CardRenderer::Render(data.cards[i], card_id.c_str());

                if(ImGui::IsItemHovered())
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                if(ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                {
                    if(ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.0f).x == 0.0f
                       && ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.0f).y == 0.0f)
                    {
                        editorState.OpenForEdit(data.cards[i]);
                    }
                }

                if(ImGui::BeginDragDropSource(
                       ImGuiDragDropFlags_SourceNoPreviewTooltip
                       | ImGuiDragDropFlags_AcceptNoPreviewTooltip
                       | ImGuiDragDropFlags_AcceptNoDrawDefaultRect
                   ))
                {
                    DragDropPayload d = { listIndex, (int)i };
                    ImGui::SetDragDropPayload("CARD_PAYLOAD", &d, sizeof(d));
                    ImGui::EndDragDropSource();
                }
            }
        }
    }

    void CardListRenderer::Render(
        CardList& data,
        CardListUIState& uiState,
        CardEditorState& editorState,
        int listIndex,
        ImVec2 size
    )
    {
        const float dpiScale = FontManager::GetDpiScale();
        std::vector<Dropzone>& aDropZones = DragDropManager::GetDropZones();

        if(listIndex == 0)
        {
            aDropZones.clear();
            DragDropManager::ClearListBounds();
        }

        ImGui::PushStyleColor(ImGuiCol_ChildBg, sStyle.backgroundColor);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, sStyle.backgroundColor);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, sStyle.cornerRadius);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGui::BeginChild(
            (std::string("CardList_") + uiState.uniqueId).c_str(),
            size,
            false,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
        );

        ImRect listRect(
            ImGui::GetWindowPos(),
            ImVec2(
                ImGui::GetWindowPos().x + ImGui::GetWindowSize().x,
                ImGui::GetWindowPos().y + ImGui::GetWindowSize().y
            )
        );
        DragDropManager::RegisterListBounds(listIndex, listRect);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));

        RenderHeader(data, uiState);

        float yAfterHeader = ImGui::GetCursorPosY();
        float footerHeight = 60.0f;
        float spacing = ImGui::GetStyle().ItemSpacing.y;
        float availableHeight = size.y - yAfterHeader - footerHeight + spacing;

        if(availableHeight < 100.0f)
            availableHeight = 100.0f;

        ImGui::BeginChild(
            (std::string("CardContainer_") + data.title).c_str(),
            ImVec2(0, availableHeight),
            false,
            ImGuiWindowFlags_None
        );

        RenderCards(data, editorState, listIndex);

        ImGui::EndChild();

        bool openNewCard = ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly)
                           && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
        if(openNewCard)
        {
            editorState.OpenForCreate();
        }

        RenderFooter(data, editorState);

        if(editorState.isOpen || openNewCard)
        {
            ImGui::OpenPopup((std::string("Card Popup##") + data.id).c_str());
            editorState.isOpen = true;
        }

        RenderCardPopup(data, editorState);

        ImGui::PopStyleColor();
        ImGui::EndChild();

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(3);
    }
}
