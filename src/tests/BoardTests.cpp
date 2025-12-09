#include "pch.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_test_engine/imgui_te_engine.h"
#include "imgui_test_engine/imgui_te_context.h"
#include "managers/BoardManager.h"

ImGuiID FindItemBySubstring(ImGuiTestContext* ctx, const char* substring)
{
    ImGuiTestItemList items;
    ctx->GatherItems(&items, ctx->GetRef(), 99);
    ctx->LogItemList(&items);
    
    for (const auto& item : items)
    {
        if (item.ID == 0) continue;
        if (strstr(item.DebugLabel, substring))
        {
            return item.ID;
        }
    }
    return 0;
}


void RegisterBoardTests(ImGuiTestEngine* engine)
{
    // -----------------------------------------------------------------
    // Test: Add a new list to the board
    // -----------------------------------------------------------------
    ImGuiTest* t = IM_REGISTER_TEST(engine, "Board", "AddList");
    t->TestFunc = [](ImGuiTestContext* ctx) {
        // Reset state
        auto& boardManager = BoardManager::Get();
        boardManager.Setup(); // Resets to default state
        
        BoardData* activeBoard = boardManager.GetActiveBoard();
        IM_CHECK(activeBoard != nullptr);
        
        size_t initial_count = activeBoard->mCardLists.size();

        // 1. Set Reference Window
        ctx->SetRef("Board");
        ImGuiID id= FindItemBySubstring(ctx, "Add another list");
        
        // 2. Click "Add another list"
        ctx->ItemClick(id);
        
        // 3. Enter Title
        ctx->ItemInput("**/##NewListTitle");
        ctx->KeyCharsReplaceEnter("Test List");
        
        // 4. Verify
        ctx->Yield(); 
        
        size_t final_count = activeBoard->mCardLists.size();
        IM_CHECK_EQ(final_count, initial_count + 1);
        
        if (final_count > 0)
        {
            IM_CHECK_STR_EQ(activeBoard->mCardLists.back().mTitle.c_str(), "Test List");
        }
    };

    //Board\/BoardContent_02388B04/+  Add another list
    //Board\/BoardContent_02388B04/+  Add another list
    //Board\/BoardContent_02388B04/+  Add another list##AddAnotherList
}