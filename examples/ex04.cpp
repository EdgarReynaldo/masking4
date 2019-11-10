// example 4: A stopwatch program that demonstrates how to use user
// installable timers in MASkinG.

#include "../include/MASkinG.h"
using namespace MAS;

// a fancy class for printing out LCD display style numbers
typedef struct NUMCHAR {
	BITMAP *ch;
	BITMAP *alpha;
} NUMCHAR;

class NUMFONT {
	protected:
		NUMCHAR fnt[12];
		BITMAP *bfont, *alpha;

	public:
		NUMFONT() {
			bfont = NULL;
			alpha = NULL;

			for (int i=0; i<12; i++) {
				fnt[i].ch = NULL;
				fnt[i].alpha = NULL;
			}
		}

		~NUMFONT() {
			Destroy();
		}

		void Destroy() {
			for (int i=0; i<12; i++) {
				if (fnt[i].ch) {
					destroy_bitmap(fnt[i].ch);
					fnt[i].ch = NULL;
				}
				if (fnt[i].alpha) {
					destroy_bitmap(fnt[i].alpha);
					fnt[i].alpha = NULL;
				}
			}

			if (bfont) {
				destroy_bitmap(bfont);
				bfont = NULL;
			}

			if (alpha) {
				destroy_bitmap(alpha);
				alpha = NULL;
			}
		}

		void Load(const char *fontFile, const char *alphaFile) {
			Destroy();

			int i, x;
			int fw, fh;

			bfont = load_bitmap(fontFile, NULL);
			if (bfont) {
				fw = bfont->w / 12;
				fh = bfont->h;

				for (i=0, x=0; i<12; i++, x+=fw) {
					fnt[i].ch = create_sub_bitmap(bfont, x, 0, fw, fh);
				}
			}

			if (bitmap_color_depth(screen) != 32) return;
			alpha = load_bitmap(alphaFile, NULL);
			if (alpha) {
				fw = alpha->w / 12;
				fh = alpha->h;

				for (i=0, x=0; i<12; i++, x+=fw) {
					fnt[i].alpha = create_sub_bitmap(alpha, x, 0, fw, fh);
				}
			}
		}

		void Print(BITMAP *canvas, int x, int y, const char *string) {
			if (!bfont) {
				clear_to_color(canvas, Color::black);
#if (ALLEGRO_VERSION >= 4 && ALLEGRO_SUB_VERSION >= 1)
				textout_ex(canvas, font, string, (canvas->w - text_length(font, string))/2, (canvas->h - text_height(font))/2, Color::red, -1);
#else
				text_mode(-1);
				textout(canvas, font, string, (canvas->w - text_length(font, string))/2, (canvas->h - text_height(font))/2, Color::red);
#endif
				return;
			}

			int l = ustrsize(string);

			int i, ch, fw, xx, yy;
			fw = fnt[0].ch->w;

			xx = x;
			yy = y;
			for (i=0; i<l; i++) {
				if (string[i] >= '0' && string[i] <= '9') {
					ch = string[i] - '0';
				}
				else if (string[i] == ':') {
					ch = 10;
				}
				else {
					ch = 11;
				}

				draw_sprite(canvas, fnt[ch].ch, xx, yy);
				xx += fw;
			}

			if (!alpha) return;
			set_alpha_blender();
			xx = x - (fnt[0].alpha->w - fw)/2;
			yy = y - (fnt[0].alpha->h - fnt[0].ch->h)/2;
			for (i=0; i<l; i++) {
				if (string[i] >= '0' && string[i] <= '9') {
					ch = string[i] - '0';
				}
				else if (string[i] == ':') {
					ch = 10;
				}
				else {
					ch = 11;
				}

				draw_trans_sprite(canvas, fnt[ch].alpha, xx, yy);
				xx += fw;
			}
		}
};

// our LCD font
NUMFONT lcdFont;


// a simple widget that dislpays time using the special LCD font
class TimeDisplay : public Widget {
	protected:
		int time;

		void Draw(Bitmap &canvas) {
			int hrs, min, sec, thd;

			thd = time%1000;
			sec = (time/1000)%60;
			min = (time/60000)%60;
			hrs = (time/360000)%24;

			char buf[16];
			usprintf(buf, "%02d:%02d:%02d:%03d", hrs, min, sec, thd);
			lcdFont.Print(canvas, 0, 0, buf);
		}

	public:
		TimeDisplay() : Widget(), time(0) {
		}

		void SetTime(int i) {
			time = i;
			Redraw();
		}
};


class TimerDialog : public Dialog {
	private:
		Panel panel;
		Label panelTitle;
		Button bStart, bStop, bReset, bIntermediate;
		TimeDisplay tMain, tIntermediate1, tIntermediate2;

		int stopWatchTimer, curIntermediate, curTime;

	public:
		TimerDialog() : Dialog(), stopWatchTimer(-1), curIntermediate(0), curTime(0) {
			panel.Shape(0, 0, 448, 216);
			panelTitle.ClearFlag(D_AUTOSIZE);
			panelTitle.Setup(4, 4, 440, 20, 0, 0, "Example no. 4: Stopwatch");
			panelTitle.AlignCentre();
			bStart.Setup(8, 28, 105, 24, KEY_S, 0, "&Start");
			bStop.Setup(117, 28, 105, 24, KEY_P, D_DISABLED, "Sto&p");
			bReset.Setup(226, 28, 105, 24, KEY_R, 0, "&Reset");
			bIntermediate.Setup(335, 28, 105, 24, KEY_I, D_DISABLED, "&Interm.");
			tMain.Shape(8, 58, 432, 48);
			tIntermediate1.Shape(8, 108, 432, 48);
			tIntermediate2.Shape(8, 158, 432, 48);

			Shape((SCREEN_W - 432)/2 - 8, (SCREEN_H - 180)/2 - 28, 448, 216);
			SetFlag(D_MOVABLE | D_WINDOW);
			Add(panel);
			Add(panelTitle);
			Add(bStart);
			Add(bStop);
			Add(bReset);
			Add(bIntermediate);
			Add(tMain);
			Add(tIntermediate1);
			Add(tIntermediate2);
		}

		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0) {
			Dialog::HandleEvent(obj, msg, arg1, arg2);

			if (msg == MSG_ACTIVATE) {
				if (obj == bStart)			OnStart();			else
				if (obj == bStop)			OnStop();			else
				if (obj == bReset)			OnReset();			else
				if (obj == bIntermediate)	OnIntermediate();
			}
		}

		void OnStart() {
			bStart.Disable();
			bStop.Enable();
			bIntermediate.Enable();

			// install a timer that will tick at 1 ms intervals;
			// note how we need to store the ID of the timer we installed; we need
			// this so we can later handle timer tick events
			stopWatchTimer = Timer::Install(1);
		}

		void OnStop() {
			bStart.Enable();
			bStop.Disable();
			bIntermediate.Disable();

			// stop the timer we installed when the start button was pressed
			Timer::Kill(stopWatchTimer);
			stopWatchTimer = -1;
		}

		void OnReset() {
			tMain.SetTime(0);
			tIntermediate1.SetTime(0);
			tIntermediate2.SetTime(0);
			curTime = 0;
			curIntermediate = 0;
		}

		void OnIntermediate() {
			curIntermediate++;
			if (curIntermediate == 1) {
				tIntermediate1.SetTime(curTime);
			}
			else if (curIntermediate == 2) {
				tIntermediate2.SetTime(curTime);
				curIntermediate = 0;
			}
		}

		void MsgInitSkin() {
			Dialog::MsgInitSkin();

			panelTitle.SetFontColor(skin->fcol[Skin::INFO_WINDOW][Skin::SELECT], skin->scol[Skin::INFO_WINDOW][Skin::SELECT], Skin::NORMAL);
			panelTitle.SetTextMode(skin->c_select);
		}

		void MsgTimer(int id) {
			Dialog::MsgTimer(id);

			// MsgTimer() is called whenever a timer ticks. There could be other
			// timers installed (actually there is at least one - the system timer)
			// so we need to check if this call was actually made by our stopwatch
			// timer.
			if (id == stopWatchTimer) {
				tMain.SetTime(++curTime);
			}
		}
};


class MyDialog : public Dialog {
	private:
		ClearScreen desktop;
		FPS fpsCounter;
		Label driver;
		TimerDialog timerDlg;
		Button bExit;

	public:
		MyDialog() : Dialog() {
			fpsCounter.Shape(0,0,40,16);
			driver.Shape(0,16,240,16);
			bExit.Setup(514, 450, 120, 24, KEY_X, D_EXIT, "E&xit");

			Add(desktop);
			Add(fpsCounter);
			Add(driver);
			Add(bExit);
			Add(timerDlg);
		}

		void MsgStart() {
			Dialog::MsgStart();

			char buf[128];
			usprintf(buf, "%s, %d bpp", GetDriver()->GetDescription(), bitmap_color_depth(::screen));
			driver.SetText(buf);
		}
};



int main() {
	Error err = InstallMASkinG("allegro.cfg");
	if (err) {
		err.Report();
	}

	char fntPath[1024], alphaPath[1024];
	MakeFullPath(fntPath, "digifont.pcx");
	MakeFullPath(alphaPath, "alpha.tga");
	lcdFont.Load(fntPath, alphaPath);

	MyDialog *dlg = new MyDialog;
	dlg->Execute();
	delete dlg;

	lcdFont.Destroy();
	ExitMASkinG();
	return 0;
}
END_OF_MAIN();
