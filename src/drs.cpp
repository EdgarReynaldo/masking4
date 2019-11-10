////////////////////////////////////////////////////////////////////////////////
//         MASkinG - Miran Amon's Skinnable GUI Library for Allegro           //
//    Copyright (c) 2002-2005 Miran Amon (miranamon@users.sourceforge.net)    //
//          Project websites: http://ferisrv5.uni-mb.si/~ma0747               //
//                  http://sourceforge.net/projects/masking                   //
////////////////////////////////////////////////////////////////////////////////

#include "../include/MASkinG/drs.h"
#include "../include/MASkinG/defines.h"
#include "../include/MASkinG/global.h"
#include "../include/MASkinG/settings.h"


void MAS::DRS::Redraw() {
	acquire_screen();
	while (!rectlist.empty()) {
		Rect r = rectlist.back();
		blit(canvas, ::screen, r.x(), r.y(), r.x(), r.y(), r.w(), r.h());
		rectlist.pop_back();
	}
	release_screen();
}


////////////////////////////////////////////////////////////////////////////////
// Check if a rectangle can be safely added to the DRS list
MAS::Rect *MAS::DRS::CanAdd(const MAS::Rect& r) {
	// See if there is a rectangle in the way
	MAS::Rect topLeft(r.x2() - MAXINT, r.y2() - MAXINT, MAXINT, MAXINT);
	MAS::Rect bottomRight(r.x(), r.y(), MAXINT, MAXINT);
	MAS::Point topLeftS, bottomRightS;
	for (std::list<MAS::Rect>::iterator i = rectlist.begin(); i != rectlist.end(); ++i) {
		if (i->topLeft() < topLeft && i->bottomRight() < bottomRight) {
			// found one!
			return &*i;
		}
	}

	return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// Adds a rectangle into the dirty list. If it can't be added it is split in
// 4 parts which are then added recursively
void MAS::DRS::InvalidateRect(const Rect& r) {
	MAS::Rect *s = CanAdd(r);
	if (!s) {
		rectlist.push_back(r);
	}
	else {
		int x1 = r.x();
		int x2 = r.x2();
		int y1 = r.y();
		int y2 = r.y2();

		// Get the coordiantes of the rectangle that is in the way
		int wx1 = s->x();
		int wy1 = s->y();
		int wx2 = s->x2();
		int wy2 = s->y2();

		// Now check which corner points of the rect we're trying to add are
		// inside the rect that is on top and split the rect accordingly
		int inside = 0;

		inside |= (((MAS::Point(x1, y1) <= *s) ? 1 : 0)<<0);		// upper-left
		inside |= (((MAS::Point(x2, y1) <= *s) ? 1 : 0)<<1);		// upper-right
		inside |= (((MAS::Point(x1, y2) <= *s) ? 1 : 0)<<2);		// lower-left
		inside |= (((MAS::Point(x2, y2) <= *s) ? 1 : 0)<<3);		// lower right

		switch (inside) {
			// no corners are inside the window (the window is inside the rectangle) -->
			// --> split the rectangle in 3 or 4 pieces
			case 0x00:
				if (wy1 > y1)	InvalidateRect(MAS::Rect(x1, y1, r.w(), wy1-y1));
				if (wx1 > x1)	InvalidateRect(MAS::Rect(x1, MAX(y1, wy1), wx1-x1, MIN(y2, wy2) - MAX(y1, wy1)));
				if (wx2 < x2)	InvalidateRect(MAS::Rect(wx2, MAX(y1, wy1), x2-wx2, MIN(y2, wy2) - MAX(y1, wy1)));
				if (wy2 < y2)	InvalidateRect(MAS::Rect(x1, wy2, r.w(), y2-wy2));
				break;
			// only the top left is inside --> split in 2 rectangles
			case 0x01:
				InvalidateRect(MAS::Rect(wx2, y1, x2-wx2, wy2-y1));
				InvalidateRect(MAS::Rect(x1, wy2, r.w(), y2-wy2));
				break;
			// only the top right is inside --> 2 rectangles
			case 0x02:
				InvalidateRect(MAS::Rect(x1, y1, wx1-x1, wy2-y1));
				InvalidateRect(MAS::Rect(x1, wy2, r.w(), y2-wy2));
				break;
			// both top corners are inside --> add the bottom half
			case 0x03:
				InvalidateRect(MAS::Rect(x1, wy2, r.w(), y2-wy2));
				break;
			// only the lower left is inside --> 2 rectangles
			case 0x04:
				InvalidateRect(MAS::Rect(x1, y1, r.w(), wy1-y1));
				InvalidateRect(MAS::Rect(wx2, wy1, x2-wx2, y2-wy1));
				break;
			// both left corners are inside --> add the right half
			case 0x05:
				InvalidateRect(MAS::Rect(wx2, y1, x2-wx2, r.h()));
				break;
			// only the lower right is inside --> 2 rectangles
			case 0x08:
				InvalidateRect(MAS::Rect(x1, y1, r.w(), wy1-y1));
				InvalidateRect(MAS::Rect(x1, wy1, wx1-x1, y2-wy1));
				break;
			// both right corners are inside --> add the left half
			case 0x0A:
				InvalidateRect(MAS::Rect(x1, y1, wx1-x1, r.h()));
				break;
			// both lower corners are inside --> add the top half
			case 0x0C:
				InvalidateRect(MAS::Rect(x1, y1, r.w(), wy1-y1));
				break;
		};
	}
}


bool MAS::DRS::RequiresFullRedraw() {
	return false;
}


char *MAS::DRS::GetDescription() {
	return "Dirty Rectangle System";
}
