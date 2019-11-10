////////////////////////////////////////////////////////////////////////////////
//         MASkinG - Miran Amon's Skinnable GUI Library for Allegro           //
//    Copyright (c) 2002-2005 Miran Amon (miranamon@users.sourceforge.net)    //
//          Project websites: http://ferisrv5.uni-mb.si/~ma0747               //
//                  http://sourceforge.net/projects/masking                   //
////////////////////////////////////////////////////////////////////////////////

#include "../include/MASkinG/settings.h"
#include "../include/MASkinG/global.h"


char MAS::Settings::fileName[1024] = "allegro.cfg";
bool MAS::Settings::haveName = false;

char MAS::Settings::skinPath[1024] = "default";
int MAS::Settings::screenWidth = 640;
int MAS::Settings::screenHeight = 480;
int MAS::Settings::gfxMode = GFX_AUTODETECT;
bool MAS::Settings::fullscreen = true;
int MAS::Settings::colorDepth = 16;
int MAS::Settings::refreshRate = 60;
bool MAS::Settings::useVideoMemory = false;
bool MAS::Settings::fullDrag = true;
bool MAS::Settings::fullResize = true;
int MAS::Settings::logicFrameRate = 50;
int MAS::Settings::gfxFrameRate = 50;
bool MAS::Settings::unlimitedFrameRate = false;
bool MAS::Settings::showFrameRate = false;
bool MAS::Settings::antialiasing = true;
bool MAS::Settings::guiSound = true;
int MAS::Settings::soundVolume = 255;
int MAS::Settings::soundDriver = DIGI_AUTODETECT;
int MAS::Settings::midiDriver = MIDI_AUTODETECT;
bool MAS::Settings::showMouse = true;
bool MAS::Settings::mouseShadow = true;
int MAS::Settings::doubleClickDelay = 200;
int MAS::Settings::mouseSpeed = 2;
bool MAS::Settings::useKeyboard = true;
int MAS::Settings::keyDelay = 160;
int MAS::Settings::keyRepeat = 80;
bool MAS::Settings::useJoystick = false;
int MAS::Settings::screenUpdateMethod = 3;
bool MAS::Settings::vsync = false;
bool MAS::Settings::yield = true;
bool MAS::Settings::runInBackground = true;
int MAS::Settings::maxFrameSkip = 5;
int MAS::Settings::tooltipDelay = 200;
int MAS::Settings::tooltipAutohideDelay = 5000;
int MAS::Settings::scrollingSpeed = 3;
bool MAS::Settings::useOpenGLMouseHack = false;

void MAS::Settings::Load(const char *file) {
	if (file) {
		haveName = true;
	}
	else {
		return;
	}

	ustrcpy(fileName, file);
	set_config_file(fileName);

	char section[256];
	usprintf(section, uconvert_ascii("MASkinG", section));

	char var[256];
	char def[256];

	usprintf(skinPath, get_config_string(section, uconvert_ascii("skinPath", var), uconvert_ascii(skinPath, def)));
	screenWidth = get_config_int(section, uconvert_ascii("screenWidth", var), screenWidth);
	screenHeight = get_config_int(section, uconvert_ascii("screenHeight", var), screenHeight);
	refreshRate = get_config_int(section, uconvert_ascii("refreshRate", var), refreshRate);
	colorDepth = get_config_int(section, uconvert_ascii("colorDepth", var), colorDepth);
	gfxMode = get_config_id(section, uconvert_ascii("gfxMode", var), gfxMode);
	fullscreen = get_config_int(section, uconvert_ascii("fullscreen", var), fullscreen) ? true : false;
	useVideoMemory = get_config_int(section, uconvert_ascii("useVideoMemory", var), useVideoMemory) ? true : false;
	fullResize = get_config_int(section, uconvert_ascii("fullResize", var), fullResize) ? true : false;
	fullDrag = get_config_int(section, uconvert_ascii("fullDrag", var), fullDrag) ? true : false;
	logicFrameRate = get_config_int(section, uconvert_ascii("logicFrameRate", var), logicFrameRate);
	gfxFrameRate = get_config_int(section, uconvert_ascii("gfxFrameRate", var), gfxFrameRate);
	unlimitedFrameRate = get_config_int(section, uconvert_ascii("unlimitedFrameRate", var), unlimitedFrameRate) ? true : false;
	showFrameRate = get_config_int(section, uconvert_ascii("showFrameRate", var), showFrameRate) ? true : false;
	antialiasing = get_config_int(section, uconvert_ascii("antialiasing", var), antialiasing) ? true : false;
	guiSound = get_config_int(section, uconvert_ascii("guiSound", var), guiSound) ? true : false;
	soundVolume = get_config_int(section, uconvert_ascii("soundVolume", var), soundVolume);
	soundDriver = get_config_id(section, uconvert_ascii("soundDriver", var), soundDriver);
	midiDriver = get_config_id(section, uconvert_ascii("midiDriver", var), midiDriver);
	showMouse = get_config_int(section, uconvert_ascii("showMouse", var), showMouse) ? true : false;
	mouseShadow = get_config_int(section, uconvert_ascii("mouseShadow", var), mouseShadow) ? true : false;
	doubleClickDelay = get_config_int(section, uconvert_ascii("doubleClickDelay", var), doubleClickDelay);
	mouseSpeed = get_config_int(section, uconvert_ascii("mouseSpeed", var), mouseSpeed);
	useKeyboard = get_config_int(section, uconvert_ascii("useKeyboard", var), useKeyboard) ? true : false;
	keyDelay = get_config_int(section, uconvert_ascii("keyDelay", var), keyDelay);
	keyRepeat = get_config_int(section, uconvert_ascii("keyRepeat", var), keyRepeat);
	useJoystick = get_config_int(section, uconvert_ascii("useJoystick", var), useJoystick) ? true : false;
	screenUpdateMethod = get_config_int(section, uconvert_ascii("screenUpdateMethod", var), screenUpdateMethod);
	vsync = get_config_int(section, uconvert_ascii("vsync", var), vsync) ? true : false;
	yield = get_config_int(section, uconvert_ascii("yield", var), yield) ? true : false;
	runInBackground = get_config_int(section, uconvert_ascii("runInBackground", var), runInBackground) ? true : false;
	maxFrameSkip = get_config_int(section, uconvert_ascii("maxFrameSkip", var), maxFrameSkip);
	tooltipDelay = get_config_int(section, uconvert_ascii("tooltipDelay", var), tooltipDelay);
	tooltipAutohideDelay = get_config_int(section, uconvert_ascii("tooltipAutohideDelay", var), tooltipAutohideDelay);
	scrollingSpeed = get_config_int(section, uconvert_ascii("scrollingSpeed", var), scrollingSpeed);
}


void MAS::Settings::Save() {
	if (haveName) {
		set_config_file(fileName);
	}
	else {
		return;
	}

	char section[256];
	usprintf(section, uconvert_ascii("MASkinG", section));

	char var[256];

	set_config_string(section, uconvert_ascii("skinPath", var), skinPath);
	set_config_int(section, uconvert_ascii("screenWidth", var), screenWidth);
	set_config_int(section, uconvert_ascii("screenHeight", var), screenHeight);
	set_config_int(section, uconvert_ascii("refreshRate", var), refreshRate);
	set_config_int(section, uconvert_ascii("colorDepth", var), colorDepth);
	set_config_id(section, uconvert_ascii("gfxMode", var), gfxMode);
	set_config_int(section, uconvert_ascii("fullscreen", var), fullscreen);
	set_config_int(section, uconvert_ascii("useVideoMemory", var), useVideoMemory);
	set_config_int(section, uconvert_ascii("fullDrag", var), fullDrag);
	set_config_int(section, uconvert_ascii("fullResize", var), fullResize);
	set_config_int(section, uconvert_ascii("logicFrameRate", var), logicFrameRate);
	set_config_int(section, uconvert_ascii("gfxFrameRate", var), gfxFrameRate);
	set_config_int(section, uconvert_ascii("unlimitedFrameRate", var), unlimitedFrameRate);
	set_config_int(section, uconvert_ascii("showFrameRate", var), showFrameRate);
	set_config_int(section, uconvert_ascii("antialiasing", var), antialiasing);
	set_config_int(section, uconvert_ascii("guiSound", var), guiSound);
	set_config_int(section, uconvert_ascii("soundVolume", var), soundVolume);
	set_config_id(section, uconvert_ascii("soundDriver", var), soundDriver);
	set_config_id(section, uconvert_ascii("midiDriver", var), midiDriver);
	set_config_int(section, uconvert_ascii("showMouse", var), showMouse);
	set_config_int(section, uconvert_ascii("mouseShadow", var), mouseShadow);
	set_config_int(section, uconvert_ascii("doubleClickDelay", var), doubleClickDelay);
	set_config_int(section, uconvert_ascii("mouseSpeed", var), mouseSpeed);
	set_config_int(section, uconvert_ascii("useKeyboard", var), useKeyboard);
	set_config_int(section, uconvert_ascii("keyDelay", var), keyDelay);
	set_config_int(section, uconvert_ascii("keyRepeat", var), keyRepeat);
	set_config_int(section, uconvert_ascii("useJoystick", var), useJoystick);
	set_config_int(section, uconvert_ascii("screenUpdateMethod", var), screenUpdateMethod);
	set_config_int(section, uconvert_ascii("vsync", var), vsync);
	set_config_int(section, uconvert_ascii("yield", var), yield);
	set_config_int(section, uconvert_ascii("runInBackground", var), runInBackground);
	set_config_int(section, uconvert_ascii("maxFrameSkip", var), maxFrameSkip);
	set_config_int(section, uconvert_ascii("tooltipDelay", var), tooltipDelay);
	set_config_int(section, uconvert_ascii("tooltipAutohideDelay", var), tooltipAutohideDelay);
	set_config_int(section, uconvert_ascii("scrollingSpeed", var), scrollingSpeed);

	flush_config_file();
}
