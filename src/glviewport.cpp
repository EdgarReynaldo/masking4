////////////////////////////////////////////////////////////////////////////////
//         MASkinG - Miran Amon's Skinnable GUI Library for Allegro           //
//    Copyright (c) 2002-2005 Miran Amon (miranamon@users.sourceforge.net)    //
//          Project websites: http://ferisrv5.uni-mb.si/~ma0747               //
//                  http://sourceforge.net/projects/masking                   //
////////////////////////////////////////////////////////////////////////////////

#ifdef		MASKING_GL

#include "../include/MASkinG/glviewport.h"
#include "../include/MASkinG/settings.h"
#include <alleggl.h>


void MAS::GLViewport::MsgDraw() {
	MAS::Bitmap viewport;	//null bitmap

	// First we get back into a 3D mode
	allegro_gl_unset_allegro_mode();

	// Save the Viewport and Scissor states
	glPushAttrib(GL_SCISSOR_BIT | GL_VIEWPORT_BIT);

	// Adapt the viewport to the object size and position
	glViewport(x(), SCREEN_H - y() - h(), w(), h());
	glScissor(x(), SCREEN_H - y() - h(), w(), h());
	glEnable(GL_SCISSOR_TEST);

	// clear the GL context
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw the contents of the vieport
	Draw(viewport);

	// restore previous state
	glPopAttrib();
	allegro_gl_set_allegro_mode();

	// copy the GL screen to the MASkinG canvas so it will actually be shown on screen
	// (only required with the DRS or double buffer drivers)
	if (MAS::Settings::screenUpdateMethod >= 0) {
		blit(screen, parent->GetDriver()->GetCanvas(), x(), y(), x(), y(), w(), h());
	}

	ClearFlag(D_DIRTY);
}


#endif		//MASKING_GL
