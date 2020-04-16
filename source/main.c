#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include <switch.h>

bool CheckPort () {
	Result ret;
	Handle saltysd;
	for (int i = 0; i < 200; i++)
	{
		ret = svcConnectToNamedPort(&saltysd, "InjectServ");
		svcSleepThread(1000*1000);
		
		if (!ret) break;
	}
	svcCloseHandle(saltysd);
	if (ret != 0x0) return false;
	else return true;
}

uint64_t TID;

bool CheckIfGameRunning() {
	pmdmntInitialize();
	pminfoInitialize();
	uint64_t PID = 0;
	
	pmdmntGetApplicationProcessId(&PID);
	pminfoGetProgramId(&TID, PID);
	pminfoExit();
	pmdmntExit();
	if (TID == 0x0) return false;
	else return true;
}

uint8_t flagcode;

void GetRunningFlag() {
	char handheld_flag[128];
	char docked_flag[128];
	snprintf(handheld_flag, sizeof handheld_flag, "%s%llx%s", "sdmc:/SaltySD/plugins/0", TID, "/ReverseNX/handheld.flag");
	snprintf(docked_flag, sizeof docked_flag, "%s%llx%s", "sdmc:/SaltySD/plugins/0", TID, "/ReverseNX/docked.flag");
	FILE* handheld_titleid_flag = fopen(handheld_flag, "r");
	FILE* docked_titleid_flag = fopen(docked_flag, "r");
	if (handheld_titleid_flag != NULL) flagcode = 0x1;
	else if (docked_titleid_flag != NULL) flagcode = 0x2;
	else flagcode = 0xFF;
	fclose(handheld_titleid_flag);
	fclose(docked_titleid_flag);
}

int main(int argc, char **argv)
{
	bool log = false;
	AppletType at = appletGetAppletType();
	consoleInit(NULL);
	DIR* flags_dir = opendir("sdmc:/SaltySD/flags");
	if (!flags_dir) {
		mkdir("sdmc:/SaltySD/flags", ACCESSPERMS);
	}
	else closedir(flags_dir);
	FILE* logflag = fopen("sdmc:/SaltySD/flags/log.flag", "r");
	if (logflag) {
		log = true;
		fclose(logflag);
	}
	DIR* flags_reversenx_dir = opendir("sdmc:/SaltySD/flags/ReverseNX");
	if (!flags_reversenx_dir) {
		mkdir("sdmc:/SaltySD/flags/ReverseNX", ACCESSPERMS);
	}
	else closedir(flags_reversenx_dir);
	DIR* reversenx_dir = opendir("sdmc:/SaltySD/plugins/ReverseNX");
	if (!reversenx_dir) {
		mkdir("sdmc:/SaltySD/plugins/ReverseNX", ACCESSPERMS);
	}
	else closedir(reversenx_dir);
	FILE *titleid = fopen("sdmc:/SaltySD/flags/ReverseNX/titleid.flag", "r");
	FILE *disable_flag = fopen("sdmc:/SaltySD/flags/disable.flag", "r");
	goto start;

start:
	if (disable_flag) {
		fclose(disable_flag);
		fclose(titleid);
		goto disabled;
	}
	else if (titleid) {
		fclose(disable_flag);
		fclose(titleid);
		goto titleid_1;
	}
	else goto global_1;
	
disable:
	consoleClear();
	goto disabled;

bool inj;
bool Running;

disabled:
	if (at != AppletType_Application && at != AppletType_SystemApplication) {
		inj = CheckPort();
		if (inj == true) printf("SaltyNX is injected properly.\n");
		else printf(CONSOLE_RED "SaltyNX is not injected!!\n");
	}
	else printf(CONSOLE_RED "Checking SaltyNX is not possible! Run homebrew in Applet Mode!\n");
	remove("sdmc:/SaltySD/FPSoffset.hex");
	printf("SaltyNX is disabled.\n\n");
	printf("To enable SaltyNX and loading ReverseNX, press A.\n");
	if (log == true) printf("Press ZL to disable log writing.\n");
	else printf("Press ZL to enable log writing.\n");
	printf("Press X to exit.\n");
	consoleUpdate(NULL);
	while(appletMainLoop())
	{	
		hidScanInput();
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		if (kDown & KEY_X) {
			goto close;
		}
		else if (kDown & KEY_ZL) {
			if (log == true) {
				remove("sdmc:/SaltySD/flags/log.flag");
				log = false;
				goto disable;
			}
			else {
				logflag = fopen("sdmc:/SaltySD/flags/log.flag", "w");
				fclose(logflag);
				log = true;
				goto disable;
			}
		}
		else if (kDown & KEY_A) {
			remove("sdmc:/SaltySD/flags/disable.flag");
			consoleClear();
			titleid = fopen("sdmc:/SaltySD/flags/ReverseNX/titleid.flag", "r");
			if (titleid) {
				fclose(titleid);
				goto titleid_1;
			}
			else {
				fclose(titleid);
				goto global_1;
			}
		}
	}
	goto close;
			
titleid_1:
	if (at != AppletType_Application && at != AppletType_SystemApplication) {
		inj = CheckPort();
		if (inj == false) printf(CONSOLE_RED "SaltyNX is not injected!!\n");
	}
	else printf(CONSOLE_RED "Checking SaltyNX is not possible! Run homebrew in Applet Mode!\n");
	Running = CheckIfGameRunning();
	if (Running == true) {
		GetRunningFlag();
		if (flagcode == 0xFF) printf("Flag of game in background: not set.\n");
		else if (flagcode == 0x1) printf("Flag of game in background: handheld.\n");
		else if (flagcode == 0x2) printf("Flag of game in background: docked.\n");
	}
		
	printf("Titleid mode set. Press + to change mode to global.\n\n");
	FILE *handheld_flag_titleid = fopen("sdmc:/SaltySD/flags/ReverseNX/createhandheld.flag", "r");
	FILE *docked_flag_titleid = fopen("sdmc:/SaltySD/flags/ReverseNX/createdock.flag", "r");
	FILE *remove_flag_titleid = fopen("sdmc:/SaltySD/flags/ReverseNX/createremove.flag", "r");
	u8 handheld_titleid = 0;
	u8 docked_titleid = 0;
	u8 remove_titleid = 0;

	if (docked_flag_titleid && handheld_flag_titleid) {
		printf("Detected both flags.\n");
		fclose(handheld_flag_titleid);
		fclose(docked_flag_titleid);
		remove("sdmc:/SaltySD/flags/ReverseNX/createdock.flag");
		remove("sdmc:/SaltySD/flags/ReverseNX/createhandheld.flag");
		printf("Flags deleted.\n");
	}
	else if (docked_flag_titleid) 
	{
		printf("Detected docked titleid flag.\n");
		docked_titleid = 1;
	}
	else if (handheld_flag_titleid) 
	{
		printf("Detected handheld titleid flag.\n");
		handheld_titleid = 1;
	}
	else if (remove_flag_titleid) 
	{
		printf("Detected reset settings.\n");
		remove_titleid = 1;
	}
	else printf("No change flag detected. ReverseNX-Tool disabled.\n");
	fclose(handheld_flag_titleid);
	fclose(docked_flag_titleid);
	fclose(remove_flag_titleid);
	printf("What titleid flag you want to set?\n\n");
	printf("It will be applied only to first game You will boot.\n\n");
	printf("Profiles:\t\t\t\tOptions:\n");	
	printf("A - Docked\t\t\t\tX - Exit\n");
	printf("B - Handheld\t\t\tZR - Disable SaltyNX\n");
	if (log == true) printf("Y - Reset settings\tZL - Disable log writing\n\n");
	else printf("Y - Reset settings\tZL - Enable log writing\n\n");	

	// Main loop
	while(appletMainLoop())
	{
		hidScanInput();
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		if (kDown & KEY_X) {
			break;
		}
		else if (kDown & KEY_ZR) {
			FILE* disable_flag = fopen("sdmc:/SaltySD/flags/disable.flag", "w");
			fclose(disable_flag);
			goto disable;
		}
		else if (kDown & KEY_ZL) {
			if (log == true) {
				remove("sdmc:/SaltySD/flags/log.flag");
				log = false;
				consoleClear();
				goto titleid_1;
			}
			else {
				logflag = fopen("sdmc:/SaltySD/flags/log.flag", "w");
				fclose(logflag);
				log = true;
				consoleClear();
				goto titleid_1;
			}
		}
		else if (kDown & KEY_A) {
			if (docked_titleid == 1) printf("You have already docked flag!\n");
			else if (docked_titleid == 0) {
				if (handheld_titleid == 1) remove("sdmc:/SaltySD/flags/ReverseNX/createhandheld.flag");
				if (remove_titleid == 1) remove("sdmc:/SaltySD/flags/ReverseNX/createremove.flag");
				FILE *docked_file = fopen("sdmc:/SaltySD/flags/ReverseNX/createdock.flag", "w");
				fclose(docked_file);
				docked_titleid = 1;
				handheld_titleid = 0;
				remove_titleid = 0;
				printf("Docked flag set.\n");
			}
		}
		else if (kDown & KEY_B) {
			if (handheld_titleid == 1) printf("You have already handheld flag!\n");
			else if (handheld_titleid == 0) {
				if (docked_titleid == 1) remove("sdmc:/SaltySD/flags/ReverseNX/createdock.flag");
				if (remove_titleid == 1) remove("sdmc:/SaltySD/flags/ReverseNX/createremove.flag");
				FILE *handheld_file = fopen("sdmc:/SaltySD/flags/ReverseNX/createhandheld.flag", "w");
				fclose(handheld_file);
				handheld_titleid = 1;
				docked_titleid = 0;
				remove_titleid = 0;
				printf("Handheld flag set.\n");
			}
		}
		else if (kDown & KEY_Y) {
			if ((handheld_titleid == 0) && (docked_titleid == 0) && (remove_titleid == 1)) printf("It's already set to be resetted!\n");
			else {
				if (docked_titleid == 1) remove("sdmc:/SaltySD/flags/ReverseNX/createdock.flag");
				if (handheld_titleid == 1) remove("sdmc:/SaltySD/flags/ReverseNX/createhandheld.flag");
				FILE *remove_file = fopen("sdmc:/SaltySD/flags/ReverseNX/createremove.flag", "w");
				fclose(remove_file);
				handheld_titleid = 0;
				docked_titleid = 0;
				remove_titleid = 1;
				printf("Reset flag set.\n");
			}
		}
		else if (kDown & KEY_PLUS) {
			if ((handheld_titleid != 0) || (docked_titleid != 0)) {
				if (docked_titleid == 1) remove("sdmc:/SaltySD/flags/ReverseNX/createdock.flag");
				if (handheld_titleid == 1) remove("sdmc:/SaltySD/flags/ReverseNX/createhandheld.flag");
				if (remove_titleid == 1) remove("sdmc:/SaltySD/flags/ReverseNX/createremove.flag");
			}
			fclose(titleid);
			remove("sdmc:/SaltySD/flags/ReverseNX/titleid.flag");
			consoleClear();
			goto global_1;
			}

		consoleUpdate(NULL);
		}
	goto close;

global_1:
	if (at != AppletType_Application && at != AppletType_SystemApplication) {
		inj = CheckPort();
		if (inj == false) printf(CONSOLE_RED "SaltyNX is not injected!!\n");
	}
	else printf(CONSOLE_RED "Checking SaltyNX is not possible! Run homebrew in Applet Mode!\n");
	printf("Global mode set. Press + to change mode to titleid.\n\n");
	FILE *handheld_flag_global = fopen("sdmc:/SaltySD/plugins/ReverseNX/handheld.flag", "r");
	FILE *docked_flag_global = fopen("sdmc:/SaltySD/plugins/ReverseNX/docked.flag", "r");
	u8 handheld = 0;
	u8 docked = 0;

	if (docked_flag_global && handheld_flag_global) {
		printf("Detected both flags.\n");
		fclose(handheld_flag_global);
		fclose(docked_flag_global);
		remove("sdmc:/SaltySD/plugins/ReverseNX/handheld.flag");
		remove("sdmc:/SaltySD/plugins/ReverseNX/docked.flag");
		printf("Flags deleted.\n");
	}
	else if (docked_flag_global) 
	{
		printf("Detected docked global flag.\n");
		docked = 1;
	}
	else if (handheld_flag_global) 
	{
		printf("Detected handheld global flag.\n");
		handheld = 1;
	}
	else printf("No global flag detected. ReverseNX disabled.\n");
	fclose(handheld_flag_global);
	fclose(docked_flag_global);
	printf("What global flag you want to set?\n\n");
	printf("Profiles:\t\t\t\tOptions:\n");	
	printf("A - Docked\t\t\t\tX - Exit\n");
	printf("B - Handheld\t\t\tZR - Disable SaltyNX\n");
	if (log == true) printf("\t\t\t\t\t\t\tZL - Disable log writing\n\n");
	else printf("\t\t\t\t\t\t\tZL - Enable log writing\n\n");

	// Main loop
	while(appletMainLoop())
	{
		hidScanInput();
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		if (kDown & KEY_X) {
			break;
		}
		else if (kDown & KEY_ZR) {
			FILE* disable_flag = fopen("sdmc:/SaltySD/flags/disable.flag", "w");
			fclose(disable_flag);
			goto disable;
		}
		else if (kDown & KEY_ZL) {
			if (log == true) {
				remove("sdmc:/SaltySD/flags/log.flag");
				log = false;
				consoleClear();
				goto global_1;
			}
			else {
				logflag = fopen("sdmc:/SaltySD/flags/log.flag", "w");
				fclose(logflag);
				log = true;
				consoleClear();
				goto global_1;
			}
		}
		else if (kDown & KEY_A) {
			if (docked == 1) printf("You have already docked flag!\n");
			else if (docked == 0) {
				if (handheld == 1) remove("sdmc:/SaltySD/plugins/ReverseNX/handheld.flag");
				FILE *docked_file = fopen("sdmc:/SaltySD/plugins/ReverseNX/docked.flag", "w");
				fclose(docked_file);
				docked = 1;
				handheld = 0;
				printf("Docked flag set.\n");
			}
		}
		else if (kDown & KEY_B) {
			if (handheld == 1) printf("You have already handheld flag!\n");
			else if (handheld == 0) {
				if (docked == 1) remove("sdmc:/SaltySD/plugins/ReverseNX/docked.flag");
				FILE *handheld_file = fopen("sdmc:/SaltySD/plugins/ReverseNX/handheld.flag", "w");
				fclose(handheld_file);
				handheld = 1;
				docked = 0;
				printf("Handheld flag set.\n");
			}
		}
		else if (kDown & KEY_PLUS) {
				fclose(titleid);
				titleid = fopen("sdmc:/SaltySD/flags/ReverseNX/titleid.flag", "w");
				fclose(titleid);
				consoleClear();
				goto titleid_1;
			}

		consoleUpdate(NULL);
		
	}
	goto close;

close:
	consoleExit(NULL);
	return 0;
}
