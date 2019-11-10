// example 9: Demonstrates how you can use MASkinG side by side with an existing engine
#include "../include/MASkinG.h"
using namespace MAS;

#define	NSTARS	200

// some forward declarations
class MyDialog;
void doMyEngine();


// the backbuffer for double buffering
BITMAP *buffer;


// the timer system variable and callback
static volatile int timer = 0;
static void timer_f() {
	++timer;
} END_OF_STATIC_FUNCTION(timer_f);


// a custom screen update driver
class MyUpdateDriver : public MAS::ScreenUpdate {
	public:
		BITMAP *GetCanvas() {
			return buffer;
		}

		bool RequiresFullRedraw() {
			return true;
		}
};


// a simple dialog
class MyDialog : public Dialog {
	private:
		PanelRaised panel;
		TextArea info;
		Button bExit;

	protected:
		void SelectDriver() {
			if (!driver) {
				CreateUpdateDriver(new MyUpdateDriver());
			}
		}

	public:
		MyDialog() : Dialog() {
			info.AddText("Example 9:");
			info.AddText("Demonstrates how MASkinG can be used together with an existing");
			info.AddText("game engine. The scrolling stars you see in the background are");
			info.AddText("made with a system that is completely indpendant of MASkinG.");
			info.AddText("The timers, double buffering and the main loop, the systems");
			info.AddText("MASkinG usually implements, are in this example written outside");
			info.AddText("of MASkinG. The MASkinG dialog containing this text is not");
			info.AddText("executed with the Dialog::Execute() or Dialog::Popup() functions");
			info.AddText("but is instead polled from the user's main loop.");
			info.AddText("Also MASkinG is not installed with InstallMASkinG(). Instead");
			info.AddText("everything is installed, set up and initialized as in any other");
			info.AddText("regular Allegro application.");

			// Put a panel somewhere in the middle and a couple of widgets
			// on top of it.
			// Note that a top level dialog must take the whole screen!
			panel.Shape(120, 90, 460, 220);
			info.Shape(130, 100, 440, 170);
			bExit.Setup(290, 278, 120, 24, KEY_X, D_EXIT, "E&xit");
			bExit.SetTooltipText("Click me!");

			Add(panel);
			Add(info);
			Add(bExit);
		}

	friend void doMyEngine();
};


// a simple star class
class Star {
	protected:
		float x, y;
		float vy;
		Color col;

		void NewStar() {
			x = (float)(rand()%SCREEN_W);
			y = 0.0f;
			int l = rand()%128+128;
			vy = (float)(l)/128.0f;
			col = Color(l,l,l);
		}

	public:
		Star() {
			NewStar();
			y = (float)(rand()%SCREEN_H);
		}

		void Draw(BITMAP *buffer) {
			putpixel(buffer, (int)x, (int)y, col);
		}

		void Update() {
			y += vy;
			if ((int)y >= SCREEN_H) {
				NewStar();
			}
		}
};

// tha Engine
void doMyEngine() {
	int i;

	// make the backbuffer for doublebuffering
	buffer = create_bitmap(SCREEN_W, SCREEN_H);

	// make some stars;
	Star stars[NSTARS];

	// setup the timer system
	LOCK_VARIABLE(timer);
	LOCK_FUNCTION(timer_f);
	install_int_ex(timer_f, BPS_TO_TIMER(100));

	// make a new dialog and initialize it
	MyDialog *dlg = new MyDialog;
	dlg->MsgStart();

	timer = 0;
	while (!dlg->close) {
		while (timer) {
			// do the engine logic
			for (i=0; i<NSTARS; i++) {
				stars[i].Update();
			}

			--timer;
		}

		// draw the engine
		clear(buffer);
		for (i=0; i<NSTARS; i++) {
			stars[i].Draw(buffer);
		}

		// update the GUI (will redraw itself)
		dlg->MsgIdle();

		// do the double buffering thingy
		blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	}

	// deinitialize the dialog and delete it
	dlg->MsgEnd();
	delete dlg;

	// destory the backbuffer
	destroy_bitmap(buffer);
}


int main() {
	// install and initialize everything (instead of calling InstallMASkinG())
	allegro_init();
	char fullPath[1024];
	MakeFullPath(fullPath, "allegro.cfg");
	Settings::Load(fullPath);
	install_keyboard();
	install_timer();
	Timer::Lock();
	install_mouse();
	install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);

	//Settings::antialiasing = false;
	//Settings::mouseShadow = false;

	set_color_depth(16);
	if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 640, 480, 0, 0) != 0) {
		allegro_message("Can't set graphics mode!\n");
		return -1;
	}

	Color::OnColorDepthChange();
	alfont_init();
	MakeFullPath(fullPath, (char *)Settings::skinPath);
	theSkin = new Skin(fullPath);
	if (!theSkin || theSkin->GetError()) {
		allegro_message("Error loading skin!\n");
		return -1;
	}

	// do the engine thingy
	doMyEngine();

	// clean up and exit (instead of calling ExitMASkinG())
	if (theSkin) {
		delete theSkin;
		theSkin = NULL;
	}
	return 0;
}
END_OF_MAIN();
