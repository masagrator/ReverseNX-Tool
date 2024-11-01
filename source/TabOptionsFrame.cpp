#include "TabOptionsFrame.hpp"

TabOptionsFrame::TabOptionsFrame() : TabFrame() {		
		// Create a sample view
		this->setTitle("ReverseNX-Tool");
		this->setIcon(BOREALIS_ASSET("icon.jpg"));
		
		brls::List* OptionsList = new brls::List();
		
		brls::Label* Warning = new brls::Label(brls::LabelStyle::DESCRIPTION, "Any game on this list that is in exceptions list will be ignored by SaltyNX.", true);
		OptionsList->addView(Warning);

		//List all titles with flags
		countGames = static_cast<uint32_t>(titles.size());
		if (countGames > 160 && isAlbum) memorySafety = true;
		for (uint32_t i = 0; i < countGames; i++) {
			brls::SelectListItem* StatusItem = new brls::SelectListItem(titles.at(i).TitleName.c_str(), { "Handheld", "Docked", "System" }, (unsigned)titles.at(i).ReverseNX);
			
			double textLength = (double)titles.at(i).TitleName.size();
			if (isAllUpper(titles.at(i).TitleName.c_str()) && textLength >= 33) StatusItem->setTextSize((int)((19 / (pow(pow((textLength/33), (33/textLength)), 1.55)-0.06))));
			else {
				switch((int)textLength) case 33 ... 42: StatusItem->setTextSize(22);
				if (textLength >= 43) StatusItem->setTextSize((int)((22 / (pow(pow((textLength/43), (43/textLength)), 2)-0.01))));
			}
			
			if (memorySafety == false) StatusItem->setThumbnail(titles.at(i).icon, sizeof(titles.at(i).icon));
			
			StatusItem->getValueSelectedEvent()->subscribe([i](size_t selection) {
				changeFlag = (Flag)selection;
				setReverseNX(titles.at(i).TitleID, changeFlag, true);
				setReverseNX(titles.at(i).TitleID, changeFlag, false);
			});
			
			OptionsList->addView(StatusItem);
		}
		if (memorySafety == true) brls::Application::notify("Disabled icons to prevent memory overflow.");
		
		//Add option to hide tab using cursed method
		OptionsList->registerAction("Hide tab", brls::Key::L, [] {
			brls::Application::pushView(new FullOptionsFrame());
			return true;
		});
		
		this->addTab("Games", OptionsList);
		
		//Settings
		brls::List* SettingsList = new brls::List();
		
		brls::SelectListItem* SettingItem = new brls::SelectListItem("Enforce mode globally", { "Handheld", "Docked", "Disabled" }, (unsigned)getReverseNX(UINT64_MAX), "Option to force all games set to System in Games tab to run in one mode");
		SettingItem->getValueSelectedEvent()->subscribe([](size_t selection) {
				Flag changeFlag = (Flag)selection;
				setReverseNX(UINT64_MAX, changeFlag, true);
				setReverseNX(UINT64_MAX, changeFlag, false);
		});
		
		SettingsList->addView(SettingItem);
		
		this->addTab("Settings", SettingsList);
		
		//About (check About_tab.cpp)
		this->addTab("About", new AboutTab());
}
