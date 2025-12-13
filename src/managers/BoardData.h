#pragma once
#include <string>
#include <vector>
#include "CardList.h"

using Stride::CardList;

struct BoardData
{
    std::string mID;
    std::string mTitle;
    std::vector<CardList> mCardLists;

    BoardData(std::string id, std::string title)
        : mID(std::move(id)), mTitle(std::move(title)) {}
};
