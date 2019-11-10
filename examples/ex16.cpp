// Simple OpenGL example: shows how to use the OpenGL DRS screen update
// driver and the GLViewport class by implementing a simple spinning cube demo.
#include "../include/MASkinG.h"
using namespace MAS;

#ifdef		MASKING_GL
#include <GL/glu.h>


class MyOpenGlWindow : public GLViewport {
	private:
		float rotX, rotY, rotZ;

	protected:
		void DrawCube() {
			glTranslatef(0,0,-15);
			glRotatef(rotX, 1.0f, 0.0f, 0.0f);
			glRotatef(rotY, 0.0f, 1.0f, 0.0f);
			glRotatef(rotZ, 0.0f, 0.0f, 1.0f);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glBegin(GL_QUADS);
				glColor3f(0.5f, 0.5f, 0.5f);

				// face
				glVertex3f(-2, -2, -2);
				glVertex3f(2, -2, -2);
				glVertex3f(2, 2, -2);
				glVertex3f(-2, 2, -2);

				// right
				glVertex3f(2, -2, -2);
				glVertex3f(2, -2, 2);
				glVertex3f(2, 2, 2);
				glVertex3f(2, 2, -2);

				// left
				glVertex3f(-2, -2, -2);
				glVertex3f(-2, 2, -2);
				glVertex3f(-2, 2, 2);
				glVertex3f(-2, -2, 2);

				// back
				glVertex3f(2, -2, 2);
				glVertex3f(-2, -2, 2);
				glVertex3f(-2, 2, 2);
				glVertex3f(2, 2, 2);

				// top
				glVertex3f(-2, 2, -2);
				glVertex3f(2, 2, -2);
				glVertex3f(2, 2, 2);
				glVertex3f(-2, 2, 2);

				// bottom
				glVertex3f(-2, -2, -2);
				glVertex3f(2, -2, -2);
				glVertex3f(2, -2, 2);
				glVertex3f(-2, -2, 2);
			glEnd();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

	public:
		MyOpenGlWindow() : GLViewport(), rotX(0.0f), rotY(0.0f), rotZ(0.0f) {}
		virtual ~MyOpenGlWindow() {}

		void Draw(Bitmap &canvas) {
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(45, (float)w()/(float)h(), 1,100);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			DrawCube();
		}

		void MsgTick() {
			GLViewport::MsgTick();

			rotX += 1.0f;	rotX = rotX >= 360.0f ? 360.0f - rotX : rotX;
			rotY += 1.5f;	rotY = rotY >= 360.0f ? 360.0f - rotY : rotY;
			rotZ += 0.5f;	rotZ = rotZ >= 360.0f ? 360.0f - rotZ : rotZ;
			Redraw();
		}
};


class ViewportDialog : public Dialog {
	protected:
		MyOpenGlWindow viewport;

	public:
		ViewportDialog() : Dialog() {
			viewport.Shape(0,0,100,100, true);
			Add(viewport);
		}
};


class MyDialog : public Dialog {
	private:
		ClearScreen desktop;
		FPS fpsCounter;
		Label driverDescription;

		MyOpenGlWindow aglWindow;
		Button buttonExit, buttonPopup, buttonAdd, buttonGL;
		MAS::Window myWindow;
		Menu fileMenu, editMenu, helpMenu, mainMenu;
		MAS::Window viewportWindow;
		ViewportDialog viewportDialog;

		enum { FILE_EXIT = MSG_SUSER };

	protected:
		void MakeMenu() {
			fileMenu.Add("New\tCtrl-N");
			fileMenu.Add("Open\tCtrl-O");
			fileMenu.Add("Save\tCtrl-S");
			fileMenu.Add("Close\tCtrl-C");
			fileMenu.Add();
			fileMenu.Add("Exit\tCtrl-Q", FILE_EXIT);

			editMenu.Add("Cut\tCtrl-X");
			editMenu.Add("Copy\tCtrl-C");
			editMenu.Add("Paste\tCtrl-V");

			helpMenu.Add("Index\tF1");
			helpMenu.Add("About\tCtrl-A");

			mainMenu.Add("File", fileMenu);
			mainMenu.Add("Edit", editMenu);
			mainMenu.Add("Help", helpMenu);
		}

	public:
		MyDialog() : Dialog() {
			fpsCounter.Resize(32,14);
			driverDescription.Shape(0,16,240,16);

			buttonExit.Setup(20, 40, 120, 22, KEY_X, D_EXIT, "E&xit");
			aglWindow.Setup(20, 70, 300, 300, 0, 0);
			//aglWindow.Shape(0, 0, 100, 100, true);
			buttonPopup.Setup(20, 380, 150, 22, KEY_P, 0, "&Popup a window");
			buttonAdd.Setup(20, 410, 150, 22, KEY_A, 0, "&Add a window");
			buttonGL.Setup(20, 440, 150, 22, KEY_G, 0, "Open&GL window");
			viewportDialog.Resize(300, 200);
			viewportWindow.SetClientArea(&viewportDialog);
			viewportWindow.Place(100, 100);
			MakeMenu();

			Add(desktop);
			Add(fpsCounter);
			Add(driverDescription);
			Add(aglWindow);
			Add(buttonExit);
			Add(buttonPopup);
			Add(buttonAdd);
			Add(buttonGL);
		}

		void MsgStart() {
			Dialog::MsgStart();

			char buf[128];
			usprintf(buf, "%s, %d bpp", GetDriver()->GetDescription(), bitmap_color_depth(::screen));
			driverDescription.SetText(buf);
		}

		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0) {
			Dialog::HandleEvent(obj, msg, arg1, arg2);

			switch (msg) {
				case FILE_EXIT: {
					Close();
				}
				break;

				case MSG_ACTIVATE: {
					if (obj == buttonPopup) {
						MessageBox dlg("Message",
									"This is a popup window",
									"It actually works :)",
									NULL,
									"OK");
						dlg.Popup(this);
					}
					else if (obj == buttonAdd) {
						myWindow.SetParent(this);
						myWindow.Resize(200, 200);
						myWindow.Centre();
						Add(myWindow);
						buttonAdd.Disable();
					}
					else if (obj == buttonGL) {
						viewportWindow.Popup(this);
					}
				}
				break;

				case MSG_CLOSE: {
					if (obj == myWindow) {
						Remove(myWindow);
						buttonAdd.Enable();
					}
				}
				break;

				case MSG_RPRESS: {
					if (obj == desktop) {
						mainMenu.Popup(this, NULL, GetMousePos());
					}
				}
				break;
			};
		}
};


int main() {
	Error err = InstallMASkinG("gl.cfg");
	if (err) {
		err.Report();
	}

	// Set up OpenGL
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	glDisable(GL_DITHER);
	glShadeModel(GL_FLAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
	glPolygonMode(GL_FRONT, GL_FILL);

	glPointSize(2.);
	glEnable(GL_TEXTURE_2D);
	glScissor(0, 0, SCREEN_W, SCREEN_H);
	glEnable(GL_SCISSOR_TEST);

	// do the dialog
	MyDialog *dlg = new MyDialog;
	dlg->Execute();
	delete dlg;

	// get out
	ExitMASkinG();
	return 0;
}
END_OF_MAIN();

#else

int main() {
	Error err = InstallMASkinG("allegro.cfg");
	if (err) {
		err.Report();
	}

	// do the dialog

	Dialog dlg;

	ClearScreen desktop;
	dlg.Add(desktop);

	Label message;
	message.SetWordWrap(true);
	message.Setup(20, 20, 300, 400, 0, 0, "Sorry, this example won't work. It requires AllegroGL, but you built MASkinG without AllegroGL support. Please install AllegroGL and rebuild MASkinG with AllegroGL support for this example to work. See the documentation for more detailed instructions.");
	dlg.Add(message);
	dlg.Execute();

	ExitMASkinG();
	return 0;
}
END_OF_MAIN();


#endif	//MASKING_GL

