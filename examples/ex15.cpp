// Simple OpenGL example: shows how to use the default OpenGL screen update
// driver and the GLDialog class by implementing a simple spinning cube demo.
#include "../include/MASkinG.h"
using namespace MAS;

#ifdef		MASKING_GL

#include <GL/glu.h>

class SpinningCube {
	private:
		float rotX, rotY, rotZ;
		Color colBack;
		Color colWire;

	protected:
		void Cube() {
			glBegin(GL_QUADS);
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
		}

	public:
		SpinningCube() : rotX(0.0f), rotY(0.0f), rotZ(0.0f), colBack(Color::black), colWire(Color::gray) {
		}

		void Draw() {
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glClearColor(colBack.r()/256.0f, colBack.g()/256.0f, colBack.b()/256.0f, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glTranslatef(0,0,-15);
			glRotatef(rotX, 1.0f, 0.0f, 0.0f);
			glRotatef(rotY, 0.0f, 1.0f, 0.0f);
			glRotatef(rotZ, 0.0f, 0.0f, 1.0f);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glColor3f(colWire.r()/256.0f, colWire.g()/256.0f, colWire.b()/256.0f);
			Cube();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		void Tick() {
			rotX += 1.0f;	rotX = rotX >= 360.0f ? 360.0f - rotX : rotX;
			rotY += 1.5f;	rotY = rotY >= 360.0f ? 360.0f - rotY : rotY;
			rotZ += 0.5f;	rotZ = rotZ >= 360.0f ? 360.0f - rotZ : rotZ;
		}

		void SetColors(Color colBack, Color colWire) {
			this->colBack = colBack;
			this->colWire = colWire;
		}
};

class MyOpenGlWindow : public GLViewport, public SpinningCube {
	public:
		MyOpenGlWindow() : GLViewport() {
			SetColors(Color::gray, Color::white);
		}

		void Draw(Bitmap&) {
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(45, (float)w()/(float)h(), 1,100);
			SpinningCube::Draw();
		}

		void MsgTick() {
			GLViewport::MsgTick();
			SpinningCube::Tick();
			Redraw();
		}
};


class MyDialog : public GLDialog, public SpinningCube {
	private:
		FPS fpsCounter;
		Button bExit;
		MyOpenGlWindow aglWindow;
		MAS::Window wnd;

	public:
		MyDialog() : GLDialog() {
			SetColors(Color::gray, Color::white);

			aglWindow.Setup(16, 400, 64, 64, 0, 0);
			aglWindow.SetColors(Color::yellow, Color::black);
			Add(aglWindow);

			fpsCounter.Resize(32,14);
			Add(fpsCounter);

			bExit.Shape(70, 90, 28, 8, true);
			bExit.SetText("Exit");
			bExit.MakeExit();
			Add(bExit);

			wnd.Shape(340,0,300,200);
			Add(wnd);
		}

		void Draw(Bitmap&) {
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(45, (float)w()/(float)h(), 1,100);
			SpinningCube::Draw();
		}

		void MsgTick() {
			GLDialog::MsgTick();
			SpinningCube::Tick();
			Redraw();
		}
};


int main() {
	Error err = InstallMASkinG("gl.cfg");
	if (err) {
		err.Report();
	}
	Settings::useOpenGLMouseHack = true;

	// Set up OpenGL
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glShadeModel(GL_FLAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPointSize(2.0);
	glEnable(GL_TEXTURE_2D);
	glScissor(0, 0, SCREEN_W, SCREEN_H);
	glEnable(GL_SCISSOR_TEST);

	// do the dialog
	MyDialog *dlg = new MyDialog;
	dlg->Execute();
	delete dlg;

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
