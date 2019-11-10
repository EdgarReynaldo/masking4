// example 6: A simlpe piano application.
#include "../include/MASkinG.h"
#include <string.h>
#include <math.h>
using namespace MAS;


// some user defined messages for the piano
#define		MSG_NOTEON		MSG_SUSER+1
#define		MSG_NOTEOFF		MSG_SUSER+2
#define		MSG_NOTECUT		MSG_SUSER+3
#define		MSG_SUSTAINOFF	MSG_SUSER+4

#define		MINNOTE			5


// functions for loading samples
SAMPLE *load_sample_ex(const char *filename);
SAMPLE *load_its(const char *filename);


// the declaration of the piano view widget
class PianoView : public Widget {
	private:
		int keyState[120];
		int octaveLow, octaveHigh;
		bool sustain;

	protected:
		void Draw(Bitmap &bmp);
		void DrawOctave(Bitmap &bmp, int o, int x, int w, int h);
		bool MsgWantfocus();
		bool MsgXChar(int c);
		void MsgTick();

	
	public:
		PianoView();
};


// the declaration of the sample view widget
class SampleView : public Widget {
	private:
		SAMPLE *smp;
	
	protected:
		void Draw(Bitmap &bmp, int x, int y, int w, int h, int channel);
		void MsgDraw();

	public:
		SampleView();
		void SetSample(SAMPLE *smp);
};


// the implementation of the sample loading functions
SAMPLE *load_sample_ex(const char *filename) {
	SAMPLE *ret = NULL;
	int len = ustrsizez(filename);
	char *s = new char[len];
	for (int i=0; i<len; i++) {
		usetat(s, i, utolower(ugetat(filename, i)));
	}
	char *ext = get_extension(s);
	
	if (ustrcmp(ext, "wav") == 0 || ustrcmp(ext, "voc") == 0) {
		ret = load_sample(filename);
	}
	else if (ustrcmp(ext, "its") == 0) {
		ret = load_its(filename);
	}
	
	delete [] s;
	return ret;
}

// loads an Impulse Tracker sample into an Allegro SAMPLE
SAMPLE *load_its(const char *filename) {
	SAMPLE *smp = NULL;
	PACKFILE *file = pack_fopen(filename, "r");
	
	char buf[32];
	int flags;
	int cvt;
	int ptr;
	pack_fread(buf, 4, file);
	buf[4] = 0;
	if (memcmp(buf, "IMPS", 4) == 0) {
		smp = new SAMPLE;
		pack_fread(buf, 14, file);
		flags = pack_getc(file);
		if (flags&0x08) {
			delete smp;
			smp = NULL;
			goto getout;
		}
		smp->bits = (flags&0x02) ? 16 : 8;
		smp->stereo = (flags&0x04) ? 1 : 0;
		pack_fread(buf, 27, file);
		cvt = pack_getc(file);
		pack_fread(buf, 1, file);
		smp->len = pack_igetl(file);
		smp->loop_start = pack_igetl(file);
		smp->loop_end = pack_igetl(file);
		if (smp->loop_end >= smp->len) smp->loop_end = smp->len-1;
		smp->freq = pack_igetl(file);
		pack_fread(buf, 8, file);
		ptr = pack_igetl(file);
		pack_fread(buf, 4, file);
		pack_fseek(file, ptr - 0x50);
		if (smp->bits == 16) {
			smp->data = new short[smp->len];
			for (unsigned int i=0; i<smp->len; i++) {
				((short *)smp->data)[i] = 0x8000 + pack_igetw(file);
			}
		}
		else {
			smp->data = new char[smp->len];
			for (unsigned int i=0; i<smp->len; i++) {
				((char *)smp->data)[i] = 0x80 + pack_getc(file);
			}
		}
	}
getout:
	pack_fclose(file);
	return smp;
}


// implementation of the piano view widget

PianoView::PianoView() : Widget() {
	for (int i=0; i<108; i++) {
		keyState[i] = 0;
	}
	
	octaveLow = 3;
	octaveHigh = 4;
	sustain = false;
}

void PianoView::Draw(Bitmap &bmp) {
	int ow = w()/9;
	bmp.Clear(theSkin->c_back);
	for (int i=0, x=0; i<9; i++, x+=ow) {
		DrawOctave(bmp, i, x, ow, h() - 16);
		if (i == octaveHigh) {
			bmp.Draw3DFrame(x, h()-15, x+ow, h()-9, Color(96,200,160), Color(110,220,180), Color(80,160,140));
		}
		if (i == octaveLow) {
			bmp.Draw3DFrame(x, h()-7, x+ow, h()-1, Color(96,200,160), Color(110,220,180), Color(80,160,140));
		}
	}
	if (HasFocus()) bmp.DrawDottedRect(0, 0, w()-1, h()-1, Color::black);
}


void PianoView::DrawOctave(Bitmap &bmp, int o, int x, int w, int h) {
	int whiteW = w/7;
	int blackW = 3*w/28;
	int i, xx, key;
	
	key = o*12;
	for (i=0, xx=x; i<7; i++, xx+=whiteW) {
		if (keyState[key]) {
			bmp.Draw3DFrame(xx, 0, xx+whiteW, h, Color(250,250,235), Color(160,160,140), Color(255,255,255));
		}
		else {
			bmp.Draw3DFrame(xx, 0, xx+whiteW, h, Color(250,250,235), Color(255,255,255), Color(240,240,220));
		}
		key += key-o*12==4 ? 1 : 2;
	}

	key = o*12+1;
	for (i=0, xx=x+whiteW-blackW/2; i<6; i++, xx+=whiteW) {
		if (i==2) {
			key += 1;
			continue;
		}
		if (!keyState[key]) {
			bmp.Draw3DFrame(xx, 0, xx+blackW, 3*h/5, Color(0,0,0), Color(80,80,96), Color(255,255,255));
		}
		else {
			bmp.Draw3DFrame(xx, 0, xx+blackW, 3*h/5, Color(0,0,0), Color(255,255,255), Color(80,80,96));
		}
		key += 2;
	}
}


bool PianoView::MsgWantfocus() {
	Widget::MsgWantfocus();
	return true;
}


bool PianoView::MsgXChar(int c) {
	Widget::MsgXChar(c);
	
	switch (c>>8) {
		case KEY_LEFT:		octaveLow--;		break;
		case KEY_RIGHT:		octaveLow++;		break;
		case KEY_UP:		octaveHigh++;		break;
		case KEY_DOWN:		octaveHigh--;		break;
		default: return false;
	};

	octaveLow = MID(0, octaveLow, 8);
	octaveHigh = MID(0, octaveHigh, 8);
	Redraw();
	return true;
}


void PianoView::MsgTick() {
	Widget::MsgTick();
	//if (!HasFocus()) return;

	int i;
	bool needToRedraw = false;
	
	static int keyHighArray[] = {
		KEY_Q,
		KEY_2,
		KEY_W,
		KEY_3,
		KEY_E,
		KEY_R,
		KEY_5,
		KEY_T,
		KEY_6,
		KEY_Y,
		KEY_7,
		KEY_U,
		KEY_I,
		KEY_9,
		KEY_O,
		KEY_0,
		KEY_P
	};
	
	static int keyLowArray[] = {
		KEY_Z,
		KEY_S,
		KEY_X,
		KEY_D,
		KEY_C,
		KEY_V,
		KEY_G,
		KEY_B,
		KEY_H,
		KEY_N,
		KEY_J,
		KEY_M
	};

	static int nHigh = 17;
	static int nLow = 12;
	
	bool oldSustain = sustain;
	if (::key[KEY_LSHIFT] || ::key[KEY_RSHIFT]) {
		sustain = true;
	}
	else if (oldSustain) {
		parent->HandleEvent(*this, MSG_SUSTAINOFF);
		sustain = false;
	}
	
	int c;
	for (i=0, c=octaveHigh*12; i<nHigh && c<108; i++, c++) {
		if (!keyState[c] && ::key[keyHighArray[i]]) {
			keyState[c] = MINNOTE;
			parent->HandleEvent(*this, MSG_NOTEON, c);
			needToRedraw = true;
		}
		else if (keyState[c]>1) {
			keyState[c]--;
		}
		else if (keyState[c] && !(::key[keyHighArray[i]])) {
			keyState[c] = 0;
			parent->HandleEvent(*this, (::key[KEY_LSHIFT] || ::key[KEY_RSHIFT]) ? MSG_NOTEOFF : MSG_NOTECUT, c);
			needToRedraw = true;
		}
	}
	
	for (i=0, c=octaveLow*12; i<nLow && c<108; i++, c++) {
		if (!keyState[c] && ::key[keyLowArray[i]]) {
			keyState[c] = MINNOTE;
			parent->HandleEvent(*this, MSG_NOTEON, c);
			needToRedraw = true;
		}
		else if (keyState[c]>1) {
			keyState[c]--;
		}
		else if (keyState[c] && !(::key[keyLowArray[i]])) {
			keyState[c] = 0;
			parent->HandleEvent(*this, (::key[KEY_LSHIFT] || ::key[KEY_RSHIFT]) ? MSG_NOTEOFF : MSG_NOTECUT, c);
			needToRedraw = true;
		}
	}

	if (needToRedraw) Redraw();
}


// implementation of the sample view widget

SampleView::SampleView() : Widget(), smp(NULL) {
}


void SampleView::Draw(Bitmap &bmp, int x, int y, int w, int h, int channel) {
	int half = smp->bits == 8 ? 0x80 : 0x8000;
	int channels = smp->stereo ? 2 : 1;
	int len = smp->len * channels;
	float delta = (float)len / (float)w;
	float delta2;
	float xx = 0.0f;
	int mid = y + h/2;
	int yy, yOld = mid;
	int col = Color(212,255,212);
	int f = (int)(sqrt(len)/40.0f);
	f = (f<1 && len>0) ? 1 : (f<4 ? 2 : f);
	delta2 = delta/(float)f;
	float cx;
	int index;
	if (smp->loop_start != smp->loop_end && smp->loop_end < smp->len) {
		rectfill(bmp, x+smp->loop_start*w/smp->len, y, x+smp->loop_end*w/smp->len, y+h, makecol(16,96,48));
	}
	for (int i=0; i<w-1; ++i) {
		cx = 0.0f;
		for (int j=0; j<f; ++j) {
			index = (int)(xx+cx);
			if (channels == 2) {
				index = (index%2 != channel) ? index + 1 : index;
			}
			yy = smp->bits == 16 ? ((short *)(smp->data))[index] : ((char *)(smp->data))[index];
			yy = yy<0 ? -(yy+half) : -(yy-half);
			yy *= h;
			yy >>= smp->bits;
			yy += mid;
			vline(bmp, x+i, yOld, yy, col);
			yOld = yy;
			cx += delta2;
		}
		xx += delta;
	}
	hline(bmp, x, y+h/2, w-1, makecol(0,255,0));

	if (smp->loop_start != smp->loop_end && smp->loop_end < smp->len) {
		vline(bmp, x+smp->loop_start*w/smp->len, y, y+h, makecol(0,255,212));
		vline(bmp, x+smp->loop_end*w/smp->len, y, y+h, makecol(0,212,255));
	}
}


void SampleView::MsgDraw() {
	Bitmap canvas = GetCanvas();

	if (canvas) {
		canvas.Clear();

		if (smp) {
			if (smp->stereo) {
				Draw(canvas, 0, 0, w(), h()/2, 0);
				Draw(canvas, 0, h()/2, w(), h()/2, 1);
			}
			else {
				Draw(canvas, 0, 0, w(), h(), 0);
			}
		}

		ReleaseCanvas(canvas);
		parent->GetDriver()->InvalidateRect(*this);
	}
	
	ClearFlag(D_DIRTY);
	GetParent()->HandleEvent(*this, MSG_DRAW);
}


void SampleView::SetSample(SAMPLE *smp) {
	this->smp = smp;
	Redraw();
}


// the main dialog

class PianoDialog : public Dialog {
	private:
		ClearScreen desktop;
		Label pathLabel;
		EditBox pathBox;
		Button bQuit, bLoad;
		SampleView smplView;
		PianoView pianoView;

		SAMPLE *smp;
		int voice[108];
	
		void OnSelectSample();
		void OnLoadSample();
		void NoteOn(int n);
		void NoteOff(int n);
		void NoteCut(int n);
		void SustainOff();
	
	public:
		PianoDialog();
		~PianoDialog();
	
		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0);
};


PianoDialog::PianoDialog() : Dialog(), smp(NULL) {
	bQuit.Setup(516, 452, 120, 24, KEY_Q, D_EXIT, "&Quit");
	pathLabel.ClearFlag(D_AUTOSIZE);
	pathLabel.Setup(10, 40, 120, 24, 0, 0, "Path to sample", 2);
	pathBox.Setup(130, 40, 380, 24, 0, 0, NULL, 1024);
	bLoad.Setup(520, 40, 110, 24, KEY_L, 0, "Se&lect");
	smplView.Shape(10, 70, 620, 200);
	pianoView.Shape(5, 280, 630, 100);
	
	Add(desktop);
	Add(pathLabel);
	Add(pathBox);
	Add(bLoad);
	Add(smplView);
	Add(pianoView);
	Add(bQuit);
	
	for (int i=0; i<108; i++) {
		voice[i] = -1;
	}
}


PianoDialog::~PianoDialog() {
	if (smp) {
		destroy_sample(smp);
		smp = NULL;
	}
}


void PianoDialog::HandleEvent(Widget &obj, int msg, int arg1, int arg2) {
	Dialog::HandleEvent(obj, msg, arg1, arg2);
	
	switch (msg) {
		case MSG_ACTIVATE:
			if (obj == bLoad) {
				OnSelectSample();
			}
			else if (obj == pathBox) {
				OnLoadSample();
			}
			break;
			
		case MSG_NOTEON:
			NoteOn(arg1);
			break;
		
		case MSG_NOTEOFF:
			NoteOff(arg1);
			break;

		case MSG_NOTECUT:
			NoteCut(arg1);
			break;

		case MSG_SUSTAINOFF:
			SustainOff();
			break;
	};
}

void PianoDialog::NoteOn(int n) {
	if (!smp) return;
	voice[n] = allocate_voice(smp);
	if (voice[n] == -1) return;
	
	static int periodTable[] = {
	27392,25856,24384,23040,21696,20480,19328,18240,17216,16256,15360,14496,
	13696,12928,12192,11520,10848,10240, 9664, 9120, 8608, 8128, 7680, 7248,
	 6848, 6464, 6096, 5760, 5424, 5120, 4832, 4560, 4304, 4064, 3840, 3624,
	 3424, 3232, 3048, 2880, 2712, 2560, 2416, 2280, 2152, 2032, 1920, 1812,
	 1712, 1616, 1524, 1440, 1356, 1280, 1208, 1140, 1076, 1016,  960,  906,
	  856,  808,  762,  720,  678,  640,  604,  570,  538,  508,  480,  453,
	  428,  404,  381,  360,  339,  320,  302,  285,  269,  254,  240,  226,
	  214,  202,  190,  180,  170,  160,  151,  143,  135,  127,  120,  113,
	  107,  101,   95,   90,   85,   80,   75,   71,   67,   63,   60,   56 };

	if (smp->loop_start != smp->loop_end && smp->loop_end < smp->len) {
		voice_set_playmode(voice[n], PLAYMODE_LOOP);
	}
	voice_set_frequency(voice[n], 14317056L/(8363*periodTable[n]/22050));
	voice_start(voice[n]);
}


void PianoDialog::NoteOff(int n) {
	if (!smp) return;
	if (voice[n] == -1) return;
	release_voice(voice[n]);
	voice[n] = -1;
}


void PianoDialog::NoteCut(int n) {
	if (!smp) return;
	if (voice[n] == -1) return;
	voice_stop(voice[n]);
	deallocate_voice(voice[n]);
	voice[n] = -1;
}


void PianoDialog::SustainOff() {
	if (smp) stop_sample(smp);
}


void PianoDialog::OnSelectSample() {
	FileSelect dlg("Load sample...", pathBox.GetText(), "All samples (*.wav;*.voc;*.its)|Waves (*.wav)|Voc files (*.voc)|IT Samples (*.its)|All files (*.*)", FA_ARCH);
	char *path = dlg.Popup(this);
	if (path) {
		pathBox.SetText(path);
		OnLoadSample();
	}
}

void PianoDialog::OnLoadSample() {
	SAMPLE *smp2 = load_sample_ex(pathBox.GetText());
	if (smp2) {
		if (smp) {
			destroy_sample(smp);
		}
		smp = smp2;
		smplView.SetSample(smp);
	}
}


int main() {
	Error err = InstallMASkinG("allegro.cfg");
	if (err) {
		err.Report();
	}

	PianoDialog *dlg = new PianoDialog;
	dlg->Execute();
	delete dlg;
	
	ExitMASkinG();
	return 0;
}
END_OF_MAIN()
