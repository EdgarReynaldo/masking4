////////////////////////////////////////////////////////////////////////////////
//         MASkinG - Miran Amon's Skinnable GUI Library for Allegro           //
//    Copyright (c) 2002-2005 Miran Amon (miranamon@users.sourceforge.net)    //
//          Project websites: http://ferisrv5.uni-mb.si/~ma0747               //
//                  http://sourceforge.net/projects/masking                   //
////////////////////////////////////////////////////////////////////////////////

#include <allegro.h>
#include "../include/MASkinG/color.h"


MAS::Color::Color(int c)
	:col(c)
{
}


MAS::Color::Color(int r, int g, int b) {
	Make(r,g,b);
}


MAS::Color::Color(int r, int g, int b, int a) {
	Make(r,g,b,a);
}


MAS::Color::Color(float h, float s, float v) {
	int r,g,b;
	hsv_to_rgb(h,s,v, &r,&g,&b);
	Make(r,g,b);
}


MAS::Color::Color(const char *str) {
	Make(str);
}

////////////////////////////////////////////////////////////////////////////////
// Converts a string to a color where string is in standard rgb
// format (e.g. 64,128,200 or 200 150 125)
void MAS::Color::Make(const char *str) {
	char *str2 = new char [ustrsizez(str)];
	char tmp[32];
	int r = 0;
	int g = 0;
	int b = 0;

	if (!ustrcmp(str, uconvert_ascii("-1", tmp))) {
		delete [] str2;
		col = -1;
		return;
	}
	else {
		ustrcpy(str2, str);
		char *tok;

		tok = ustrtok(str2, uconvert_ascii(", ;", tmp));
		if (tok) {
			r = ustrtol(tok, NULL, 10);

			tok = ustrtok(0, uconvert_ascii(", ;", tmp));
			if (tok) {
				g = ustrtol(tok, NULL, 10);

				tok = ustrtok(0, uconvert_ascii(", ;", tmp));
				if (tok) {
					b = ustrtol(tok, NULL, 10);
				}
			}
		}
	}

	delete [] str2;
	Make(r, g, b);
}



void MAS::Color::Make(int r, int g, int b) {
	col = makecol(r,g,b);
}


void MAS::Color::Make(int r, int g, int b, int a) {
	col = makeacol32(r,g,b,a);
}


MAS::Color::operator int() const {
	return col;
}


MAS::Color::operator bool() const {
	return (col >= 0);
}


void MAS::Color::operator=(int c) {
	col = c;
}


int MAS::Color::r() const {
	return getr(col);
}


int MAS::Color::g() const {
	return getg(col);
}


int MAS::Color::b() const {
	return getb(col);
}


int MAS::Color::a() const {
	return geta32(col);
}


void MAS::Color::r(int _r) {
	Make(_r, g(), b());
}


void MAS::Color::g(int _g) {
	Make(r(), _g, b());
}


void MAS::Color::b(int _b) {
	Make(r(), g(), _b);
}


void MAS::Color::a(int _a) {
	Make(r(), g(), b(), _a);
}


void MAS::Color::Colorize(const Color &c, int alpha) {
	float a = (float)alpha/256.0f;

	// convert the pixel to HSV
	float hp, sp, vp;
	ToHSV(hp, sp, vp);

	// convert the color to HSV
	float hc, sc, vc;
	c.ToHSV(hc, sc, vc);

	// make a new rgb pixel
	int r,g,b;
	HSVToRGB(hc, sp*sc, vp*vc, r,g,b);

	// blend the new color with the old one
	r = (int)(r*a + this->r()*(1-a));
	g = (int)(g*a + this->g()*(1-a));
	b = (int)(b*a + this->b()*(1-a));
	Make(r,g,b);
}


MAS::Color MAS::Color::white = 0;
MAS::Color MAS::Color::black = 0;
MAS::Color MAS::Color::red = 0;
MAS::Color MAS::Color::green = 0;
MAS::Color MAS::Color::blue = 0;
MAS::Color MAS::Color::cyan = 0;
MAS::Color MAS::Color::magenta = 0;
MAS::Color MAS::Color::yellow = 0;
MAS::Color MAS::Color::darkgray = 0;
MAS::Color MAS::Color::gray = 0;
MAS::Color MAS::Color::lightgray = 0;
MAS::Color MAS::Color::orange = 0;
MAS::Color MAS::Color::brown = 0;
MAS::Color MAS::Color::purple = 0;
MAS::Color MAS::Color::maroon = 0;
MAS::Color MAS::Color::darkgreen = 0;
MAS::Color MAS::Color::darkblue = 0;


void MAS::Color::OnColorDepthChange() {
	white		= makecol(255,255,255);
	black		= makecol(  0,  0,  0);
	red			= makecol(255,  0,  0);
	green		= makecol(  0,255,  0);
	blue		= makecol(  0,  0,255);
	cyan		= makecol(  0,255,255);
	magenta		= makecol(255,  0,255);
	yellow		= makecol(255,255,  0);
	darkgray	= makecol( 96, 96, 96);
	gray		= makecol(128,128,128);
	lightgray	= makecol(192,192,192);
	orange		= makecol(255,170,  0);
	brown		= makecol(150, 96,  0);
	purple		= makecol(144,  0,168);
	maroon		= makecol(128,  0,  0);
	darkgreen	= makecol(  0,128,  0);
	darkblue	= makecol(  0,  0,128);
}
