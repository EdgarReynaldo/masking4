// example 1: A simple "Hello, world!" example for MASkinG. We put a button
// on a dialog and execute it.

#include "../include/MASkinG.h"
using namespace MAS;


class MyDialog : public Dialog {
	private:
		// here you put your dialog controls like boxes, buttons, etc.:
		ClearScreen desktop;
		Button myFirstButton;
		Button mySecondButton;

	public:
		MyDialog();
};

// All the initialization can be done in the constructor
MyDialog::MyDialog() : Dialog() {
	// Set the control's size, position, flags, etc.
	myFirstButton.SetText("Hello, world!");
	myFirstButton.Shape(120, 180, 120, 24);
	myFirstButton.MakeExit();

	// Add the controls to the dialog
	Add(desktop);
	Add(myFirstButton);
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
