////////////////////////////////////////////////////////////////////////////////
//         MASkinG - Miran Amon's Skinnable GUI Library for Allegro           //
//    Copyright (c) 2002-2005 Miran Amon (miranamon@users.sourceforge.net)    //
//          Project websites: http://ferisrv5.uni-mb.si/~ma0747               //
//                  http://sourceforge.net/projects/masking                   //
////////////////////////////////////////////////////////////////////////////////

#include "../include/MASkinG/fps.h"
#include "../include/MASkinG/fpslabel.h"
#include "../include/MASkinG/settings.h"


MAS::FPS::FPS()
	:MAS::Label()
{
	ClearFlag(D_AUTOSIZE);
}


MAS::FPS::~FPS() {
}


void MAS::FPS::MsgTick() {
	Label::MsgTick();
	Redraw();
}



void MAS::FPS::Draw(Bitmap &canvas) {
	//TODO
	SetNumber(globalFpsCounter.Get());
	Label::Draw(canvas);
}


void MAS::FPS::MsgInitSkin() {
	SetNumber(80000);
	if (GetTextMode() == -1) SetTextMode(skin->c_back);
	Label::MsgInitSkin();
}


int MAS::FPS::Get() {
	return globalFpsCounter.Get();
}
