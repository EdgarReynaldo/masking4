////////////////////////////////////////////////////////////////////////////////
//         MASkinG - Miran Amon's Skinnable GUI Library for Allegro           //
//    Copyright (c) 2002-2005 Miran Amon (miranamon@users.sourceforge.net)    //
//          Project websites: http://ferisrv5.uni-mb.si/~ma0747               //
//                  http://sourceforge.net/projects/masking                   //
////////////////////////////////////////////////////////////////////////////////

#include "../include/MASkinG/triplebuffer.h"

MAS::TripleBuffer::TripleBuffer()
	:PageFlipping(),
	page3(NULL),
	page(0)
{
}


MAS::Error MAS::TripleBuffer::Create() {
	Destroy();

	// try to enable triple buffering
	if (!(gfx_capabilities & GFX_CAN_TRIPLE_BUFFER)) {
		enable_triple_buffer();
	}

	if (!(gfx_capabilities & GFX_CAN_TRIPLE_BUFFER)) {
		return MAS::Error(MAS::Error::TRIPLEBUFFER);
	}

	// create the pages
	page1 = create_video_bitmap(SCREEN_W, SCREEN_H);
	page2 = create_video_bitmap(SCREEN_W, SCREEN_H);
	page3 = create_video_bitmap(SCREEN_W, SCREEN_H);

	if (!page1 || !page2 || !page3) {
		Destroy();
		return MAS::Error(MAS::Error::VIDEOMEMORY);
	}

	activePage = page1;
	page = 0;
	show_video_bitmap(activePage);
	Redraw();

	return MAS::Error(MAS::Error::NONE);
}


void MAS::TripleBuffer::Destroy() {
	PageFlipping::Destroy();

	if (page3) {
		destroy_bitmap(page3);
		page3 = NULL;
	}

	page = 0;
}


void MAS::TripleBuffer::Redraw() {
	do {
	} while (poll_scroll());

	request_video_bitmap(activePage);

	switch (page) {
		case 0:  page = 1;  activePage = page2;  break;
		case 1:  page = 2;  activePage = page3;  break;
		case 2:  page = 0;  activePage = page1;  break;
	}
}


BITMAP *MAS::TripleBuffer::GetCanvas() {
	return activePage;
}


char *MAS::TripleBuffer::GetDescription() {
	return "Triple Buferring";
}
