#include <string.h>
#include <stdio.h>
#include <dirent.h>

#include <switch.h>

int main(int argc, char **argv)
{
    consoleInit(NULL);
	FILE *titleid = fopen("sdmc:/SaltySD/flags/ReverseNX/titleid.flag", "r");
	int loops = 0;
	
	if (titleid) {
		printf("Detected titleid.flag. GUI doesn't support titleid mode for now.\n");
		printf("Do you want to delete titleid.flag?\n");
		printf("A - Yes | B - No\n");
		fclose(titleid);
		while(appletMainLoop())
		{
			hidScanInput();
			u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
			if (kDown & KEY_B) 
			{
				consoleExit(NULL);
				return 0;
			}
			else if (kDown & KEY_A)
			{
				int status;
				status = remove("sdmc:/SaltySD/flags/ReverseNX/titleid.flag");
					if (status == 0) {
						printf("\n-------------\n\ntitleid.flag file deleted successfully.\n");
						break;
					}
					else {
						while(1)
						{
							printf("Unable to delete the file.\n");
							printf("Press A to exit.\n");
							if (loops == 0) consoleUpdate(NULL);
							loops = 1;
							if (kDown & KEY_A) return 0;
						}
					}
			}
			consoleUpdate(NULL);
		}
	}
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
	else printf("No global flag detected.\n");
	fclose(handheld_flag_global);
	fclose(docked_flag_global);
	printf("What global flag you want to set? X - Exit\n");
	printf("A - Docked, B - Handheld.\n\n");

    // Main loop
    while(appletMainLoop())
    {
		hidScanInput();
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        if (kDown & KEY_X) break;
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

        consoleUpdate(NULL);
    }

    consoleExit(NULL);
    return 0;
}
