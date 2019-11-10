////////////////////////////////////////////////////////////////////////////////
//         MASkinG - Miran Amon's Skinnable GUI Library for Allegro           //
//    Copyright (c) 2002-2005 Miran Amon (miranamon@users.sourceforge.net)    //
//          Project websites: http://ferisrv5.uni-mb.si/~ma0747               //
//                  http://sourceforge.net/projects/masking                   //
////////////////////////////////////////////////////////////////////////////////

#include "../include/MASkinG/mouse.h"
#include "../include/MASkinG/settings.h"
#include "../include/MASkinG/size.h"
#include "../include/MASkinG/gldriver.h"


MAS::Cursor *MAS::Mouse::globalCursor = NULL;

MAS::Mouse::Mouse()
	:parent(NULL),
	hidden(true),
	pos(-10000,-10000),
	flags(0),
	z(0)
{
}


MAS::Mouse::~Mouse() {
	Hide();
	Destroy();
}


void MAS::Mouse::Destroy() {
	back.Destroy();
}


void MAS::Mouse::CalculateGeometry() {
	// calculate position of the sprite
	spritePos.x(pos.x() - cur->focus.x());
	spritePos.y(pos.y() - cur->focus.y());

	// calculate position of the shadow
	shadowPos.x(spritePos.x() + cur->shadowOffset.x() - cur->PADDING);
	shadowPos.y(spritePos.y() + cur->shadowOffset.y() - cur->PADDING);

	// calculate position of the background buffer
	backPos.x(MIN(spritePos.x(), shadowPos.x()));
	backPos.y(MIN(spritePos.y(), shadowPos.y()));
}


void MAS::Mouse::SetCursor(Cursor &cur) {
	// remember if the mouse was on screen
	bool show = !hidden;

	// hide the mouse if necessary
	if (!hidden) {
		Hide();
	}

	// set the pointer
	this->cur = &cur;

	// destroy the old background if necessary
	back.Destroy();

	// calculate positions of the sprite, shadow and buffer
	CalculateGeometry();

	// calculate the new size of the background buffer
	Point spritePos2(spritePos.x() + cur.Sprite().w(), spritePos.y() + cur.Sprite().h());
	Point shadowPos2(shadowPos.x() + cur.Sprite().w() + 2*cur.PADDING, shadowPos.y() + cur.Sprite().h() + 2*cur.PADDING);
	Size backSize(MAX(spritePos2.x(), shadowPos2.x()) - backPos.x(), MAX(spritePos2.y(), shadowPos2.y()) - backPos.y());

	// create a new background buffer
	back.Create(backSize);

	// paint the new background buffer
	Bitmap screen = parent->GetCanvas();
	screen.Blit(back, backPos, Point(0, 0), backSize);

	// show the mouse again if necessary
	if (show) {
		Show();
	}
}


MAS::Cursor* MAS::Mouse::GetCursor() {
	return cur;
}


void MAS::Mouse::Show() {
	if (!MAS::Settings::showMouse || !parent) return;

	Bitmap canvas = parent->GetCanvas();

	// save the background
	CalculateGeometry();
	if (!parent->RequiresFullRedraw()) {
		canvas.Blit(back, backPos, Point(0, 0), back.size());
	}

#ifdef MASKING_GL
	if (Settings::useOpenGLMouseHack) {
		// this is a dirty little hack :o
		BITMAP *tmp = create_bitmap(back.w(), back.h());
		blit(::screen, tmp, backPos.x(), backPos.y(), 0, 0, back.w(), back.h());
		masked_blit(canvas, tmp, backPos.x(), backPos.y(), 0, 0, back.w(), back.h());
		blit(tmp, ::screen, 0, 0, backPos.x(), backPos.y(), back.w(), back.h());
		blit(::screen, canvas, backPos.x(), backPos.y(), backPos.x(), backPos.y(), back.w(), back.h());
		destroy_bitmap(tmp);
	}
#endif

	// draw shadow
	if (MAS::Settings::mouseShadow) {
		set_alpha_blender();
		canvas.DrawTransSprite(cur->Shadow(), shadowPos);
	}

	// draw sprite
	canvas.DrawSprite(cur->Sprite(), spritePos);

	// invalidate rectangle
	parent->InvalidateRect(Rect(backPos, back.size()));

	hidden = false;
}


void MAS::Mouse::Hide() {
	if (!MAS::Settings::showMouse || !parent || Settings::useOpenGLMouseHack) return;

	Bitmap canvas = parent->GetCanvas();

	// restore background
	if (!parent->RequiresFullRedraw()) {
		back.Blit(canvas, Point(0, 0), backPos, back.size());
	}

	// invalidate rectangle
	parent->InvalidateRect(Rect(backPos, back.size()));

	hidden = true;
}


void MAS::Mouse::UpdateState() {
	// get the new position of the mouse
	pos = Point(mouse_x, mouse_y);

	// read the flags
	flags = mouse_b;

	// read the position f the wheel
	z = mouse_z;
}

void MAS::Mouse::Update() {
	// update cursor animation
	cur->Update();

	// get the new position, etc.
	Point oldPos = pos;
	UpdateState();

	// update positions of the sprite, shadow and backbuffer
	Point d = oldPos - pos;
	spritePos -= d;
	shadowPos -= d;

#ifdef MASKING_GL
	if (back && Settings::useOpenGLMouseHack) {
		CalculateGeometry();
		GLDriverDRS::mouse_back.push_back(Rect(backPos, back.size()));
	}
#endif
}


void MAS::Mouse::SetParent(ScreenUpdate *p) {
	parent = p;
}
