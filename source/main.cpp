#include <switch.h>
#include <dirent.h>
#include <vector>
#include <sstream>

#include <borealis.hpp>
#include "About_tab.hpp"

using namespace std;

enum Flag {
	Flag_Broken = 0,
	Flag_Handheld = 1,
	Flag_Docked = 2,
	Flag_System = 3
};

struct Title
{
    u64 TitleID;
    std::string TitleName;
	Flag ReverseNX;
};

Result nsError = 0x1;
std::vector<Title> titles;

uint8_t Docked[0x10] = {0xE0, 0x03, 0x00, 0x32, 0xC0, 0x03, 0x5F, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t Handheld[0x10] = {0x00, 0x00, 0xA0, 0x52, 0xC0, 0x03, 0x5F, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
bool dockedflag = false;
bool handheldflag = false;
char Files[2][38] = { "_ZN2nn2oe18GetPerformanceModeEv.asm64", "_ZN2nn2oe16GetOperationModeEv.asm64" };
char ReverseNX[128];
uint8_t filebuffer[0x10] = {0};

void setReverseNX(uint64_t tid, Flag changedFlag) {
	
	for (uint8_t i = 0; i < 2; i++) {
		snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%016" PRIx64, tid);
		DIR* patchdir = opendir(ReverseNX);
		if (patchdir == NULL) mkdir(ReverseNX, 777);
		else closedir(patchdir);
		snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%016" PRIx64 "/%s", tid, Files[i]);
		FILE* asem = fopen(ReverseNX, "wb");
		switch(changedFlag) {
			case Flag_Handheld:
				fwrite(Handheld, 1, 16, asem);
				fclose(asem);
				break;
			
			case Flag_Docked:
				fwrite(Docked, 1, 16, asem);
				fclose(asem);
				break;
			
			default:
				fclose(asem);
				remove(ReverseNX);
				break;
				
		}
	}
}

Flag getReverseNX(uint64_t tid) {
	snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%016" PRIx64 "/%s", tid, Files[0]);
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
			if (c != 16) {
				fclose(asem);
				return Flag_Broken;
			}
			else dockedflag = true;
		}
		else handheldflag = true;
		fclose(asem);
			
		snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%016" PRIx64 "/%s", tid, Files[1]);
		asem = fopen(ReverseNX, "r");
		if (asem != NULL) {
			fread(&filebuffer, 1, 16, asem);
			c = 0;
			while (c < 16) {
				if (filebuffer[c] == Handheld[c]) c++;
				else break;
			}
			if (c == 16 && handheldflag == true) {
				fclose(asem);
				return Flag_Handheld;
			}
			else {
				c = 0;
				while (c < 16) {
					if (filebuffer[c] == Docked[c]) c++;
					else break;
				}
				fclose(asem);
				if (c == 16 && dockedflag == true) return Flag_Docked;
				else return Flag_Broken;
			}
		}
	}
	else {
		snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%016" PRIx64 "/%s", tid, Files[1]);
		asem = fopen(ReverseNX, "r");
		if (asem != NULL) {
			fclose(asem);
			return Flag_Broken;
		}
		else return Flag_System;
	}
	
	return Flag_Broken;
}

string getAppName(uint64_t Tid)
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

vector<Title> getAllTitles()
{
    vector<Title> apps;
    NsApplicationRecord *appRecords = new NsApplicationRecord[1024]; // Nobody's going to have more than 1024 games hopefully...
    s32 actualAppRecordCnt = 0;
    Result rc;
    rc = nsListApplicationRecord(appRecords, 1024, 0, &actualAppRecordCnt);
    if (R_FAILED(rc))
    {
		while (brls::Application::mainLoop());
		exit(rc);
    }

    for (s32 i = 0; i < actualAppRecordCnt; i++)
    {
        Title title;
        title.TitleID = appRecords[i].application_id;
        title.TitleName = getAppName(appRecords[i].application_id);
		title.ReverseNX = getReverseNX(appRecords[i].application_id);
        apps.push_back(title);
    }
    delete[] appRecords;
    return apps;
}

int main(int argc, char *argv[])
{
	// Init the app

	if (!brls::Application::init("ReverseNX-Tool"))
	{
		brls::Logger::error("Unable to init ReverseNX-Tool");
		return EXIT_FAILURE;
	}
	
	brls::TabFrame* rootFrame = new brls::TabFrame();
	
	DIR* patches_dir = opendir("sdmc:/SaltySD/patches");
	if (patches_dir == NULL) mkdir("sdmc:/SaltySD/patches", 777);
	else closedir(patches_dir);
	
	nsError = nsInitialize();
		
	if (R_FAILED(nsError)) {
		char Error[64];
		sprintf(Error, "nsInitialize error: 0x%08x\nApplication will be closed.", (uint32_t)nsError);
		std::string stdError = Error;
		brls::Application::crash(stdError);
	}
	
	else {
		titles = getAllTitles();

		// Create a sample view
		rootFrame->setTitle("ReverseNX-Tool");
		rootFrame->setIcon(BOREALIS_ASSET("icon.jpg"));
		
		brls::List* OptionsList = new brls::List();
		
		brls::Label* Warning = new brls::Label(brls::LabelStyle::DESCRIPTION, "Any game on this list that is 32-bit or exists in exceptions list will be ignored by SaltyNX.", true);
		OptionsList->addView(Warning);
		
		uint32_t count = static_cast<uint32_t>(titles.size());
		for (uint32_t i = 0; i < count; i++) {
			
			brls::ListItem* StatusItem = new brls::ListItem(titles.at(i).TitleName.c_str());
			switch (titles.at(i).ReverseNX) {
				case Flag_Handheld:
					StatusItem->setValue("Handheld");
					break;
					
				case Flag_Docked:
					StatusItem->setValue("Docked");
					break;
					
				case Flag_System:
					StatusItem->setValue("System");
					break;
					
				case Flag_Broken:
					StatusItem->setValue("Handheld");
					break;
					
				default:
					StatusItem->setValue("Error");
					break;
			}
			StatusItem->getClickEvent()->subscribe([i](brls::View* view) {
				Flag changeFlag = Flag_Docked;
				setReverseNX(titles.at(i).TitleID, changeFlag);
			});
			
			OptionsList->addView(StatusItem);
		}
		
		rootFrame->addTab("Games", OptionsList);
		
		rootFrame->addTab("About", new AboutTab());

	}

	// Add the root view to the stack
	brls::Application::pushView(rootFrame);

	// Run the app
	while (brls::Application::mainLoop());

	// Exit
	nsExit();
	return EXIT_SUCCESS;
}