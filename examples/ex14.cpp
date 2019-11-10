// example 14: Shows how to use normalized widget coordinates.


#include "../include/MASkinG.h"
using namespace MAS;


class NormalizedDialog : public Dialog {
	protected:
		Panel panel;
		Label label;
		Button b[5];

	public:
		NormalizedDialog() : Dialog() {
			panel.Shape(0, 0, 100, 100, true);
			Add(panel);

			label.Place(0, 4, true);
			label.w(100, true);
			label.h(20);
			label.ClearFlag(D_AUTOSIZE);
			label.AlignCentre();
			label.SetText("This is like a game menu or something...");
			Add(label);

			static char *bName[] = { "Play game", "Load game", "Options", "Highscore table", "Exit" };
			for (int i=0; i<5; i++) {
				b[i].Shape(20, 16+i*16, 60, 14, true);
				b[i].SetText(bName[i]);
				Add(b[i]);
			}
		}

		void SetButtonWidth(int w) {
			panel.Redraw();
			for (int i=0; i<5; i++) {
				b[i].x(50-w, true);
				b[i].w(w*2, true);
			}
		}
};


class MyDialog : public Dialog {
	protected:
		ClearScreen desktop;
		FPS fpsCounter;
		NormalizedDialog nDialog;

		Button b800x600, b640x480, bExit;
		Slider sWidth;

	public:
		MyDialog() : Dialog() {
			Add(desktop);

			fpsCounter.Resize(32,14);
			Add(fpsCounter);

			nDialog.Shape(3, 5, 94, 60, true);
			Add(nDialog);

			b800x600.Shape(3, 72, 20, 5, true);
			b800x600.SetText("800x600");
			Add(b800x600);

			b640x480.Shape(3, 80, 20, 5, true);
			b640x480.SetText("640x480");
			Add(b640x480);

			bExit.Shape(3, 88, 20, 5, true);
			bExit.SetText("Exit");
			bExit.MakeExit();
			Add(bExit);

			sWidth.Place(30, 76, true);
			sWidth.w(25, true);
			sWidth.SetRange(10, 45);
			sWidth.SetPosition(30);
			sWidth.SetOrientation(1);
			Add(sWidth);
		}

		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0) {
			Dialog::HandleEvent(obj, msg, arg1, arg2);

			if (msg == MSG_ACTIVATE) {
				if (obj == b800x600) {
					if (Settings::screenWidth != 800) {
						ChangeResolution(Settings::gfxMode, Settings::fullscreen, 800, 600, Settings::colorDepth, this);
					}
				}
				else if (obj == b640x480) {
					if (Settings::screenWidth != 640) {
						ChangeResolution(Settings::gfxMode, Settings::fullscreen, 640, 480, Settings::colorDepth, this);
					}
				}
			}
			else if (msg == MSG_SCROLL) {
				nDialog.SetButtonWidth(arg1);
			}
		}
};


int main() {
	allegro_init();
	char full_path[1024];
	MakeFullPath(full_path, "allegro.cfg");
	Settings::Load(full_path);
	Settings::screenWidth = 640;
	Settings::screenHeight = 480;
	Error err = InstallMASkinG();
	if (err) {
		err.Report();
	}

	MyDialog *dlg = new MyDialog;
	dlg->Execute();
	delete dlg;

	ExitMASkinG();
	return 0;
}
END_OF_MAIN();
