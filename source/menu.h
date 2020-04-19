#pragma once

#include <switch.h>
#include <map>
#include <string>
#include <vector>

struct Title
{
    u64 TitleID;
    std::string TitleName;
	u8 ReverseNXflag; //0 - handheld, 1 - docked, 2 - broken, 3 - default, 4 - system
};
const int max_title_items = 43;