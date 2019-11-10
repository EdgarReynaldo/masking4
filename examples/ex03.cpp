// example 3: This example demonstrates how to use windows and
// menus in MASkinG. Note that the file selector doesn't work
// sometimes for some inexplicable reason.


#include "../include/MASkinG.h"
using namespace MAS;

class MyWindow : public MAS::Window {
	protected:
		GroupBox gb;
		Button but[4];
		SpinBox spin;
		ComboBox cmb;

	public:
		MyWindow() : MAS::Window() {
			title.SetText("This is my window...");
			gb.SetTitle("This is a GroupBox");
			for (int i=0; i<4; i++) {
				but[i].Setup(10, 38 + 30*i, 120, 24, 0, 0, "Hello...");
				gb.Add(but[i]);
			}
			spin.Setup(10, 130, 120, 24, 0, 0, 1.0, 5.0, 2.0, 0.1);
			gb.Add(spin);

			cmb.list.InsertItem(new ListItemString("option 1"), 0);
			cmb.list.InsertItem(new ListItemString("option 2"), 1);
			cmb.list.InsertItem(new ListItemString("option 3"), 2);
			cmb.list.InsertItem(new ListItemString("option 4"), 3);
			cmb.list.InsertItem(new ListItemString("option 5"), 4);
			cmb.list.InsertItem(new ListItemString("option 6"), 5);
			cmb.list.InsertItem(new ListItemString("option 7"), 6);
			cmb.Setup(160, 20, 120, 24, 0, 0, 2);
			//cmb.editBox.Disable();
			cmb.list.h(120);
			gb.Add(cmb);

			Add(gb);
		}

		void UpdateSize() {
			Window::UpdateSize();
			minSize = Size(280 + w() - clientArea->w(), 160 + h() - clientArea->h());
			gb.Shape(4, 4, clientArea->w() - 8, clientArea->h() - 8);
			spin.Place(10, -30);
			cmb.Place(-130, clientArea->h()/3);
			but[3].Place(-130, -60);
		}

		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0) {
			Window::HandleEvent(obj, msg, arg1, arg2);
			if (msg == MSG_ACTIVATE && obj == but[3]) {
				MessageBox msg("Hello", "Hello, world!", NULL, NULL, "OK");
				msg.Popup(Root());
			}
		}
};


class ClippedImage : public Image {
	public:
		ClippedImage() : Image(), offX(0), offY(0) {
			ClearFlag(D_AUTOSIZE);
		}

		int offX, offY;
		void Draw(Bitmap &canvas) {
			if (bmp) {
				int sx = bmp.w() < w() ? (w() - bmp.w())/2 : 0;
				int sy = bmp.h() < h() ? (h() - bmp.h())/2 : 0;
				bmp.Blit(canvas, offX, offY, sx, sy, w(), h());
			}
		}
};

class ImagePad : public Dialog {
	protected:
		ClearScreen clrscr;
		ClippedImage img;
		Scroller hScroller, vScroller;

	public:
		ImagePad() : Dialog() {
			Add(clrscr);
			hScroller.SetOrientation(1);
			vScroller.SetOrientation(0);
			hScroller.Hide();
			vScroller.Hide();
			Add(img);
			Add(hScroller);
			Add(vScroller);
		}

		void Setup(const char *fileName) {
			img.offX = img.offY = 0;
			img.SetBitmap(fileName);
		}

		void UpdateSize() {
			Dialog::UpdateSize();

			img.Resize(w(), h());
			Bitmap bmp = img.GetBitmap();
			hScroller.Hide();
			vScroller.Hide();
			if (bmp) {
				if (bmp.w() > img.w()) {
					hScroller.Unhide();
					hScroller.Shape(0, h() - hScroller.h(), vScroller.Hidden() ? w() : w() - vScroller.w(), hScroller.h());
					img.h(h() - hScroller.h());
				}

				if (bmp.h() > img.h()) {
					vScroller.Unhide();
					vScroller.Shape(w() - vScroller.w(), 0, vScroller.w(), hScroller.Hidden() ? h() : h() - hScroller.h());
					img.w(w() - vScroller.w());
				}

				if (bmp.w() > img.w()) {
					hScroller.Unhide();
					hScroller.Shape(0, h() - hScroller.h(), vScroller.Hidden() ? w() : w() - vScroller.w(), hScroller.h());
					img.h(h() - hScroller.h());
				}

				if (bmp.h() > img.h()) {
					vScroller.Unhide();
					vScroller.Shape(w() - vScroller.w(), 0, vScroller.w(), hScroller.Hidden() ? h() : h() - hScroller.h());
					img.w(w() - vScroller.w());
				}

				if (!hScroller.Hidden()) {
					hScroller.SetRange(bmp.w(), img.w());
					if (img.offX > bmp.w() - img.w()) {
						img.offX = bmp.w() - img.w();
						hScroller.SetPosition(img.offX);
					}
				}
				else {
					img.offX = 0;
				}

				if (!vScroller.Hidden()) {
					vScroller.SetRange(bmp.h(), img.h());
					if (img.offY > bmp.h() - img.h()) {
						img.offY = bmp.h() - img.h();
						hScroller.SetPosition(img.offY);
					}
				}
				else {
					img.offY = 0;
				}
			}

			if (bmp && bmp.w() > img.w() && bmp.h() > img.h()) {
				clrscr.Shape(vScroller.x() - x(), hScroller.y() - y(), vScroller.w(), hScroller.h());
			}
			else {
				clrscr.Shape(0, 0, w(), h());
			}
		}

		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0) {
			if (msg == MSG_SCROLL) {
				if (obj == hScroller) {
					img.offX = arg1;
					img.Redraw();
				}
				else if (obj == vScroller) {
					img.offY = arg1;
					img.Redraw();
				}
			}

			Dialog::HandleEvent(obj, msg, arg1, arg2);
		}
};


class MyDialog : public Dialog {
	protected:
		ClearScreen desktop;
		FPS fpsCounter;
		Label driver;
		Button bExit;
		Button button[5];
		EditBox edit;
		MyWindow myWin;
		bool haveWindow;
		Button loadButton;
		ImagePad imagePad;
		MAS::Window imageWindow;
		bool haveImageWindow;

		Menu topMenu, popupMenu, fileMenu, editMenu, helpMenu, windowMenu, subMenu, subMenu2;
		enum { FILE_EXIT = MSG_SUSER, ADD_WINDOW, POPUP_WINDOW, MESSAGE_BOX, FILE_SELECTOR, COLOR_SELECTOR };


	public:
		MyDialog() : Dialog(), haveWindow(false), haveImageWindow(false) {
			fpsCounter.ClearFlag(D_AUTOSIZE);
			driver.ClearFlag(D_AUTOSIZE);
			fpsCounter.AlignRight();
			driver.AlignRight();
			fpsCounter.Shape(600,0,40,16);
			driver.Shape(400,16,240,16);

			edit.Setup(20, 100, 300, 24, 0, 0, NULL);
			loadButton.Setup(324, 100, 120, 24, 0, 0, "Show Image");
			imageWindow.Shape(60, 130, 300, 300);
			imagePad.Resize(300, 300);
			imageWindow.SetClientArea(&imagePad);

			//myWin.Shape(20, 20, 300, 200);
			myWin.Place(20, 20);
			myWin.GetClientArea()->Resize(300, 200);

			Add(desktop);
			Add(fpsCounter);
			Add(driver);
			Add(bExit);
			Add(edit);
			Add(loadButton);

			button[0].Setup(200, 240, 120, 24, KEY_A, 0, "&Add MyWindow");
			button[1].Setup(210, 270, 120, 24, KEY_P, 0, "&Popup MyWindow");
			button[2].Setup(230, 300, 120, 24, KEY_B, 0, "Message&Box");
			button[3].Setup(262, 330, 120, 24, KEY_F, 0, "&File Selector");
			button[4].Setup(310, 360, 120, 24, KEY_L, 0, "Co&lor Selector");
			for (int i=0; i<5; i++) Add(button[i]);
			bExit.Setup(370, 390, 120, 24, KEY_X, D_EXIT, "E&xit");

			MakeMenu();
		}

		void MakeMenu() {
			fileMenu.Add("&New\tCtrl-N");
			fileMenu.Add("&Open\tCtrl-O");
			fileMenu.Add("&Close\tCtrl-W");
			fileMenu.Add("&Save\tCtrl-S");
			fileMenu.Add("Save &as\tF12");
			fileMenu.Add();
			fileMenu.Add("E&xit\tCtrl-Q", FILE_EXIT);

			editMenu.Add("&Undo\tCtrl-Z");
			editMenu.Add("&Redo\tCtrl-Y");
			editMenu.Add();
			editMenu.Add("Cu&t\tCtrl-X");
			editMenu.Add("&Copy\tCtrl-C");
			editMenu.Add("&Paste\tCtrl-V");

			windowMenu.Add("&Add MyWindow\tCtrl-A", ADD_WINDOW);
			windowMenu.Add("&Popup MyWindow\tCtrl-P", POPUP_WINDOW);
			windowMenu.Add("&MessageBox\tCtrl-M", MESSAGE_BOX);
			windowMenu.Add("&File Selector\tCtrl-F", FILE_SELECTOR);
			windowMenu.Add("Co&lorSelector\tCtrl-L", COLOR_SELECTOR);

			subMenu2.Add("item 1");
			subMenu2.Add("item 2");
			subMenu2.Add("item 3");
			subMenu2.Add("item 4");
			subMenu2.Check(1);
			subMenu2.Check(2);

			subMenu.Add("item 1");
			subMenu.Add("item 2");
			subMenu.Add("item 3");
			subMenu.Add("item 4");
			subMenu.Add("item 5", subMenu2);
			subMenu.Add("item 6");

			helpMenu.Add("&Index\tF1");
			helpMenu.Add("&Homepage");
			helpMenu.Add("&About");
			helpMenu.Add();
			helpMenu.Add("&Submenu", subMenu);

			topMenu.Add("&File", fileMenu);
			topMenu.Add("&Edit", editMenu);
			topMenu.Add("&Window", windowMenu);
			topMenu.Add("&Help", helpMenu);
			Add(topMenu);

			popupMenu.Add("&File", fileMenu);
			popupMenu.Add("&Edit", editMenu);
			popupMenu.Add("&Window", windowMenu);
			popupMenu.Add("&Help", helpMenu);
		}

		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0) {
			Dialog::HandleEvent(obj, msg, arg1, arg2);

			switch (msg) {
				case FILE_EXIT:					Close();				break;
				case ADD_WINDOW:				OnAddRemoveWindow();	break;
				case POPUP_WINDOW:				OnPopupWindow();		break;
				case MESSAGE_BOX:				OnMessageBox();			break;
				case FILE_SELECTOR:				OnFileSelect();			break;
				case COLOR_SELECTOR:			OnColorSelect();		break;

				case MSG_ACTIVATE:
					if (obj == button[0])		OnAddRemoveWindow();
					else if (obj == button[1])	OnPopupWindow();
					else if (obj == button[2])	OnMessageBox();
					else if (obj == button[3]) 	OnFileSelect();
					else if (obj == button[4])	OnColorSelect();
					else if (obj == loadButton)	OnShowBitmap();
				break;

				case MSG_CLOSE:
					if (obj == myWin)			OnAddRemoveWindow();
					else if (obj == imageWindow) OnShowBitmap();
				break;

				case MSG_RPRESS:
					if (obj == desktop)			popupMenu.Popup(this, NULL, GetMousePos());	break;
			};
		}

		void MsgStart() {
			Dialog::MsgStart();

			char buf[128];
			usprintf(buf, "%s, %d bpp", GetDriver()->GetDescription(), bitmap_color_depth(::screen));
			driver.SetText(buf);
			edit.Select();
		}

		bool MsgClose() {
			Dialog::MsgClose();

			MessageBox msg("Message", "Do you really want to exit?", NULL, NULL, "Yes", "No", NULL);
			return msg.Popup(this) == 1 ? true : false;
		}

		void OnAddRemoveWindow() {
			if (!haveWindow) {
				Add(myWin);
				button[0].SetText("&Remove MyWindow");
				button[0].Key(KEY_R);
				windowMenu.GetItem(0)->SetText("&Remove MyWindow\tCtrl-R");
			}
			else {
				Remove(myWin);
				button[0].SetText("&Add MyWindow");
				button[0].Key(KEY_A);
				windowMenu.GetItem(0)->SetText("&Add MyWindow\tCtrl-A");
			}
			haveWindow = !haveWindow;
		}

		void OnPopupWindow() {
			MyWindow win;
			//win.Shape(200, 200, 300, 200);
			win.Place(200, 200);
			win.GetClientArea()->Resize(300, 200);
			win.Popup(this);
		}

		void OnMessageBox() {
			MessageBox msg("One question", "Are you really really sure", "you want to format your hard drive?", NULL, "Absolutely", "No way", NULL);
			if (msg.Popup(this) == 1) {
				msg.title.SetText("OK");
				msg.line1.SetText("Whatever...");
				msg.line2.SetText(NULL);
				msg.button1.SetText("OK");
				msg.button2.SetText(NULL);
			}
			else {
				msg.title.SetText("You said no.");
				msg.line1.SetText("Wrong answer!.");
				msg.line2.SetText("You will now die a slow painfull death!");
				msg.line3.SetText("Muahahahahaha!!!!");
				msg.button1.SetText("OK");
				msg.button2.SetText(NULL);
			}
			msg.Popup(this);
		}

		void OnFileSelect() {
			FileSelect dlg("Open image...", edit.GetText(), "Bitmaps (*.bmp;*.pcx;*.tga)|BMP files (*.bmp)|PCX files (*.pcx)|TGA Files (*.tga)|All files (*.*)", FA_ARCH);
			char *path = dlg.Popup(this);
			if (path) edit.SetText(path);
		}

		void OnColorSelect() {
			ColorSelect dlg;
			dlg.Popup(this);
		}

		void OnShowBitmap() {
			if (haveImageWindow) {
				Remove(imageWindow);
				loadButton.SetText("Load Image");
			}
			else {
				imagePad.Setup(edit.GetText());
				Add(imageWindow);
				loadButton.SetText("Hide Image");
			}
			haveImageWindow = !haveImageWindow;
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
