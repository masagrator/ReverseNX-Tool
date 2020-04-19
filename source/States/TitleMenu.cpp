#include "TitleMenu.h"
#include <inttypes.h>
#include <sys/stat.h>
#include <dirent.h>

void ReverseNXMenu::Update(StateMachine &stateMachine, u64 kDown)
{
    if (kDown & KEY_UP)
        selection--;

    if (kDown & KEY_DOWN)
        selection++;

    if (kDown & KEY_LEFT)
        titlePage--;

    if (kDown & KEY_RIGHT)
        titlePage++;

    //Underflow/Overflow check
    int start = titlePage * max_title_items;
    int end = std::min((int)(Utils::titles.size()), start + max_title_items);
    onScreenItems = end - start;
    if (selection < 0)
        selection = onScreenItems - 1;
    if (onScreenItems - 1 < selection)
        selection = 0;

    if (titlePage < 0)
        titlePage = Utils::maxTitlePages;
    if (Utils::maxTitlePages < titlePage)
        titlePage = 0;

    if (kDown & KEY_A)
    {
		char Flag[128];
		
		DIR* patches_dir = opendir("sdmc:/SaltySD/patches");
		if (!patches_dir) mkdir("sdmc:/SaltySD/patches", 777);
		else closedir(patches_dir);
		Utils::gameSelected = (titlePage * max_title_items) + selection;
		
		if (Utils::titles.at(Utils::gameSelected).TitleID == 0x0F0F0F0F0F0F0F0F) {
			for (int i = 0; i < 2; ++i) {
				snprintf(Flag, sizeof Flag, "sdmc:/SaltySD/patches/%s", Utils::Files[i]);
				
				if (Utils::titles.at(Utils::gameSelected).ReverseNXflag >=2) {
					FILE* Mode = fopen(Flag, "w");
					fwrite(&Utils::Handheld, 1, 16, Mode);
					fclose(Mode);
				}
				else if (Utils::titles.at(Utils::gameSelected).ReverseNXflag == 0){
					FILE* Mode = fopen(Flag, "w");
					fwrite(&Utils::Docked, 1, 16, Mode);
					fclose(Mode);
				}
				else if (Utils::titles.at(Utils::gameSelected).ReverseNXflag == 1) remove(Flag);
			}
		}
		else {
			snprintf(Flag, sizeof Flag, "sdmc:/SaltySD/patches/%016" PRIx64, Utils::titles.at(Utils::gameSelected).TitleID);
			patches_dir = opendir(Flag);
			if (!patches_dir) mkdir(Flag, 777);
			else closedir(patches_dir);
			for (int i = 0; i < 2; ++i) {
				snprintf(Flag, sizeof Flag, "sdmc:/SaltySD/patches/%016" PRIx64 "/%s", Utils::titles.at(Utils::gameSelected).TitleID, Utils::Files[i]);
				
				if (Utils::titles.at(Utils::gameSelected).ReverseNXflag >= 2) {
					FILE* Mode = fopen(Flag, "w");
					fwrite(&Utils::Handheld, 1, 16, Mode);
					fclose(Mode);
				}
				else if (Utils::titles.at(Utils::gameSelected).ReverseNXflag == 0){
					FILE* Mode = fopen(Flag, "w");
					fwrite(&Utils::Docked, 1, 16, Mode);
					fclose(Mode);
				}
				else if (Utils::titles.at(Utils::gameSelected).ReverseNXflag == 1) remove(Flag);
			}
		}
		Utils::titles = Utils::getAllTitles();
		stateMachine.PushState("title");
		return;
    }

    Utils::printTitles(selection, titlePage, onScreenItems);

    if (kDown & KEY_B)
    {
        stateMachine.PushState("main");
        return;
    }
}