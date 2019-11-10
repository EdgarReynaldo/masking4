// example 10: Shows how to animate a dialog by dynamically adding and removing widgets.

#include "../include/MASkinG.h"
using namespace MAS;


#define		BUTTON_COUNT		8
#define		BUTTON_BITMAP		Skin::nBitmaps+1
#define		BUTTON_MESSAGE		1000
#define		ANIMATION_INTERVAL	50

class MyDialog : public Dialog {
	private:
		ClearScreen desktop;
		Button button[BUTTON_COUNT];

		int timerID;
		int counter;
		int animation;

	protected:
		void MsgStart();
		void MsgTimer(int id);

	public:
		MyDialog();
		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0);
};


MyDialog::MyDialog() : Dialog(), timerID(-1), counter(0), animation(0) {
	desktop.SetColor(Color(192,192,192));
	Add(desktop);

	int w = 120;
	int h = 24;
	int x = (SCREEN_W - w)/2;
	int dy = 32;
	int y = (SCREEN_H - dy*BUTTON_COUNT)/2;

	char fullPath[1024];
	MakeFullPath(fullPath, "button.tga");
	skin->LoadBitmap(fullPath, BUTTON_BITMAP);
	for (int i=0; i<BUTTON_COUNT; i++) {
		button[i].Setup(x, y + dy*i, w, h, 0, 0, BUTTON_BITMAP);
		button[i].SetCallbackID(BUTTON_MESSAGE);
	}
}


void MyDialog::MsgStart() {
	Dialog::MsgStart();
	timerID = Timer::Install(ANIMATION_INTERVAL);
	counter = 0;
	animation = 1;
}


void MyDialog::MsgTimer(int id) {
	Dialog::MsgTimer(id);

	if (id == timerID) {
		switch (animation) {
			case 1:
				Add(button[counter]);
				++counter;

				if (counter == BUTTON_COUNT) {
					Timer::Kill(timerID);
					timerID = -1;
					animation = 0;
				}
			break;

			case 2:
				--counter;
				Remove(button[counter]);

				if (counter < 0) {
					Timer::Kill(timerID);
					timerID = -1;
					counter = 0;
					Close();
					animation = 0;
				}
			break;
		}
	}
}


void MyDialog::HandleEvent(Widget &obj, int msg, int arg1, int arg2) {
	Dialog::HandleEvent(obj, msg, arg1, arg2);

	if (msg == BUTTON_MESSAGE) {
		timerID = Timer::Install(ANIMATION_INTERVAL);
		animation = 2;
	}
}


int main() {
	// install everything
	Error err = InstallMASkinG("allegro.cfg");
	if (err) {
		err.Report();
	}

	// make a dialog and execute it
	MyDialog *dlg = new MyDialog;
	rest(500);
	dlg->Execute();
	delete dlg;

	// clean up
	ExitMASkinG();
	return 0;
}
END_OF_MAIN();
