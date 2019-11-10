#include "../include/MASkinG.h"
using namespace MAS;


class MainDlg : public Dialog {
	protected:
		ClearScreen desktop;
		FPS fpsCounter;
		Panel panel;
		Splitter splitter;
		PanelSunken pan[4];
		CheckBox chk[4];
		Label lBuffer;
		SpinBox spin;
	
	public:
		MainDlg() : Dialog() {
			int i;
			// setup the desktop
			Add(desktop);

			fpsCounter.Resize(32,14);
			Add(fpsCounter);
			
			panel.Shape(4, 28, 632, 448);
			Add(panel);
			
			// setup the splitter
			splitter.Shape(8, 32, 500, 440);
			for (i=0; i<4; i++) {
				splitter.Attach(&pan[i], i);
			}
			Add(splitter);
			
			int x = 516;
			int dy = 22;
			int y = 32;
			
			chk[0].Setup(x, y+=dy, 112, dy-2, 0, D_SELECTED, "North east");
			chk[1].Setup(x, y+=dy, 112, dy-2, 0, D_SELECTED, "South east");
			chk[2].Setup(x, y+=dy, 112, dy-2, 0, D_SELECTED, "North west");
			chk[3].Setup(x, y+=dy, 112, dy-2, 0, D_SELECTED, "South west");

			for (i=0; i<4; i++) {
				Add(chk[i]);
			}

			y+=2*dy;
			lBuffer.Setup(x, y+6, 40, 24, 0, 0, "width", 0);
			spin.Setup(x+40, y, 72, 24, 0, 0, 2, 16, 2, 2);
			Add(lBuffer);
			Add(spin);
		}
		
		
		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0) {
			Dialog::HandleEvent(obj, msg, arg1, arg2);
			
			if (msg == MSG_ACTIVATE) {
				if (obj == spin) {
					splitter.SetBufferZone((int)spin.GetPosition());
				}
				else {
					for (int i=0; i<4; i++) {
						if (obj == chk[i]) {
							if (obj.Selected()) {
								splitter.Attach(&pan[i], i);
							}
							else {
								splitter.Detach(&pan[i]);
							}
						}
					}
				}
			}
			else if (msg == MSG_SCROLL) {
				if (obj == spin) {
					splitter.SetBufferZone((int)spin.GetPosition());
				}
			}
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
