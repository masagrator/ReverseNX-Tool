#pragma once
#include <switch.h>
#include <vector>
#include <string>
#include <algorithm>
#include "menu.h"

#define center(p, c) (int)((p - c) / 2)

template <typename T>
auto findIT(const std::vector<T> &vecOfElements, const T &element) { return std::find(vecOfElements.begin(), vecOfElements.end(), element); }

namespace Utils
{
extern int gameSelected;
extern int configSelected;
extern std::vector<Title> titles;
extern int maxTitlePages;
extern uint8_t Docked[0x10];
extern uint8_t Handheld[0x10];
extern char Files[2][38];

std::vector<Title> getAllTitles();
std::string getAppName(u64 Tid);
void printTitles(int selection, int titlePage, int onScreenItems);
void printItems(const std::vector<std::string> &items, std::string menuTitle, int selection);
u64 GetControllerInputs();
} // namespace Utils