////////////////////////////////////////////////////////////////////////////////
//         MASkinG - Miran Amon's Skinnable GUI Library for Allegro           //
//    Copyright (c) 2002-2005 Miran Amon (miranamon@users.sourceforge.net)    //
//          Project websites: http://ferisrv5.uni-mb.si/~ma0747               //
//                  http://sourceforge.net/projects/masking                   //
////////////////////////////////////////////////////////////////////////////////

#ifdef		MASKING_GL

#include "../include/MASkinG/gldriver.h"
#include "../include/MASkinG/settings.h"


std::vector<MAS::Rect> MAS::GLDriverDRS::mouse_back;


bool MAS::GLDriver::RequiresFullRedraw() {
	return true;
}


char *MAS::GLDriver::GetDescription() {
	return "default OpenGL";
}


void MAS::GLDriver::Redraw() {
	allegro_gl_flip();
}


void MAS::GLDriverDRS::AcquireCanvas() {
	DRS::AcquireCanvas();

	if (Settings::useOpenGLMouseHack) {
		while (!mouse_back.empty()) {
			Rect r = mouse_back.back();
			rectfill(canvas, r.x(), r.y(), r.x2(), r.y2(), makecol(255,0,255));
			mouse_back.pop_back();
		}
	}
}


void MAS::GLDriverDRS::Redraw() {
	/* a little hack to make drs work in fullscreen mode (page flipping): we redraw dirty regions
	   on both front and back buffers. There is no need for it in windowed mode (double buffering).
	*/
	if (!is_windowed_mode()) {
		for (std::list<Rect>::iterator r = rectlist.begin(); r != rectlist.end(); ++r)
			blit(canvas, ::screen, (*r).x(), (*r).y(), (*r).x(), (*r).y(), (*r).w(), (*r).h());
		allegro_gl_flip();

		DRS::Redraw();
	}
	else {
		DRS::Redraw();
		allegro_gl_flip();
	}
}


char *MAS::GLDriverDRS::GetDescription() {
	return "OpenGL with DRS";
}


void MAS::GLDriverDoubleBuffer::Redraw() {
	blit(canvas, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	allegro_gl_flip();
	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}


char *MAS::GLDriverDoubleBuffer::GetDescription() {
	return "software double buffered OpenGL";
}


bool MAS::GLDriverDoubleBuffer::RequiresFullRedraw() {
	return false;
}

#endif		//MASKING_GL
