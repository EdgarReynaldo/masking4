////////////////////////////////////////////////////////////////////////////////
//         MASkinG - Miran Amon's Skinnable GUI Library for Allegro           //
//    Copyright (c) 2002-2005 Miran Amon (miranamon@users.sourceforge.net)    //
//          Project websites: http://ferisrv5.uni-mb.si/~ma0747               //
//                  http://sourceforge.net/projects/masking                   //
////////////////////////////////////////////////////////////////////////////////

#include "../include/MASkinG/doublebuffer.h"
#include "../include/MASkinG/settings.h"


MAS::Error MAS::DoubleBuffer::Create() {
	Destroy();

	if (MAS::Settings::useVideoMemory) {
		canvas = create_video_bitmap(SCREEN_W, SCREEN_H);
		if (!canvas) {
			canvas = create_bitmap(SCREEN_W, SCREEN_H);
			if (!canvas) {
				return MAS::Error(MAS::Error::MEMORY);
			}
		}
	}
	else {
		canvas = create_bitmap(SCREEN_W, SCREEN_H);
		if (!canvas) {
			return MAS::Error(MAS::Error::MEMORY);
		}
	}

	return MAS::Error(MAS::Error::NONE);
}


void MAS::DoubleBuffer::Destroy() {
	if (canvas) {
		destroy_bitmap(canvas);
		canvas = NULL;
	}
}


void MAS::DoubleBuffer::Redraw() {
	if (MAS::Settings::vsync) {
		vsync();
	}
	blit(canvas, ::screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}


bool MAS::DoubleBuffer::RequiresFullRedraw() {
	return true;
}


char *MAS::DoubleBuffer::GetDescription() {
	return "Double Buffering";
}
