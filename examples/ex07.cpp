// example 7: demonstrates how to display a large bitmap in a scrollbox and how to
// use a TextArea and multiline labels.
// written largely by Martijn "amarillion" van Iersel

#include "../include/MASkinG.h"
using namespace MAS;


class ImgBox : public ScrollBox {
	private:
		BITMAP *bmp;

	protected:
		virtual void DrawClientArea(Bitmap &dest, int xofst, int yofst) {
			if (bmp) {
                blit(bmp, (BITMAP*)dest, xofst, yofst, 0, 0, ((BITMAP*)dest)->w, ((BITMAP*)dest)->h);
			}
		}

	public:
		ImgBox() : ScrollBox() {
			SetScrollSize(0,0);
		}

		void SetBitmap(BITMAP *newbmp) {
			bmp = newbmp;
			SetScrollSize(bmp->w, bmp->h);
			Redraw();
		}
};


class ScrollWnd : public MAS::Window {
	private:
		ComboBox cmbHopt, cmbVopt;
		Button bCenter, bOrigin;
		Panel panel;
		ImgBox imgBox;
		TextArea ta;
		ScrollBox *client;

	public:
		ScrollWnd() : MAS::Window() {
			title.SetText ("Scrollbox window");
			panel.Shape(0,0,320,80);

			cmbHopt.Shape(4, 8, 144, 24);
			cmbHopt.list.InsertItem (new ListItemString("SCROLLER_ON"), ScrollBox::SCROLLER_ON);
			cmbHopt.list.InsertItem (new ListItemString("SCROLLER_OFF"), ScrollBox::SCROLLER_OFF);
			cmbHopt.list.InsertItem (new ListItemString("SCROLLER_AUTO"), ScrollBox::SCROLLER_AUTO);
			cmbHopt.editBox.Disable();
			cmbHopt.list.Select(2);

			cmbVopt.Shape(164, 8, 144, 24);
			cmbVopt.list.InsertItem (new ListItemString("SCROLLER_ON"), ScrollBox::SCROLLER_ON);
			cmbVopt.list.InsertItem (new ListItemString("SCROLLER_OFF"), ScrollBox::SCROLLER_OFF);
			cmbVopt.list.InsertItem (new ListItemString("SCROLLER_AUTO"), ScrollBox::SCROLLER_AUTO);
			cmbVopt.editBox.Disable();
			cmbVopt.list.Select(2);

			bCenter.Setup(4,48,144,24,KEY_C,0,"&Center");
			bOrigin.Setup(164,48,144,24,KEY_O,0,"&Origin");
			client = &imgBox;
			imgBox.Shape(0,81,320,320);
			imgBox.Hide();
			ta.Shape(0,81,320,320);
			ta.Hide();

			Add(panel);
			Add(bCenter);
			Add(bOrigin);
			Add(imgBox);
			Add(ta);
			Add(cmbHopt);
			Add(cmbVopt);

			minSize = Size(320, 200);
			clientArea->Resize(400, 400);
			Resize(400, 400);
		}

		void SetBitmap(BITMAP *bmp) {
			client = &imgBox;
			imgBox.SetBitmap(bmp);
			imgBox.Unhide();
		}

		void LoadText(char *path) {
			ta.LoadLinesFromFile(path);
			ta.Unhide();
			client = &ta;
		}

		void UpdateSize() {
			MAS::Window::UpdateSize();

			panel.w(clientArea->w());
			imgBox.w(clientArea->w());
			imgBox.h(clientArea->h() - panel.h());
			ta.w(clientArea->w());
			ta.h(clientArea->h() - panel.h());
		}

		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0) {
			MAS::Window::HandleEvent(obj, msg, arg1, arg2);
			if (msg == MSG_ACTIVATE && obj == bCenter)
			{
				client->Center();
			}
			if (msg == MSG_ACTIVATE && obj == bOrigin)
			{
				client->SetPos(0,0);
			}
			if (msg == MSG_ACTIVATE && obj == cmbHopt)
			{
				client->SetScrollOption (arg1, client->GetVScrollOption());
			}
			if (msg == MSG_ACTIVATE && obj == cmbVopt)
			{
				client->SetScrollOption (client->GetHScrollOption(), arg1);
			}
		}
};


class LabelDialog : public MAS::Window
{
    private:
        Panel panel;
        Label label, lblHorizontal, lblVertical;
        ComboBox cmbVopt, cmbHopt;
    public:
        LabelDialog() : MAS::Window()
        {
            title.SetText ("Scrollbox window");
            panel.Shape (0,0,320,80);

            cmbHopt.Shape (112, 8, 144, 24);
            cmbHopt.list.InsertItem (new ListItemString("LEFT"), 0);
            cmbHopt.list.InsertItem (new ListItemString("CENTER"), 2);
            cmbHopt.list.InsertItem (new ListItemString("RIGHT"), 1);
            cmbHopt.editBox.Disable();
            cmbHopt.list.Select(0);

            lblHorizontal.Shape (4, 8, 100, 24);
            lblHorizontal.ClearFlag (D_AUTOSIZE);
            lblHorizontal.SetAlignment (1);
            lblHorizontal.SetVAlignment (2);
            lblHorizontal.SetText ("Horizontal :");

            cmbVopt.Shape (112, 40, 144, 24);
            cmbVopt.list.InsertItem (new ListItemString("TOP"), 0);
            cmbVopt.list.InsertItem (new ListItemString("CENTER"), 2);
            cmbVopt.list.InsertItem (new ListItemString("BOTTOM"), 1);
            cmbVopt.editBox.Disable();
            cmbVopt.list.Select(0);

            lblVertical.Shape (4, 40, 100, 24);
            lblVertical.ClearFlag (D_AUTOSIZE);
            lblVertical.SetAlignment (1);
            lblVertical.SetVAlignment (2);
            lblVertical.SetText ("Vertical :");

            label.Shape (0,81,320,320);
			label.ClearFlag(D_AUTOSIZE);
			label.SetWordWrap(true);

            Add (panel);
            Add (label);
            Add (lblHorizontal);
            Add (lblVertical);
            Add (cmbVopt);
            Add (cmbHopt);

			clientArea->Resize(400,400);
			Resize(400,400);
            minSize = Size(260, 80);
        }

        void SetText (char *text)
        {
            label.SetText (text);
        }

        void UpdateSize()
        {
            MAS::Window::UpdateSize();
            panel.w(clientArea->w());
            label.w(clientArea->w());
            label.h(clientArea->h() - panel.h());
        }

        void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0)
        {
            MAS::Window::HandleEvent(obj, msg, arg1, arg2);
            if (msg == MSG_ACTIVATE && obj == cmbHopt)
            {
                label.SetAlignment (arg1);
				Redraw(label);
            }
            if (msg == MSG_ACTIVATE && obj == cmbVopt)
            {
                label.SetVAlignment (arg1);
				Redraw(label);
            }
        }
};

class MyDialog : public Dialog {
	protected:
		ClearScreen desktop;
		Button bSoft, bHard;
		Button bLoad;
		Button bExit;
		TextArea ta;
		Label lInfo;

	public:
		MyDialog() : Dialog () {
			bExit.Setup(370, 390, 120, 24, KEY_X, D_EXIT, "E&xit");
			bLoad.Setup(370, 360, 120, 24, KEY_L, 0, "&Load");
			ta.Shape(10, 10, 400, 300);
			char fullPath[1024];
			MakeFullPath(fullPath, "../readme.txt");
			ta.LoadLinesFromFile(fullPath);
            bSoft.Setup(160, 390, 200, 24, KEY_S, 0, "&Soft line breaks");
            bHard.Setup(160, 360, 200, 24, KEY_H, 0, "&Hard line breaks");
			lInfo.Shape(10, 320, 128, 148);
			lInfo.SetWordWrap(true);
			lInfo.SetText("Hello! This is example 7 and it demonstrates how to use a ScrollBox and a TextArea. It also shows how to make labels with very long text that wraps automatically. Like this one :)");

			Add(desktop);
			Add(bExit);
			Add(bLoad);
			Add(bSoft);
			Add(bHard);
			Add(ta);
			Add(lInfo);
		}

		bool MsgClose() {
			Dialog::MsgClose();

			MessageBox msg("Message", "Do you really want to exit?", NULL, NULL, "Yes", "No", NULL);
			return msg.Popup(this) == 1 ? true : false;
		}


		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0) {
			Dialog::HandleEvent(obj, msg, arg1, arg2);

			if (msg == MSG_ACTIVATE) {
				if (obj == bLoad) {
					FileSelect dlg("Open a bitmap or a text file", NULL, "Bitmaps (*.bmp;*.pcx;*.tga)|Text files (*.txt;*.h;*.cpp;*.c;*.cc;*.hpp;*.me;*.ini)|All files (*.*)", FA_ARCH);
					char *path = dlg.Popup(this);
					if (path) {
						BITMAP *temp;
						temp = load_bitmap(path, NULL);
						if (temp) {
							ScrollWnd *myWin = new ScrollWnd();
							myWin->SetBitmap(temp);
							Add(*myWin);
							myWin->Centre();
						}
						else {
							ScrollWnd *myWin = new ScrollWnd();
							myWin->LoadText(path);
							Add(*myWin);
							myWin->Centre();
						}
					}
				}
				else if (obj == bSoft)
				{
					LabelDialog *lWin = new LabelDialog();
					lWin->SetText ("\
From:torvalds@klaava.Helsinki.FI (Linus Benedict Torvalds) \
Newsgroup: comp.os.minix \
Subject: GCC-1.40 and a posix question \
Message-ID: 1991Jul13, 100050.9886@klaava.Helsinki.FI \
Date: 3 Jul 91 10:00:50 GMT \
\
Hello netlanders, \
Due a project I'm working on (in minix), I'm interested in the posix standard definition. Could somebody please point me to a (preferably) machine-readable format of the latest posix rules? Ftp-sites would be nice. \
Linus Torvalds torvalds@kruuna.helsinki.fi");
					lWin->Popup(this);
					delete lWin;
				}
				else if (obj == bHard)
				{
					LabelDialog *lWin = new LabelDialog();
					lWin->SetText (" \
From:torvalds@klaava.Helsinki.FI (Linus Benedict Torvalds)\n\
Newsgroup: comp.os.minix\n\
Subject: GCC-1.40 and a posix question\n\
Message-ID: 1991Jul13, 100050.9886@klaava.Helsinki.FI\n\
Date: 3 Jul 91 10:00:50 GMT\n\
\n\
Hello netlanders,\n\
Due a project I'm working on (in minix), I'm interested in the posix standard definition. Could somebody please point me to a (preferably) machine-readable format of the latest posix rules? Ftp-sites would be nice.\n\
Linus Torvalds torvalds@kruuna.helsinki.fi");
					lWin->Popup(this);
					delete lWin;
				}
			}
			else if (msg == MSG_CLOSE && obj != *this) {
				Remove(obj);
			}
		}
};


int main()
{
	Error e = InstallMASkinG("allegro.cfg");
	if (e)
	{
		e.Report();
	}

	MyDialog *dlg = new MyDialog;
	dlg->Execute();
	delete dlg;

	ExitMASkinG();

	return 0;
}
END_OF_MAIN();
