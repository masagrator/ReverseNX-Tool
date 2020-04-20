#include "Utils.h"
#include <sstream>
#include <cstring>
#include "States/ErrorMenu.h"
#include <sys/stat.h>
#include <inttypes.h>

using namespace std;

namespace Utils
{
int gameSelected;
int configSelected = 0;
std::vector<Title> titles;
int maxTitlePages = titles.size() / max_title_items;

uint8_t Docked[0x10] = {0xE0, 0x03, 0x00, 0x32, 0xC0, 0x03, 0x5F, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t Handheld[0x10] = {0x00, 0x00, 0xA0, 0x52, 0xC0, 0x03, 0x5F, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
bool dockedflag = false;
bool handheldflag = false;
bool brokendata = false;
bool system = false;
char Files[2][38] = { "_ZN2nn2oe18GetPerformanceModeEv.asm64", "_ZN2nn2oe16GetOperationModeEv.asm64" };

//Thanks WerWolv :)
void printTitles(int selection, int titlePage, int onScreenItems)
{
    consoleClear();
    printf(CONSOLE_MAGENTA "\x1b[0;36HReverseNX\n");
    int start = titlePage * max_title_items;
    int end = std::min(static_cast<int>(titles.size()), start + max_title_items);
    int j = 0;
    for (int i = start; i < end; i++)
    {
        const char *prefix = " ";
        if (selection == j)
            prefix = ">";
        printf(CONSOLE_WHITE "%s%s\n", prefix, titles.at(i).TitleName.c_str());
        j++;
    }
    onScreenItems = j;
    printf(CONSOLE_MAGENTA "Page %d/%d", titlePage + 1, maxTitlePages + 1);
}

void printItems(const vector<string> &items, string menuTitle, int selection)
{
    consoleClear();
    printf(CONSOLE_MAGENTA "\x1b[0;%dH%s\n", center(80, menuTitle.size()), menuTitle.c_str());
    for (int i = 0; i < (int)items.size(); i++)
    {
        const char *prefix = " ";
        if (selection == i)
            prefix = ">";
        printf(CONSOLE_WHITE "%s%s", prefix, items[i].c_str());
        printf("\n");
    }
}

vector<Title> getAllTitles()
{
    vector<Title> apps;
    NsApplicationRecord *appRecords = new NsApplicationRecord[1024]; // Nobody's going to have more than 1024 games hopefully...
    s32 actualAppRecordCnt = 0;
	char ReverseNX[128];
	uint8_t filebuffer[0x10] = {0};
    Result rc;
    rc = nsListApplicationRecord(appRecords, 1024, 0, &actualAppRecordCnt);
    if (R_FAILED(rc))
    {
        ErrorMenu::error = rc;
        return apps;
    }
	
	Title title;
    title.TitleID = 0x0F0F0F0F0F0F0F0F;
	title.TitleName = "Default";
	snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%s", Files[0]);
	FILE* asem = fopen(ReverseNX, "r");
	if (asem != NULL) {
		fread(&filebuffer, 1, 16, asem);
		int c = 0;
		while (c < 16) {
			if (filebuffer[c] == Handheld[c]) c++;
			else break;
		}
		if (c != 16) {
			c = 0;
			while (c < 16) {
				if (filebuffer[c] == Docked[c]) c++;
				else break;
			}
			if (c != 16) brokendata = true;
		}
		fclose(asem);
		if (brokendata == false) {
			snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%s", Files[1]);
			asem = fopen(ReverseNX, "r");
			if (asem != NULL) {
				fread(&filebuffer, 1, 16, asem);
				c = 0;
				while (c < 16) {
					if (filebuffer[c] == Handheld[c]) c++;
					else break;
				}
				if (c == 16) handheldflag = true;
				else {
					c = 0;
					while (c < 16) {
						if (filebuffer[c] == Docked[c]) c++;
						else break;
					}
					if (c == 16) dockedflag = true;
					else brokendata = true;
				}
				fclose(asem);
			}
			else brokendata = true;
		}
	}
	else {
		snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%s", Files[1]);
		asem = fopen(ReverseNX, "r");
		if (asem != NULL) {
			brokendata = true;
			fclose(asem);
		}
	}
	if (brokendata == true) {
		title.ReverseNXflag = 2;
		title.TitleName = title.TitleName + ": Broken Data";
	}
	else if (handheldflag == true) {
		title.ReverseNXflag = 0;
		title.TitleName = title.TitleName + ": Handheld";
	}
	else if (dockedflag == true) {
		title.ReverseNXflag = 1;
		title.TitleName = title.TitleName + ": Docked";
	}
	else {
		title.ReverseNXflag = 4;
		title.TitleName = title.TitleName + ": System";
	}
	brokendata = false;
	handheldflag = false;
	dockedflag = false;
    apps.push_back(title);

    for (s32 i = 0; i < actualAppRecordCnt; i++)
    {
        title.TitleID = appRecords[i].application_id;
		snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%016" PRIx64 "/%s", title.TitleID, Files[0]);
		FILE* asem = fopen(ReverseNX, "r");
		if (asem != NULL) {
			fread(&filebuffer, 1, 16, asem);
			int c = 0;
			while (c < 16) {
				if (filebuffer[c] == Handheld[c]) c++;
				else break;
			}
			if (c != 16) {
				c = 0;
				while (c < 16) {
					if (filebuffer[c] == Docked[c]) c++;
					else break;
				}
				if (c != 16) brokendata = true;
			}
			fclose(asem);
			if (brokendata == false) {
				snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%016" PRIx64 "/%s", title.TitleID, Files[1]);
				asem = fopen(ReverseNX, "r");
				if (asem != NULL) {
					fread(&filebuffer, 1, 16, asem);
					c = 0;
					while (c < 16) {
						if (filebuffer[c] == Handheld[c]) c++;
						else break;
					}
					if (c == 16) handheldflag = true;
					else {
						c = 0;
						while (c < 16) {
							if (filebuffer[c] == Docked[c]) c++;
							else break;
						}
						if (c == 16) dockedflag = true;
						else brokendata = true;
					}
					fclose(asem);
				}
				else brokendata = true;
			}
		}
		else {
			snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%016" PRIx64 "/%s", title.TitleID, Files[1]);
			asem = fopen(ReverseNX, "r");
			if (asem != NULL) {
				brokendata = true;
				fclose(asem);
			}
		}
		title.TitleName = getAppName(appRecords[i].application_id);
		if (brokendata == true) {
			title.ReverseNXflag = 2;
			title.TitleName = title.TitleName + ": Broken Data";
		}
		else if (handheldflag == true) {
			title.ReverseNXflag = 0;
			title.TitleName = title.TitleName + ": Handheld";
		}
		else if (dockedflag == true) {
			title.ReverseNXflag = 1;
			title.TitleName = title.TitleName + ": Docked";
		}
		else {
			title.ReverseNXflag = 3;
			title.TitleName = title.TitleName + ": Default";
		}
		brokendata = false;
		handheldflag = false;
		dockedflag = false;
        apps.push_back(title);
    }
    delete[] appRecords;
    return apps;
}

string getAppName(u64 Tid)
{
    NsApplicationControlData appControlData;
    size_t appControlDataSize = 0;
    NacpLanguageEntry *languageEntry = nullptr;
    Result rc;

    memset(&appControlData, 0x00, sizeof(NsApplicationControlData));

    rc = nsGetApplicationControlData(NsApplicationControlSource::NsApplicationControlSource_Storage, Tid, &appControlData, sizeof(NsApplicationControlData), &appControlDataSize);
    if (R_FAILED(rc))
    {
        stringstream ss;
        ss << 0 << hex << uppercase << Tid;
        return ss.str();
    }
    rc = nacpGetLanguageEntry(&appControlData.nacp, &languageEntry);
    if (R_FAILED(rc))
    {
        stringstream ss;
        ss << 0 << hex << uppercase << Tid;
        return ss.str();
    }
    return string(languageEntry->name);
}

u64 GetControllerInputs()
{
    hidScanInput();
    u64 kDown = 0;
    for (u8 controller = 0; controller < 10; controller++)
        kDown |= hidKeysDown(static_cast<HidControllerID>(controller));

    return kDown;
}
} // namespace Utils
