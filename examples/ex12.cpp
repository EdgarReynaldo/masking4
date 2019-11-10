#include "../include/MASkinG.h"
using namespace MAS;


// the first dialog (contains a panel, title and some buttons)
class Dialog1 : public Dialog {
	protected:
		Label title;
		Button button;
		RadioButton radio1;
		RadioButton radio2;
		RadioButton radio3;

	public:
		Dialog1() : Dialog() {
			title.SetText("This is the first dialog!");
			button.SetText("Button 1");
			radio1.SetText("Radio button 1");
			radio2.SetText("Radio button 2");
			radio3.SetText("Radio button 3");

			title.Shape(   10,  10, 100, 20);
			button.Shape(  20,  30, 100, 20);
			radio1.Shape(  20,  64, 140, 20);
			radio2.Shape(  20,  86, 140, 20);
			radio3.Shape(  20, 108, 140, 20);

			// add the objects to the dialog starting at position (0,0); later we will
			// add this dialog to the main dialog at any position we want
			Add(title);
			Add(button);
			Add(radio1);
			Add(radio2);
			Add(radio3);
		}

		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0) {
			Dialog::HandleEvent(obj,msg,arg1,arg2);

			if (obj == button && msg == MSG_ACTIVATE) {
				MessageBox m("Works!", "Button in dialog 1 was clicked.", NULL, NULL, "OK", NULL, NULL);
				m.Popup(Root());
			}
		}
};


class Dialog2 : public Dialog {
	protected:
		Label title;
		Button button1, button2;
		CheckBox check1, check2, check3;
		EditBox edit;

	public:
		Dialog2() : Dialog() {
			title.SetText("This is the second dialog!");
			button1.SetText("&OK");
			button2.SetText("&Cancel");
			button1.Key(KEY_O);
			button2.Key(KEY_C);
			button1.MakeExit();
			button2.MakeExit();
			check1.SetText("Checkbox 1");
			check2.SetText("Checkbox 2");
			check3.SetText("Checkbox 3");
			edit.SetText("Edit Me!", 32);

			title.Shape(   10,  10, 100,  20);
			check1.Shape(  20,  30, 100,  20);
			check2.Shape(  20,  54, 100,  20);
			check3.Shape(  20,  78, 100,  20);
			button1.Shape( 20, 110, 100,  20);
			button2.Shape(130, 110, 100,  20);
			edit.Shape(   130,  54, 120,  20);

			Add(title);
			Add(check1);
			Add(check2);
			Add(check3);
			Add(button1);
			Add(button2);
			Add(edit);
		}

		void Close() {
			Dialog::Close();
			Root()->Close();
		}
};


class Dialog3 : public Dialog {
	protected:
		Label title;
		Slider slider[10];
		Label number;

	public:
		Dialog3() : Dialog() {
			title.SetText("This is the third dialog!");
			slider[0].Setup(20,32,20,100, 0,0, 0,100,55,0);
			slider[1].Setup(45,32,20,100, 0,0, 0,100,60,0);
			slider[2].Setup(70,32,20,100, 0,0, 0,100,50,0);
			slider[3].Setup(95,32,20,100, 0,0, 0,100,45,0);
			slider[4].Setup(120,32,20,100, 0,0, 0,100,35,0);
			slider[5].Setup(145,32,20,100, 0,0, 0,100,48,0);
			slider[6].Setup(170,32,20,100, 0,0, 0,100,52,0);
			slider[7].Setup(195,32,20,100, 0,0, 0,100,57,0);
			slider[8].Setup(220,32,20,100, 0,0, 0,100,60,0);
			slider[9].Setup(245,32,20,100, 0,0, 0,100,65,0);
			number.Setup(270,32,25,20, 0,0, "0", 0);
			number.ClearFlag(D_AUTOSIZE);
			title.Shape(10, 10, 100, 20);

			Add(number);
			Add(title);
			for (int i=0; i<10; i++)
				Add(slider[i]);
		}

		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0) {
			Dialog::HandleEvent(obj, msg, arg1, arg2);

			if (msg==MSG_SCROLL) {
				number.SetNumber(arg1);
			}
		}

		void MsgInitSkin() {
			Dialog::MsgInitSkin();
			number.SetTextMode(skin->c_face);
		}
};


// the main dialog
class MainDlg : public Dialog {
	protected:
		ClearScreen desktop;
		FPS fpsCounter;
		TabPanel tabPanel;

		// these are the 3 subdialogs
		Dialog1 dlg1;
		Dialog2 dlg2;
		Dialog3 dlg3;

	public:
		MainDlg() : Dialog() {
			// setup the desktop
			Add(desktop);

			fpsCounter.Resize(32,14);
			Add(fpsCounter);

			// setup the tab panel
			tabPanel.Shape(100, 70, 310, 180);
			tabPanel.Attach(&dlg1, "dialog 1");
			tabPanel.Attach(&dlg2, "dialog 2");
			tabPanel.Attach(&dlg3, "dialog 3");

			Add(tabPanel);
		}
};


int main() {
	Error err = InstallMASkinG("allegro.cfg");
	if (err) {
		err.Report();
	}

	MainDlg *dlg = new MainDlg;
	dlg->Execute();
	delete dlg;

	ExitMASkinG();
	return 0;
}
END_OF_MAIN();
