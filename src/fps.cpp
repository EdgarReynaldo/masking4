#include "../include/MASkinG/fps.h"
using namespace MAS;


MAS::CFPS MAS::globalFpsCounter;


CFPS::CFPS() : samples(NULL) {
	Init(0);
}


CFPS::~CFPS() {
	if (samples) {
		delete [] samples;
		samples = NULL;
	}
	nSamples = 0;
	frameCounter = 0;
}


void CFPS::Init(int fps) {
	if (samples) {
		delete [] samples;
		samples = NULL;
	}
	nSamples = fps;
	samples = fps ? new int[nSamples] : NULL;
	for (int i=0; i<nSamples; i++) {
		samples[i] = 1;
	}
	curSample = 0;
	frameCounter = 0;
	framesPerSecond = fps;
}


void CFPS::Tick() {
	curSample++;
	curSample %= nSamples;
	framesPerSecond -= samples[curSample];
	framesPerSecond += frameCounter;
	samples[curSample] = frameCounter;
	frameCounter = 0;
}


void CFPS::Frame() {
	++frameCounter;
}


int CFPS::Get() {
	return framesPerSecond;
}

void CFPS::Draw(BITMAP *buffer) {
#if ALLEGRO_SUB_VERSION < 1
	text_mode(makecol(0,0,0));
	textprintf(buffer, font, 0, 0, makecol(255,255,255), "%d CFPS", Get());
	text_mode(-1);
#else
	textprintf_ex(buffer, font, 0, 0, makecol(255,255,255), makecol(0,0,0), "%d CFPS", Get());
#endif
}
