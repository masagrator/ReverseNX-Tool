#include <dirent.h>
#include <vector>

#include <borealis.hpp>
#include "main.hpp"
#include "FullOptionsFrame.hpp"
#include "TabOptionsFrame.hpp"

using namespace std;

std::vector<Title> titles;
NsApplicationControlData appControlData;

uint8_t Docked[0x10] = {0xE0, 0x03, 0x00, 0x32, 0xC0, 0x03, 0x5F, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t Handheld[0x10] = {0x00, 0x00, 0xA0, 0x52, 0xC0, 0x03, 0x5F, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
bool dockedflag = false;
bool handheldflag = false;
bool isAlbum = false;
char Files[2][38] = { "_ZN2nn2oe18GetPerformanceModeEv.asm64", "_ZN2nn2oe16GetOperationModeEv.asm64" };
char ReverseNX[128];
uint8_t filebuffer[0x10] = {0};
uint32_t countGames = 0;
Flag changeFlag = Flag_Handheld;
bool memorySafety = false;

void isFullRAM() {
	switch(appletGetAppletType()) {
		case AppletType_Application:
			break;
		
		case AppletType_SystemApplication:
			break;
		
		default:
			isAlbum = true;
			break;
	}
}

bool isAllUpper(const std::string& word)
{
	for(auto& c: word) if(std::islower(static_cast<unsigned char>(c))) return false;
	return true;
}

void RemoveReverseNX(u64 tid) {
	if (tid == UINT64_MAX) for (uint8_t i = 0; i < 2; i++) {
		snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%s", Files[i]);
		remove(ReverseNX);
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
	uint8_t cmpresult = 0;
	
	if (tid == UINT64_MAX) {
		snprintf(ReverseNX, sizeof ReverseNX, "sdmc:/SaltySD/patches/%s", Files[0]);
		FILE* asem = fopen(ReverseNX, "r");
		
		if (asem != NULL) {
			fread(&filebuffer, 1, 16, asem);
			cmpresult = memcmp(filebuffer, Handheld, sizeof(Handheld));
			if (cmpresult != 0) {
				cmpresult = memcmp(filebuffer, Docked, sizeof(Docked));
				if (cmpresult != 0) {
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
				cmpresult = memcmp(filebuffer, Handheld, sizeof(Handheld));
				if (cmpresult == 0 && handheldflag == true) {
					fclose(asem);
					return Flag_Handheld;
				}
				else {
					cmpresult = memcmp(filebuffer, Docked, sizeof(Docked));
					fclose(asem);
					if (cmpresult == 0 && dockedflag == true) return Flag_Docked;
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
			cmpresult = memcmp(filebuffer, Handheld, sizeof(Handheld));
			if (cmpresult != 0) {
				cmpresult = memcmp(filebuffer, Docked, sizeof(Docked));
				if (cmpresult != 0) {
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
				cmpresult = memcmp(filebuffer, Handheld, sizeof(Handheld));
				if (cmpresult == 0 && handheldflag == true) {
					fclose(asem);
					return Flag_Handheld;
				}
				else {
					cmpresult = memcmp(filebuffer, Docked, sizeof(Docked));
					fclose(asem);
					if (cmpresult == 0 && dockedflag == true) return Flag_Docked;
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

void getAppIcon(uint64_t Tid, void* iconBufferPtr)
{	
	size_t appControlDataSize = 0;
	if (R_FAILED(nsGetApplicationControlData(NsApplicationControlSource::NsApplicationControlSource_Storage, Tid, &appControlData, sizeof(NsApplicationControlData), &appControlDataSize))) {
		return;
	}
	memcpy(iconBufferPtr, appControlData.icon, sizeof(appControlData.icon));
	return;
}

string getAppName(uint64_t Tid)
{
	size_t appControlDataSize = 0;
	if (R_FAILED(nsGetApplicationControlData(NsApplicationControlSource::NsApplicationControlSource_Storage, Tid, &appControlData, sizeof(NsApplicationControlData), &appControlDataSize))) {
		char returnTID[17];
		sprintf(returnTID, "%016" PRIx64, Tid);
		return (std::string)returnTID;
	}
	
	NacpLanguageEntry *languageEntry = nullptr;
	if (R_FAILED(nacpGetLanguageEntry(&appControlData.nacp, &languageEntry))) {
		char returnTID[17];
		sprintf(returnTID, "%016" PRIx64, Tid);
		return (std::string)returnTID;
	}
	
	return string(languageEntry->name);
}

std::vector<Title> getTitles(int32_t count)
{
  std::vector<Title> apps;
  NsApplicationRecord appRecords = {};
  int32_t actualAppRecordCnt = 0;
  Result rc;

  while (1)
  {
    static int32_t offset = 0;
    rc = nsListApplicationRecord(&appRecords, 1, offset, &actualAppRecordCnt);
    if (R_FAILED(rc) || (actualAppRecordCnt < 1) || (offset >= count)) break;
    if (appRecords.application_id != 0) {
        Title title;
        title.TitleID = appRecords.application_id;
        title.TitleName = getAppName(appRecords.application_id);
	    title.ReverseNX = getReverseNX(appRecords.application_id);
        getAppIcon(appRecords.application_id, &title.icon);
        apps.push_back(title);
    }
    offset++;
    appRecords = {};
  }
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
	
	//Create patches folder if doesn't exist
	DIR* patches_dir = opendir("sdmc:/SaltySD/patches");
	if (patches_dir == NULL) mkdir("sdmc:/SaltySD/patches", 777);
	else closedir(patches_dir);
	memset(&appControlData, 0x0, sizeof(NsApplicationControlData));
	
	Result nsError = nsInitialize();
		
	if (R_FAILED(nsError)) {
		char Error[64];
		sprintf(Error, "nsInitialize error: 0x%08x\nApplication will be closed.", (uint32_t)nsError);
		brls::Application::crash((std::string)Error);
	}
	
	else {
		titles = getTitles(INT32_MAX);
		isFullRAM();

		// Add the root view to the stack
		brls::Application::pushView(new TabOptionsFrame());
	}


	// Run the app
	while (brls::Application::mainLoop());

	// Exit
	nsExit();
	return EXIT_SUCCESS;
}
