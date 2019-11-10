////////////////////////////////////////////////////////////////////////////////
//         MASkinG - Miran Amon's Skinnable GUI Library for Allegro           //
//    Copyright (c) 2002-2005 Miran Amon (miranamon@users.sourceforge.net)    //
//          Project websites: http://ferisrv5.uni-mb.si/~ma0747               //
//                  http://sourceforge.net/projects/masking                   //
////////////////////////////////////////////////////////////////////////////////

#include "../include/MASkinG/pageflipping.h"

MAS::PageFlipping::PageFlipping()
	:ScreenUpdate(),
	page1(NULL),
	page2(NULL),
	activePage(NULL)
{
}


MAS::Error MAS::PageFlipping::Create() {
	Destroy();
	page1 = create_video_bitmap(SCREEN_W, SCREEN_H);
	page2 = create_video_bitmap(SCREEN_W, SCREEN_H);

	if ((!page1) || (!page2)) {
		Destroy();
		return MAS::Error(MAS::Error::VIDEOMEMORY);
	}
	activePage = page1;
	//show_video_bitmap(page2);

	return Error(Error::NONE);
}


void MAS::PageFlipping::Destroy() {
	if (page1) {
		//destroy_bitmap(page1);
		::screen = page1;
		page1 = NULL;
	}

	if (page2) {
		destroy_bitmap(page2);
		page2 = NULL;
	}

	activePage = NULL;
}


void MAS::PageFlipping::Redraw() {
	//blit(page1, ::screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	show_video_bitmap(activePage);
	activePage = activePage == page1 ? page2 : page1;
}


BITMAP *MAS::PageFlipping::GetCanvas() {
	return activePage;
}


void MAS::PageFlipping::AcquireCanvas() {
	acquire_bitmap(activePage);
}


void MAS::PageFlipping::ReleaseCanvas() {
	release_bitmap(activePage);
}


bool MAS::PageFlipping::RequiresFullRedraw() {
	return true;
}


char *MAS::PageFlipping::GetDescription() {
	return "Page Flipping";
}
