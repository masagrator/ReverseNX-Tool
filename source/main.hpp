#pragma once

#include <switch.h>
#include <borealis.hpp>
#include <math.h>

enum Flag {
	Flag_Handheld = 0,
	Flag_Docked = 1,
	Flag_System = 2,
};

typedef uint8_t NsApplicationIcon[0x20000];

struct Title
{
	uint64_t TitleID;
	std::string TitleName;
	Flag ReverseNX;
	NsApplicationIcon icon;
};

extern uint32_t countGames;
extern bool isAlbum;
extern std::vector<Title> titles;
extern Flag changeFlag;
extern bool memorySafety;

extern void getAppIcon(uint64_t Tid, void* iconBufferPtr);
extern void setReverseNX(uint64_t tid, Flag changedFlag, bool bit32);
extern bool isAllUpper(const std::string& word);
extern Flag getReverseNX(uint64_t tid);
