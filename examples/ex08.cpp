// example 11:
// Starfield scrolling demo. When you move the mouse close to the edge of
// the starfield and keep it still for a while the starfield scrolls itself.
//
// this example is the same as example 8, only we use a scrollbox as the basis of the starfield,
// and test the new ScrollToArea() and IsInClientArea() functionality.

#include "../include/MASkinG.h"
#include <vector>
using namespace MAS;
using namespace std;


// A simple class representing a star. It has 2D space coordinates
// and a colour just to make things look a little bit nicer.
class Star : public Point {
	public:
		Star(int x, int y, Color c) : Point(x,y), col(c) {}
		Color col;
};


// Forward declaration of our MiniMap widget.
class MiniMap;

// The actual starfield. The starield is a widget that can draw itself, handle
// mouse input, etc.
class Starfield : public ScrollBox {
	protected:
		vector<Star> stars;		// an array of stars
		int nStars;				// the number of stars in our starfield

		Point lastMousePos;		// the position of the mouse in the last frame; needed so
		// we can detect whether the mouse hasn't moved for a certain period of time or not
		int tickCounter;		// counter that counts the number of ticks when the mouse hasn't moved
		int scrollDelay;		// the number of ticks before the starfield starts scrolling
		int scrollSpeed;		// the rate at which the starfield scrolls itself
		int borderSize;			// the size of the area where the mouse must be to activate scrolling

		// Draws the starfield on the canvas.
		void DrawClientArea (Bitmap &canvas, int xofst, int yofst) {
			// clear the canvas to black
			canvas.Clear(Color::black);

			// paint all the stars
			for (int i=0; i<nStars; i++) {
				canvas.Putpixel(stars[i].x() - xofst, stars[i].y() - yofst, stars[i].col);
			}
		}

		// The MSG_TICK handler function. This function does the actual scrolling. It
		// detects whether the mouse is on top of a border and counts the number of
		// ticks when the mouse hasn't moved while being on top of a border. When that
		// number reaches the scrolling delay the starfield is scrolled. When the mouse
		// hasn't moved for another period of time the speed of scrolling is increased.
		void MsgTick() {
			Widget::MsgTick();

			// If we don't have the mouse there's no point in checking anything so
			// we just reset the tick counter and quit.
			if (!HasMouse()) {
				tickCounter = 0;
				return;
			}

			// Get the mouse position.
			Point mp = GetMousePos();

			// Check if the mouse is on top of a border.
			bool onBorder = !(mp < Rect(GetClientLeft() + borderSize, GetClientTop() + borderSize,
			GetClientWidth() - 2*borderSize, GetClientHeight() - 2*borderSize));
			if (!IsInClientArea (mp)) onBorder = false;

			// If the mouse is on top of a border and the tick counter has reached
			// the scrolling delay, then start scrolling. Or if the starfield is
			// already scrolling and the mouse moved, make sure the scrolling speed
			// doesn't get increased.
			if (onBorder && tickCounter >= scrollDelay) {
				if (mp != lastMousePos) {
					// the mouse has moved so we set the tick counter back so that
					// later on the scrolling speed won't be increased
					tickCounter = scrollDelay;
				}

				// Setting the last mouse position to the current mouse position makes
				// sure the starfield will start scrolling and if it already is scrolling
				// it will make sure scrolling doesn't stop when the mouse moves.
				lastMousePos = mp;
			}

			// check if the mouse has moved since the previous frame (or if we set
			// the last mouse position to the current position in order to initiate
			// scrolling or prevent it from stopping)
			if (mp == lastMousePos && IsInClientArea (mp)) {
				// increase the counter how long the mouse hasn't moved
				tickCounter++;

				// if the mouse hasn't moved for a certain period of time, we do the scrolling
				if (tickCounter >= scrollDelay) {
					// set the speed of the scrolling; if the left mouse button is not pressed
					// down, scroll with normal speed otherwise accelerate a bit
					if (parent->GetMouse()->flags & 1) {
						scrollSpeed = 16;
					}
					else {
						scrollSpeed = 4;
					}

					// check the mouse position for each of the 4 borders and scroll the
					// starfield accordingly
					Point d;
					if (mp.x() < borderSize) {
					Scroll (d = Point (-scrollSpeed, 0));
						MsgScroll ();
					}
					else if (mp.x() >= w() - borderSize) {
						Scroll (d = Point (scrollSpeed, 0));
						MsgScroll ();
					}

					if (mp.y() < borderSize) {
						Scroll (d = Point (0, -scrollSpeed));
						MsgScroll ();
					}
					else if (mp.y() >= h() - borderSize) {
						Scroll (d = Point (0, scrollSpeed));
						MsgScroll ();
					}
				}
			}
			else {
				// if got to here, the mouse moved so we just reset everything and start waiting
				// for the mouse to stand still again
				lastMousePos = mp;
				tickCounter = 0;
			}
		}

		// The scrollbox is a compound widget made of a panel, scrollbars, etc. We don't want this
		// in this example so we'll remove them and make the client area cover the entire widget.
		void MsgInitSkin() {
			ScrollBox::MsgInitSkin();
			bufl = 0;
			bufr = 0;
			buft = 0;
			bufb = 0;
			panel.Hide();
			SetScrollOption (0, 0);
		}


	public:
		// the constructor just does some basic setup stuff
		Starfield() : ScrollBox() {
			tickCounter = 0;	// reset the tick counter
			scrollDelay = 5;	// scrolling will start when the mouse hasn't moved for 5 frames
			borderSize = 40;	// the size of the area where the mouse must be in order for the starfield to scroll
			scrollSpeed = 0;	// the scrolling speed

			nStars = 3000;					// make a lot of stars
			SetScrollSize (1600, 1200);
//			scrollOffset = Point(0, 0);		// reset the scrolling offset

			// place the stars randomly into our space
			for (int i=0; i<nStars; i++) {
				int c = rand()%192 + 64;
				stars.push_back(Star(rand()%GetScrollSize().w(), rand()%GetScrollSize().h(), Color(c,c,c)));
			}
		}

		~Starfield() {
			stars.clear();
		}

		void Scroll(Point& d) {
			ScrollBox::SetPos(GetXOffset() + d.x(), GetYOffset() + d.y());
		}

		// called when the scrollbars are changed
		virtual void MsgScroll() {
			GetParent()->HandleEvent(*this, MSG_SCROLL);
		}

		// make the small map class our friend so it will be able to access our protected data
		friend class MiniMap;
};


// A simple widget that just draws a visual representation of how much we have scrolled in the starfield.
class MiniMap : public Widget {
	protected:
		void Draw(Bitmap &canvas) {
			canvas.Clear(Color::gray);
			canvas.Rectangle(0, 0, w()-1, h()-1, Color::black);

			int ww = (w()-2)*field->w()/field->GetScrollSize().w();
			int hh = (h()-2)*field->h()/field->GetScrollSize().h();
			int xx = (w()-2-ww)*field->GetXOffset()/(field->GetScrollSize().w() - field->w()) + 1;
			int yy = (h()-2-hh)*field->GetYOffset()/(field->GetScrollSize().h() - field->h()) + 1;
			canvas.Rectangle(xx, yy, xx+ww, yy+hh, Color::red);
		}

		bool MsgWantfocus() {
			Widget::MsgWantfocus();
			return true;
		}

		void MsgLPress() {
			Widget::MsgLPress();

			SetFlag(D_PRESSED);
			Size previewSize((w()-2)*field->w()/field->GetScrollSize().w(), (h()-2)*field->h()/field->GetScrollSize().h());
			Point mp = GetMousePos();
			mp.x((mp.x() - previewSize.w()/2) * field->GetScrollSize().w() / w());
			mp.y((mp.y() - previewSize.h()/2) * field->GetScrollSize().h() / h());
			field->SetPos(mp.x(), mp.y());
			Redraw();
		}

		void MsgLRelease() {
			Widget::MsgLRelease();
			ClearFlag(D_PRESSED);
		}

		void MsgMousemove(const Point &d) {
			Widget::MsgMousemove(d);

			if (TestFlag(D_PRESSED)) {
//				Size previewSize((w()-2)*field->w()/field->GetScrollSize().w(), (h()-2)*field->h()/field->GetScrollSize().h());
				Point p = d;
				p.x(p.x() * field->GetScrollSize().w() / w());
				p.y(p.y() * field->GetScrollSize().h() / h());

				field->Scroll(p);
				Redraw();
			}
		}

	public:
		Starfield *field;
};


class MyDialog : public Dialog {
	private:
		ClearScreen desktop;
		Button bExit;
		FPS fpsCounter;
		Label driver, title, lFPS, lDriver, lTitle, lInstructions;

		Starfield starfield;
		MiniMap miniMap;

	public:
		MyDialog() : Dialog() {
			Add(desktop);

			lTitle.Shape(0,0,80,12);	lTitle.ClearFlag(D_AUTOSIZE);	lTitle.AlignRight();	lTitle.SetText("example 8:");
			lDriver.Shape(0,16,80,12);	lDriver.ClearFlag(D_AUTOSIZE);	lDriver.AlignRight();	lDriver.SetText("driver:");
			lFPS.Shape(0,32,80,12);		lFPS.ClearFlag(D_AUTOSIZE);		lFPS.AlignRight();		lFPS.SetText("FPS:");

			title.Shape(88,0,240,12);		title.SetText("Scrolling starfield");
			driver.Shape(88,16,240,12);
			fpsCounter.Shape(88,32,40,12);

			lInstructions.ClearFlag(D_AUTOSIZE);
			lInstructions.Shape(20, 400, 400, 80);
			lInstructions.SetText("Instructions: Hover with the mouse over an edge of the starfield to scroll the view. Click to scroll faster. Hold and drag in the minimap to change the viewport.");
			lInstructions.SetWordWrap(true);
			Add(lInstructions);

			Add(lTitle);
			Add(lDriver);
			Add(lFPS);
			Add(title);
			Add(driver);
			Add(fpsCounter);

			bExit.Setup(500, 450, 134, 24, KEY_X, D_EXIT, "E&xit");
			Add(bExit);

			starfield.Shape(20, 80, 400, 300);
			miniMap.Shape(440, 80, 100, 100);
			miniMap.field = &starfield;

			Add(starfield);
			Add(miniMap);
		}

		void MsgStart() {
			Dialog::MsgStart();

			char buf[128];
			usprintf(buf, "%s, %d bpp", GetDriver()->GetDescription(), bitmap_color_depth(::screen));
			driver.SetText(buf);
		}

		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0) {
			Dialog::HandleEvent(obj, msg, arg1, arg2);
			if (obj == starfield && msg == MSG_SCROLL) {
				miniMap.Redraw();
			}
		}
};


int main() {
	Error err = InstallMASkinG("allegro.cfg");
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
