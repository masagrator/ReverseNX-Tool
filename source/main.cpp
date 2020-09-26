#include <switch.h>
#include <dirent.h>
#include <vector>
#include <sstream>
#include <math.h>

#include <borealis.hpp>
#include "About_tab.hpp"

using namespace std;

enum Flag {
	Flag_Handheld = 0,
	Flag_Docked = 1,
	Flag_System = 2,
};

typedef uint8_t NsApplicationIcon[0x20000];

struct Title
{
	u64 TitleID;
	std::string TitleName;
	Flag ReverseNX;
	NsApplicationIcon icon;
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
NsApplicationControlData appControlData;

bool isAllUpper(const std::string& word)
{
	for(auto& c: word) if(std::islower(static_cast<unsigned char>(c))) return false;
    return true;
}

void RemoveReverseNX(u64 tid) {
	if (tid == UINT64_MAX) {
		for (uint8_t i = 0; i < 2; i++) {
			snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%s", Files[i]);
			remove(ReverseNX);
		}
	}
	else for (uint8_t i = 0; i < 2; i++) {
		snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%016" PRIx64 "/%s", tid, Files[i]);
		remove(ReverseNX);
	}
	brls::Application::notify("Found and deleted broken patches.");
}

void setReverseNX(uint64_t tid, Flag changedFlag) {
	
	for (uint8_t i = 0; i < 2; i++) {
		if (tid == UINT64_MAX) {
			snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%s", Files[i]);
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
		else {
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
}

Flag getReverseNX(uint64_t tid) {
	if (tid == UINT64_MAX) {
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
				if (c != 16) {
					fclose(asem);
					RemoveReverseNX(tid);
					return Flag_System;
				}
				else dockedflag = true;
			}
			else handheldflag = true;
			fclose(asem);
				
			snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%s", Files[1]);
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
					else {
						RemoveReverseNX(tid);
						return Flag_System;
					}
				}
			}
		}
		else {
			snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%016" PRIx64 "/%s", tid, Files[1]);
			asem = fopen(ReverseNX, "r");
			if (asem != NULL) {
				fclose(asem);
				RemoveReverseNX(tid);
				return Flag_System;
			}
			else return Flag_System;
		}
	}
	else {
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
					RemoveReverseNX(tid);
					return Flag_System;
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
					else {
						RemoveReverseNX(tid);
						return Flag_System;
					}
				}
			}
		}
		else {
			snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%016" PRIx64 "/%s", tid, Files[1]);
			asem = fopen(ReverseNX, "r");
			if (asem != NULL) {
				fclose(asem);
				RemoveReverseNX(tid);
			}
			return Flag_System;
		}
	}
		
	return Flag_System;
}

string getAppName(uint64_t Tid)
{
	size_t appControlDataSize = 0;
	NacpLanguageEntry *languageEntry = nullptr;
	Result rc;

	memset(&appControlData, 0x0, sizeof(NsApplicationControlData));

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
		memcpy(&title.icon, appControlData.icon, sizeof(title.icon));
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
	
	//Create patches folder if doesn't exist
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

		//List all titles with flags
		uint32_t count = static_cast<uint32_t>(titles.size());
		for (uint32_t i = 0; i < count; i++) {
			
			brls::SelectListItem* StatusItem = new brls::SelectListItem(titles.at(i).TitleName.c_str(), { "Handheld", "Docked", "System" }, (unsigned)titles.at(i).ReverseNX);
			
			double textLength = (double)titles.at(i).TitleName.size();
			uint8_t isCapital = 0;
			if (isAllUpper(titles.at(i).TitleName.c_str())) isCapital = 3;
			switch((int)textLength) case 33 ... 42: StatusItem->setTextSize(20 - isCapital);
			if (textLength >= 43) StatusItem->setTextSize((int)((20 / (pow(pow((textLength/43), (43/textLength)), 1.7)-0.06))) - isCapital);
			
			StatusItem->setThumbnail(titles.at(i).icon, sizeof(titles.at(i).icon));
			
			StatusItem->getValueSelectedEvent()->subscribe([i](size_t selection) {
				Flag changeFlag = (Flag)selection;
				setReverseNX(titles.at(i).TitleID, changeFlag);
			});
			
			OptionsList->addView(StatusItem);
		}
		
		//Add option to hide tab using cursed method
		OptionsList->registerAction("Hide tab", brls::Key::L, [titles, count] {
			brls::AppletFrame* FullOptionsFrame = new brls::AppletFrame(true, true);	
			FullOptionsFrame->setTitle("ReverseNX-Tool");
			FullOptionsFrame->setIcon(BOREALIS_ASSET("icon.jpg"));
			brls::List* FullOptionsList = new brls::List();
			brls::Label* Warning2 = new brls::Label(brls::LabelStyle::DESCRIPTION, "Any game on this list that is 32-bit or exists in exceptions list will be ignored by SaltyNX.", true);
			FullOptionsList->addView(Warning2);
			for (uint32_t i = 0; i < count; i++) {
				brls::SelectListItem* StatusItem2 = new brls::SelectListItem(titles.at(i).TitleName.c_str(), { "Handheld", "Docked", "System" }, (unsigned)titles.at(i).ReverseNX);
			
				StatusItem2->setThumbnail(titles.at(i).icon, sizeof(titles.at(i).icon));
				double textLength = (double)titles.at(i).TitleName.size();
				if (textLength >= 43) StatusItem2->setTextSize((int)((22 / pow((textLength/43), 0.212))));
				
				StatusItem2->getValueSelectedEvent()->subscribe([i](size_t selection) {
					Flag changeFlag = (Flag)selection;
					setReverseNX(titles.at(i).TitleID, changeFlag);
				});
				
				FullOptionsList->addView(StatusItem2);
			}
			
			FullOptionsList->registerAction("Show tab", brls::Key::R, [] {
				brls::Application::popView();
				return true;
			});
			
			FullOptionsFrame->setContentView(FullOptionsList);
			brls::Application::pushView(FullOptionsFrame);
			return true;
		});
		
		rootFrame->addTab("Games", OptionsList);
		
		//Settings
		brls::List* SettingsList = new brls::List();
		
		brls::SelectListItem* SettingItem = new brls::SelectListItem("Enforce mode globally", { "Handheld", "Docked", "Disabled" }, (unsigned)getReverseNX(UINT64_MAX), "Option to force all games set to System in Games tab to run in one mode");
		SettingItem->getValueSelectedEvent()->subscribe([](size_t selection) {
				Flag changeFlag = (Flag)selection;
				setReverseNX(UINT64_MAX, changeFlag);
		});
		
		SettingsList->addView(SettingItem);
		
		rootFrame->addTab("Settings", SettingsList);
		
		//About (check About_tab.cpp)
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
