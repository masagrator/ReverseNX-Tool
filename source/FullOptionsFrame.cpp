#include "FullOptionsFrame.hpp"

FullOptionsFrame::FullOptionsFrame() : AppletFrame(true, true) {		
	this->setTitle("ReverseNX-Tool");
	this->setIcon(BOREALIS_ASSET("icon.jpg"));
	
	brls::List* FullOptionsList = new brls::List();
	
	brls::Label* Warning2 = new brls::Label(brls::LabelStyle::DESCRIPTION, "Any game on this list that is 32-bit or exists in exceptions list will be ignored by SaltyNX.", true);
	FullOptionsList->addView(Warning2);
	
	bool memorySafety2 = false;
	if (countGames > 56 && isAlbum) memorySafety2 = true;
	for (uint32_t i = 0; i < countGames; i++) {
		brls::SelectListItem* StatusItem2 = new brls::SelectListItem(titles.at(i).TitleName.c_str(), { "Handheld", "Docked", "System" }, (unsigned)titles.at(i).ReverseNX);
			
		if (memorySafety2 == false) StatusItem2->setThumbnail(titles.at(i).icon, sizeof(titles.at(i).icon));
		double textLength = (double)titles.at(i).TitleName.size();
		if (textLength >= 43) StatusItem2->setTextSize((int)((22 / pow((textLength/43), 0.212))));
				
		StatusItem2->getValueSelectedEvent()->subscribe([i](size_t selection) {
			Flag changeFlag = (Flag)selection;
			setReverseNX(titles.at(i).TitleID, changeFlag, true);
			setReverseNX(titles.at(i).TitleID, changeFlag, false);
		});
				
		FullOptionsList->addView(StatusItem2);
	}
			
	FullOptionsList->registerAction("Show tab", brls::Key::R, [] {
		brls::Application::popView();
		return true;
	});
			
	if (memorySafety2 && !memorySafety) brls::Application::notify("Disabled icons to prevent memory overflow.");
	this->setContentView(FullOptionsList);
}
