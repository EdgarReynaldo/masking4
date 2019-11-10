// example 20: Shows how the Wallpaper class is used.


#include "../include/MASkinG.h"
using namespace MAS;


// A simple dialog: a wallpaper at the bottom and a small panel with some
// buttons on top. One button is for loading a custom wallpaper, the other
// are radio buttons for selecting the wallpaper style. And there's the
// obligatory exit button.
class MyDialog : public Dialog {
	private:
		// the wallpaper
		Wallpaper desktop;

		// a small bitmap
		Panel panel;

		// the load button
		Button bLoad;

		// the radio buttons for selecting style - Wallpaper supports 5
		// different styles
		Label lStyle;
		RadioButton radio[5];

		// the exit button
		Button bExit;

	protected:
		// called when the load button is clicked
		void OnLoad();

		// called when one of the radio buttons is selected
		void OnStyle(int i);

	public:
		// constructor
		MyDialog();

		// event handler
		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0);
};


// construct the dialog
MyDialog::MyDialog() : Dialog() {
	// No need to setup the wallpaper, just add it to the dialog. It will be
	// automatically resized so that it will cover the entire screen.
	Add(desktop);

	// put a small panel in the top left
	panel.Shape(16, 16, 160, 208);
	Add(panel);

	// setup the load button
	bLoad.Setup(24, 24, 144, 22, KEY_L, 0, "&Load wallpaper image");
	Add(bLoad);

	// setup the style label
	lStyle.Setup(24, 52, 144, 22, 0, 0, "Select wallpaper style:", 0);
	Add(lStyle);

	// setup the style radio buttons
	char *rText[] = { "centered", "top left corner", "stretched", "tiled", "border tiled" };
	for (int i=0; i<5; i++) {
		radio[i].Setup(40, 72+i*24, 112, 22, 0, 0, rText[i], 0);
		Add(radio[i]);
	}

	// the "tiled" style is the default, so we select the 4th radio button
	radio[3].Select();

	// setup the exit button
	bExit.Setup(24, 192, 144, 22, KEY_Q, D_EXIT, "&Quit");
	Add(bExit);
}


// even handler: maps messages to functions
void MyDialog::HandleEvent(Widget &obj, int msg, int arg1, int arg2) {
	Dialog::HandleEvent(obj, msg, arg1, arg2);

	if (msg == MSG_ACTIVATE) {
		if (obj == bLoad) {
			OnLoad();
		}
		else {
			for (int i=0; i<5; i++) {
				if (obj == radio[i]) {
					OnStyle(i);
				}
			}
		}
	}
}


// load a new wallpaper
void MyDialog::OnLoad() {
	// make a file selector
	static char path[1024];			// this will remember the last used path
	static bool havePath = false;
	FileSelect fSel("Select wallpaper", havePath ? path : NULL, "Images (*.bmp;*.tga;*.pcx)|All files (*.*)", 0xFF);

	// get the user selected filename
	char *filename = fSel.Popup(this);

	if (filename) {
		// make the wallpaper load and use it
		desktop.SetBitmap(filename);

		// remember the path for next time
		ustrcpy(path, filename);
		get_filename(path)[0] = 0;
		havePath = true;
	}
}


// change the style; i is the index of the radio button
void MyDialog::OnStyle(int i) {
	// Set new style depending on which radio button was clicked. It just so
	// happens that the indices of the buttons match the wallpaper style
	// constants so we could just call desktop.SetStyle(i), but it's good
	// programming practice to actually use names of enumerations, so
	// we do it in a slightly ugly large switch:
	switch (i) {
		case 0:		desktop.SetStyle(Wallpaper::CENTERED);		break;
		case 1:		desktop.SetStyle(Wallpaper::TOPLEFT);		break;
		case 2:		desktop.SetStyle(Wallpaper::STRETCHED);		break;
		case 3:		desktop.SetStyle(Wallpaper::TILED);			break;
		case 4:		desktop.SetStyle(Wallpaper::TILED2);		break;
	};
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

END_OF_MAIN()
