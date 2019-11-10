////////////////////////////////////////////////////////////////////////////////
//         MASkinG - Miran Amon's Skinnable GUI Library for Allegro           //
//    Copyright (c) 2002-2005 Miran Amon (miranamon@users.sourceforge.net)    //
//          Project websites: http://ferisrv5.uni-mb.si/~ma0747               //
//                  http://sourceforge.net/projects/masking                   //
////////////////////////////////////////////////////////////////////////////////

#include "../include/MASkinG/screenupdate.h"

MAS::ScreenUpdate::ScreenUpdate() : canvas(NULL) {
}


MAS::ScreenUpdate::~ScreenUpdate() {
	Destroy();
}


MAS::Error MAS::ScreenUpdate::Create() {
	Destroy();
	canvas = create_sub_bitmap(::screen, 0, 0, SCREEN_W, SCREEN_H);
	if (!canvas) {
		return MAS::Error(MAS::Error::MEMORY);
	}
	return MAS::Error(MAS::Error::NONE);
}


void MAS::ScreenUpdate::Destroy() {
	if (canvas) {
		destroy_bitmap(canvas);
		canvas = NULL;
	}
}


void MAS::ScreenUpdate::Redraw() {
}


BITMAP *MAS::ScreenUpdate::GetCanvas() {
	return canvas;
}


void MAS::ScreenUpdate::AcquireCanvas() {
	acquire_bitmap(canvas);
}


void MAS::ScreenUpdate::ReleaseCanvas() {
	release_bitmap(canvas);
}


char *MAS::ScreenUpdate::GetDescription() {
	return "default Allegro update driver";
}


bool MAS::ScreenUpdate::RequiresFullRedraw() {
	return false;
}


void MAS::ScreenUpdate::InvalidateRect(const Rect &r) {
}


void MAS::ScreenUpdate::InvalidateRect(int x, int y, int w, int h) {
	InvalidateRect(Rect(x, y, w, h));
}
