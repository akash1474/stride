#pragma once
#include <vector>
#include "CardList.h"

class Board
{
public:
    static Board& Get()
    {
        static Board instance;
        return instance;
    }

    void Setup();
    void Render();

    std::vector<CardList> mCardLists;

private:
    Board() = default;

    bool mIsAddingList = false;
    char mNewListTitleBuffer[256] = "";
    char mTitle[512] = "TxEdit - A Minimal IDE";
};
