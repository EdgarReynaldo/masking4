// example 19: Shows how to make a simple animated widget that runs on a timer.
// Just make it bigger and you have a nice intro animation for your game.

#include "../include/MASkinG.h"
using namespace MAS;

#define		TITLE_COUNT		3
#define		FADE_INTERVAL	3000

class FadeTitle : public Widget {
	private:
		Bitmap title[TITLE_COUNT];
		Bitmap buffer;
		int timerID;
		int state;
		int counter;

	protected:
		void MsgStart();
		void MsgEnd();
		void MsgTimer(int id);
		void MsgTick();
		void Draw(Bitmap &canvas);
		void MsgInitSkin();

	public:
		FadeTitle();
		~FadeTitle();
};


FadeTitle::FadeTitle() : Widget(), timerID(-1), state(0), counter(0) {
	Resize(160, 32);
	for (int i=0; i<TITLE_COUNT; i++) {
		title[i].Create(w(), h());
	}
	buffer.Create(w(), h());
}


FadeTitle::~FadeTitle() {
	for (int i=0; i<TITLE_COUNT; i++) {
		title[i].Destroy();
	}
	buffer.Destroy();
}


void FadeTitle::MsgInitSkin() {
	Widget::MsgInitSkin();

	Color bg[] = { Color(232,212,128), Color(128,192,128), Color(128,192,232) };
	Color fg[] = { Color(192,64,64), Color(255,255,255), Color(255,255,96) };
	Color shd[] = { Color(212,192,128), Color(128,128,128), Color(128,128,128) };

	char *text[] = { "Example no. 19", "MASkinG GUI library", "by Miran Amon" };
	MAS::Font fnt = skin->GetFont(0);

	for (int i=0; i<TITLE_COUNT; i++) {
		title[i].Clear(bg[i]);
		fnt.GUITextout(title[i], text[i], w()/2, (h() - fnt.TextHeight())/2, fg[i], shd[i], -1, 2);
	}

	buffer.Clear(Color(192,192,192));
}


void FadeTitle::MsgStart() {
	Widget::MsgStart();
	timerID = Timer::Install(FADE_INTERVAL);
}


void FadeTitle::MsgEnd() {
	Widget::MsgEnd();
	Timer::Kill(timerID);
	timerID = -1;
}


void FadeTitle::MsgTick() {
	Widget::MsgTick();
	if (counter < 256) {
		++counter;
		Redraw();
	}
}


void FadeTitle::MsgTimer(int id) {
	Widget::MsgTimer(id);

	if (id == timerID) {
		++state;
		state %= TITLE_COUNT;
		counter = 0;
	}
}


void FadeTitle::Draw(Bitmap &canvas) {
	set_trans_blender(0, 0, 0, counter);
	buffer.DrawTransSprite(title[state], 0, 0);
	buffer.Blit(canvas, 0, 0, 0, 0, w(), h());
}


class MyDialog : public Dialog {
	private:
		ClearScreen desktop;
		FadeTitle title;

	public:
		MyDialog();
};


MyDialog::MyDialog() : Dialog() {
	title.Place((SCREEN_W - title.w())/2, (SCREEN_H - title.h())/2);

	Add(desktop);
	Add(title);
}


int main() {
	// install everything
	Error err = InstallMASkinG("allegro.cfg");
	if (err) {
		err.Report();
	}

	// make a dialog and execute it
	MyDialog *dlg = new MyDialog;
	dlg->Execute();
	delete dlg;

	// clean up
	ExitMASkinG();
	return 0;
}
END_OF_MAIN();
