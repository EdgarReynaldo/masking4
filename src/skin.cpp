////////////////////////////////////////////////////////////////////////////////
//         MASkinG - Miran Amon's Skinnable GUI Library for Allegro           //
//    Copyright (c) 2002-2005 Miran Amon (miranamon@users.sourceforge.net)    //
//          Project websites: http://ferisrv5.uni-mb.si/~ma0747               //
//                  http://sourceforge.net/projects/masking                   //
////////////////////////////////////////////////////////////////////////////////

#include "../include/MASkinG/skin.h"
#include "../include/MASkinG/settings.h"
#include "../include/MASkinG/global.h"

const int MAS::Skin::nBitmaps = 46;
const int MAS::Skin::nCursors = 12;
const int MAS::Skin::nFonts = 8;
const int MAS::Skin::nSamples = 7;
const int MAS::Skin::nBitmapsEx = 1024;
const int MAS::Skin::nCursorsEx = 64;
const int MAS::Skin::nFontsEx = 64;
const int MAS::Skin::nSamplesEx = 1024;


static char *bitmapName[] = { "BOX", "BOX_SHADDOW", "BUTTON", "CHECKBOX", "CLEAR_BACK",
	"COMBO_BUTTON", "HSLIDER_BACK", "HSLIDER_GRIP", "ICONEXIT",
	"ICONMAX", "ICONMIN", "ICONRESTORE", "LIST", "MENU_BACK", "MENU_BUTTON",
	"MENU_ITEM", "PANEL_GROOVE", "PANEL_RAISED", "PANEL_RIDGE", "PANEL_SUNKEN",
	"PROGRESSH", "PROGRESSV", "RADIO", "SCROLL_DOWN", "SCROLL_HBACK", "SCROLL_HGRIP",
	"SCROLL_HGRIPOVERLAY", "SCROLL_LEFT", "SCROLL_RIGHT", "SCROLL_UP", "SCROLL_VBACK",
	"SCROLL_VGRIP", "SCROLL_VGRIPOVERLAY", "SEPARATORH", "SEPARATORV", "TAB_BUTTON",
	"TAB_WINDOW", "VSLIDER_BACK", "VSLIDER_GRIP", "WINBOTTOM", "WINDOW", "WINGRIP",
	"WINLEFT", "WINRIGHT", "WINTEXT", "WINTOP" };

static char *cursorName[] = {
	"MOUSE_ALTERNATIVE",
	"MOUSE_CROSSHAIR",
	"MOUSE_HOURGLASS",
	"MOUSE_MOVE",
	"MOUSE_NORMAL",
	"MOUSE_SELECT",
	"MOUSE_SIZE_DIAGONAL1",
	"MOUSE_SIZE_DIAGONAL2",
	"MOUSE_SIZE_HORIZONTAL",
	"MOUSE_SIZE_VERTICAL",
	"MOUSE_TEXTSELECT",
	"MOUSE_UNAVAILABLE"
};

////////////////////////////////////////////////////////////////////////////////
// The default constructor: resets colors, fonts and bitmaps to default values
MAS::Skin::Skin(const char *fileName) {
	int i, j;
	lastError = Error(Error::NONE);
	skinFilePath = NULL;

	// fonts
	fntList.reserve(nFontsEx);
	for (i=0; i<nFontsEx; i++) {
		fntList.push_back(new MAS::Font);
	}

	// bitmaps
	bmpList.reserve(nBitmapsEx);
	for (i=0; i<nBitmapsEx; i++) {
		bmpList.push_back(new MAS::Bitmap);
	}

	// cursors
	curList.reserve(nCursorsEx);
	for (i=0; i<nCursorsEx; i++) {
		curList.push_back(new MAS::Cursor);
	}

	// samples
	smpList.reserve(nSamplesEx);
	for (i=0; i<nSamplesEx; i++) {
		smpList.push_back(new MAS::Sample);
	}

	// font info
	for (i=0; i<nInfoItems; i++) {
		for (j=0; j<4; j++) {
			fnt[i][j] = 0;
			fcol[i][j] = Color::black;
			scol[i][j] = -1;
		}
	}

	char tmp[64];
	if (ustrcmp(fileName, uconvert_ascii("default", tmp)) != 0 && ustrcmp(fileName, empty_string) != 0) {
		Load(fileName);
	}
	else {
		Reset();
	}
}


////////////////////////////////////////////////////////////////////////////////
// Destructor (should unload data but that seems to crash the program)
MAS::Skin::~Skin() {
	if (skinFilePath) {
		delete [] skinFilePath;
	}

	Unload();

	int i;
	for (i=0; i<nBitmapsEx; i++) {
		delete bmpList[i];
	}
	bmpList.clear();

	for (i=0; i<nFontsEx; i++) {
		delete fntList[i];
	}
	fntList.clear();

	for (i=0; i<nCursorsEx; i++) {
		delete curList[i];
	}
	curList.clear();

	for (i=0; i<nSamplesEx; i++) {
		delete smpList[i];
	}
	smpList.clear();
}


////////////////////////////////////////////////////////////////////////////////
// A helper function for setting the skin colors to defaults
void MAS::Skin::ResetColors() {
	// Global skin colors
	c_face =	MAS::Color(210,210,210);
	c_font =	MAS::Color( 16, 16, 16);
	c_shad1 =	MAS::Color(255,255,255);
	c_shad2 =	MAS::Color( 80, 80, 80);
	c_disable =	MAS::Color(128,128,128);
	c_select =	MAS::Color(128,192,128);
	c_deselect =MAS::Color(224,224,224);
	c_focus =	MAS::Color(128,192,128);
	c_sunken =	MAS::Color(232,232,232);
	c_back =	MAS::Color(180,180,180);

	// Font info for widgets
	for (int i=0; i<nInfoItems; i++) {
		fcol[i][NORMAL] = c_font;
		fcol[i][SELECT] = c_font;
		fcol[i][DISABLE] = c_disable;
		fcol[i][FOCUS] = c_font;
		scol[i][NORMAL] = -1;
		scol[i][SELECT] = -1;
		scol[i][DISABLE] = c_shad1;
		scol[i][FOCUS] = -1;
	}

	// The hyper-text is a special case
	fcol[INFO_HYPER][NORMAL] = Color(0,0,128);
	fcol[INFO_HYPER][SELECT] =  Color(128,0,128);
	fcol[INFO_HYPER][FOCUS] =  Color(128,0,128);
}


////////////////////////////////////////////////////////////////////////////////
// Loads and processes the skin description file which is in fact a regular
// Allegro configuration file
MAS::Error MAS::Skin::Load(const char *file) {
	int i = 0;
	lastError = MAS::Error(MAS::Error::NONE);

	if (!file) {
		lastError =  MAS::Error(MAS::Error::SKIN_INI);
		return lastError;
	}

	// Try to open the skin description file
	if (!file_exists(file, FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH, NULL)) {
		lastError = MAS::Error(MAS::Error::SKIN_INI);
		return lastError;
	}
	else {
		set_config_file(file);
	}

	Reset();

	// remember the path
	if (skinFilePath != file) {
		if (skinFilePath) delete [] skinFilePath;
		skinFilePath = new char[ustrsizez(file)];
		ustrcpy(skinFilePath, file);
	}

	// Get the path to the skin configuration file (without the actual filename)
	char path[2048];
	replace_filename(path, file, empty_string, 2048);

	char working[2048];
	bool default_bitmaps = false;
	////////////////////////////////////////////////////////////////////////////////
	// Read the name of the dat file and load the bitmap data
	char main_data[2048];
	char section[256];
	usprintf(section, uconvert_ascii("Skin", section));
	char var[256];
	char def[256];
	if (get_config_string(section, uconvert_ascii("main_data", var), NULL)) {
		usprintf(working, get_config_string(section, uconvert_ascii("main_data", var), NULL));
		append_filename(main_data, path, working, 2048);
		lastError = LoadData(main_data);
		if (lastError) return lastError;
	}
	else {
		default_bitmaps = true;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Read the default colors
	usprintf(section, uconvert_ascii("COLORS", section));
	c_face	   = MAS::Color(get_config_string(section, uconvert_ascii("face", var),	uconvert_ascii("210,210,210", def)));
	c_font	   = MAS::Color(get_config_string(section, uconvert_ascii("fontface", var),uconvert_ascii(" 16, 16, 16", def)));
	c_shad1	   = MAS::Color(get_config_string(section, uconvert_ascii("shad1", var),	uconvert_ascii("255,255,255", def)));
	c_shad2	   = MAS::Color(get_config_string(section, uconvert_ascii("shad2", var),	uconvert_ascii(" 80, 80, 80", def)));
	c_disable  = MAS::Color(get_config_string(section, uconvert_ascii("disable", var),	uconvert_ascii("128,128,128", def)));
	c_select   = MAS::Color(get_config_string(section, uconvert_ascii("select", var),	uconvert_ascii("128,192,128", def)));
	c_deselect = MAS::Color(get_config_string(section, uconvert_ascii("deselect", var),uconvert_ascii("224,224,224", def)));
	c_focus	   = MAS::Color(get_config_string(section, uconvert_ascii("focus", var),	uconvert_ascii("128,192,128", def)));
	c_sunken   = MAS::Color(get_config_string(section, uconvert_ascii("sunken", var),	uconvert_ascii("232,232,232", def)));
	c_back	   = MAS::Color(get_config_string(section, uconvert_ascii("back", var),	uconvert_ascii("180,180,180", def)));

	if (default_bitmaps) {
		GenerateDefaultBitmaps();
	}

	////////////////////////////////////////////////////////////////////////////////
	// Read the cursors from another datafile
	if (get_config_string(uconvert_ascii("Skin", section), uconvert_ascii("cursors", var), NULL)) {
		usprintf(working, get_config_string(uconvert_ascii("Skin", section), uconvert_ascii("cursors", var), NULL));
		append_filename(main_data, path, working, 2048);
		lastError = LoadCursors(main_data);
		if (lastError) return lastError;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Read the samples from another datafile
	if (get_config_string(uconvert_ascii("Skin", section), uconvert_ascii("sounds", var), NULL)) {
		usprintf(working, get_config_string(uconvert_ascii("Skin", section), uconvert_ascii("sounds", var), NULL));
		append_filename(main_data, path, working, 2048);
		lastError = LoadSamples(main_data);
		if (lastError) return lastError;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Now load the fonts
	const char *fonts[] = { "FONT0", "FONT1", "FONT2", "FONT3", "FONT4", "FONT5", "FONT6", "FONT7" };
	const char *ttfont;
	int size;

	for (i=0; i<nFonts; i++)	{
		size = get_config_int(uconvert_ascii(fonts[i], section), uconvert_ascii("size", var), 10);			// size of the font
		ttfont = get_config_string(uconvert_ascii(fonts[i], section), uconvert_ascii("file", var), uconvert_ascii("none", def));	// the filename of the font
		append_filename(main_data, path, ttfont, 2048);
		fntList[i]->Load(main_data);
		fntList[i]->SetSize(size);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Read the object specific data:
	//----------------------------------------------
	const char *fnames[] = { "font1", "font2", "font3", "font4" };
	const char *cnames[] = { "f_col1", "f_col2", "f_col3", "f_col4" };
	const char *snames[] = { "s_col1", "s_col2", "s_col3", "s_col4" };
	const char *def_col[] = { "0,0,0", "0,0,0", "128,128,128", "0,0,0" };
	const char *def_shd[] = { "-1", "-1", "-1", "-1" };

	// button, checkbox, radio button, etc.
	const char *info_name[] = {
		"BOX",
		"BUTTON",
		"CHECKBOX",
		"RADIO",
		"LIST",
		"WINDOW",
		"MENU",
		"TEXT",
		"EDITBOX",
		"PROGRESS",
		"HYPER",
		"TAB",
		"TOOLTIP"
	};

	for (int j=0; j<nInfoItems; j++)	{
		for (i=0; i<4; i++)	{
			fnt[j][i] = get_config_int(uconvert_ascii(info_name[j], section), uconvert_ascii(fnames[i], var), 0);
			fcol[j][i] = Color(get_config_string(uconvert_ascii(info_name[j], section),	uconvert_ascii(cnames[i], var), uconvert_ascii(def_col[i], def)));
			scol[j][i] = Color(get_config_string(uconvert_ascii(info_name[j], section),	uconvert_ascii(snames[i], var), uconvert_ascii(def_shd[i], def)));
		}
	}

	/*
	for (i=0; i<4; i++)	{
		fnt[INFO_BUTTON][i]	=    get_config_int(uconvert_ascii("BUTTON", section),	uconvert_ascii(fnames[i], var),		0);
		fnt[INFO_CHECKBOX][i] =  get_config_int(uconvert_ascii("CHECKBOX", section),uconvert_ascii(fnames[i], var),		0);
		fnt[INFO_RADIO][i] =     get_config_int(uconvert_ascii("RADIO", section),	uconvert_ascii(fnames[i], var),		0);
		fnt[INFO_LIST][i] =      get_config_int(uconvert_ascii("LIST", section),	uconvert_ascii(fnames[i], var),		0);
		fnt[INFO_TEXT][i] =      get_config_int(uconvert_ascii("TEXT", section),	uconvert_ascii(fnames[i], var),		0);
		fnt[INFO_MENU][i] =      get_config_int(uconvert_ascii("MENU", section),	uconvert_ascii(fnames[i], var),		0);
		fnt[INFO_BAR][i] =       get_config_int(uconvert_ascii("PROGRESS", section),uconvert_ascii(fnames[i], var),		0);
		fnt[INFO_WINDOW][i]	=    get_config_int(uconvert_ascii("WINDOW", section),	uconvert_ascii(fnames[i], var),		0);
		fnt[INFO_HYPER][i] =     get_config_int(uconvert_ascii("HYPER", section),	uconvert_ascii(fnames[i], var),		0);
		fnt[INFO_TAB][i] =		 get_config_int(uconvert_ascii("TAB", section),		uconvert_ascii(fnames[i], var),		0);
		fnt[INFO_TOOLTIP][i] =	 get_config_int(uconvert_ascii("TOOLTIP", section),	uconvert_ascii(fnames[i], var),		0);
		fcol[INFO_BUTTON][i] =   Color(get_config_string(uconvert_ascii("BUTTON", section),	uconvert_ascii(cnames[i], var), uconvert_ascii(def_col[i], def)));
		fcol[INFO_CHECKBOX][i] = Color(get_config_string(uconvert_ascii("CHECKBOX", section),uconvert_ascii(cnames[i], var), uconvert_ascii(def_col[i], def)));
		fcol[INFO_RADIO][i]	=    Color(get_config_string(uconvert_ascii("RADIO", section),	uconvert_ascii(cnames[i], var), uconvert_ascii(def_col[i], def)));
		fcol[INFO_LIST][i] =     Color(get_config_string(uconvert_ascii("LIST", section),	uconvert_ascii(cnames[i], var), uconvert_ascii(def_col[i], def)));
		fcol[INFO_TEXT][i] =     Color(get_config_string(uconvert_ascii("TEXT", section),	uconvert_ascii(cnames[i], var), uconvert_ascii(def_col[i], def)));
		fcol[INFO_MENU][i] =     Color(get_config_string(uconvert_ascii("MENU", section),	uconvert_ascii(cnames[i], var), uconvert_ascii(def_col[i], def)));
		fcol[INFO_BAR][i] =      Color(get_config_string(uconvert_ascii("PROGRESS", section),uconvert_ascii(cnames[i], var), uconvert_ascii(def_col[i], def)));
		fcol[INFO_WINDOW][i] =   Color(get_config_string(uconvert_ascii("WINDOW", section),	uconvert_ascii(cnames[i], var), uconvert_ascii(def_col[i], def)));
		fcol[INFO_HYPER][i]	=    Color(get_config_string(uconvert_ascii("HYPER", section),	uconvert_ascii(cnames[i], var), uconvert_ascii(def_col[i], def)));
		fcol[INFO_TAB][i]	=    Color(get_config_string(uconvert_ascii("TAB", section),	uconvert_ascii(cnames[i], var), uconvert_ascii(def_col[i], def)));
		fcol[INFO_TOOLTIP][i] =  Color(get_config_string(uconvert_ascii("TOOLTIP", section),uconvert_ascii(cnames[i], var), uconvert_ascii(def_col[i], def)));
		scol[INFO_BUTTON][i] =   Color(get_config_string(uconvert_ascii("BUTTON", section),	uconvert_ascii(snames[i], var), uconvert_ascii(def_shd[i], def)));
		scol[INFO_CHECKBOX][i] = Color(get_config_string(uconvert_ascii("CHECKBOX", section),uconvert_ascii(snames[i], var), uconvert_ascii(def_shd[i], def)));
		scol[INFO_RADIO][i]	=    Color(get_config_string(uconvert_ascii("RADIO", section),	uconvert_ascii(snames[i], var), uconvert_ascii(def_shd[i], def)));
		scol[INFO_LIST][i] =     Color(get_config_string(uconvert_ascii("LIST", section),	uconvert_ascii(snames[i], var), uconvert_ascii(def_shd[i], def)));
		scol[INFO_TEXT][i] =     Color(get_config_string(uconvert_ascii("TEXT", section),	uconvert_ascii(snames[i], var), uconvert_ascii(def_shd[i], def)));
		scol[INFO_MENU][i] =     Color(get_config_string(uconvert_ascii("MENU", section),	uconvert_ascii(snames[i], var), uconvert_ascii(def_shd[i], def)));
		scol[INFO_BAR][i] =      Color(get_config_string(uconvert_ascii("PROGRESS", section),uconvert_ascii(snames[i], var), uconvert_ascii(def_shd[i], def)));
		scol[INFO_WINDOW][i] =   Color(get_config_string(uconvert_ascii("WINDOW", section),	uconvert_ascii(snames[i], var), uconvert_ascii(def_shd[i], def)));
		scol[INFO_HYPER][i]	=    Color(get_config_string(uconvert_ascii("HYPER", section),	uconvert_ascii(snames[i], var), uconvert_ascii(def_shd[i], def)));
		scol[INFO_TAB][i]	=    Color(get_config_string(uconvert_ascii("TAB", section),	uconvert_ascii(snames[i], var), uconvert_ascii(def_shd[i], def)));
		scol[INFO_TOOLTIP][i] =  Color(get_config_string(uconvert_ascii("TOOLTIP", section),uconvert_ascii(snames[i], var), uconvert_ascii(def_shd[i], def)));
	}

	// box
	fnt[INFO_BOX][NORMAL] =	  get_config_int(section, uconvert_ascii("font1", var),		0);
	fcol[INFO_BOX][NORMAL] =  Color(get_config_string(uconvert_ascii("BOX", section), uconvert_ascii("f_col1", var),	uconvert_ascii("32,32,32", def)));
	fcol[INFO_BOX][DISABLE] = Color(get_config_string(uconvert_ascii("BOX", section), uconvert_ascii("f_col2", var),	uconvert_ascii("128,128,128", def)));
	scol[INFO_BOX][NORMAL] =  Color(get_config_string(uconvert_ascii("BOX", section), uconvert_ascii("s_col1", var),	uconvert_ascii("-1", def)));
	scol[INFO_BOX][DISABLE] = Color(get_config_string(uconvert_ascii("BOX", section), uconvert_ascii("s_col2", var),	uconvert_ascii("255,255,255", def)));
	*/

	// wallpaper style
	wallpaperStyle = get_config_int(uconvert_ascii("WALLPAPER", section), uconvert_ascii("style", var), 3);

	// buttons animation
	buttonDisplacement = get_config_int(uconvert_ascii("BUTTON", section), uconvert_ascii("displacement", var), 1);
	buttonAnimationType = get_config_int(uconvert_ascii("BUTTON", section), uconvert_ascii("animationType", var), 0);
	buttonAnimationLength = get_config_int(uconvert_ascii("BUTTON", section), uconvert_ascii("animationLength", var), 0);

	// group box info
	boxX =		get_config_int(uconvert_ascii("BOX", section), uconvert_ascii("offX", var), 6);
	boxY =		get_config_int(uconvert_ascii("BOX", section), uconvert_ascii("offY", var), 1);
	boxBack =	Color(get_config_string(uconvert_ascii("BOX", section), uconvert_ascii("backColor", var), uconvert_ascii("222,222,222", def)));
	boxAlign =	get_config_int(uconvert_ascii("BOX", section), uconvert_ascii("alignment", var), 0);

	// window
	usprintf(section, uconvert_ascii("WINDOW", section));
	winTextPos.x(get_config_int(section, uconvert_ascii("textX", var), 10));
	winTextPos.y(get_config_int(section, uconvert_ascii("textY", var),  8));
	winTextAlign = get_config_int(section, uconvert_ascii("alignment", var), 0);
	winTextBack = get_config_int(section, uconvert_ascii("textBack", var), 0);
	winExitPos.x(get_config_int(section, uconvert_ascii("exitX", var), -22));
	winExitPos.y(get_config_int(section, uconvert_ascii("exitY", var), 6));
	winMaxPos.x(get_config_int(section, uconvert_ascii("maxX", var), -40));
	winMaxPos.y(get_config_int(section, uconvert_ascii("maxY", var), 6));
	winMinPos.x(get_config_int(section, uconvert_ascii("minX", var), -56));
	winMinPos.y(get_config_int(section, uconvert_ascii("minY", var), 6));
	winAnimationType = get_config_int(section,	uconvert_ascii("animationType", var), 0);
	winAnimationLength = get_config_int(section, uconvert_ascii("animationLength", var), 0);

	// combo box
	comboAnimationType = get_config_int(uconvert_ascii("COMBOBOX", section), uconvert_ascii("animationType", var), 0);
	comboAnimationLength = get_config_int(uconvert_ascii("COMBOBOX", section), uconvert_ascii("animationLength", var), 0);

	// menu
	usprintf(section, uconvert_ascii("MENU", section));
	menuHeight = get_config_int(section, uconvert_ascii("height", var),	16);
	menuDelay =	get_config_int(section,	uconvert_ascii("delay", var), 300);
	menuAnimationType = get_config_int(section,	uconvert_ascii("animationType", var), 0);
	menuAnimationLength = get_config_int(section, uconvert_ascii("animationLength", var), 0);

	// tooltip info
	tooltipAnimationType = get_config_int(uconvert_ascii("TOOLTIP", section), uconvert_ascii("animationType", var), 0);
	tooltipAnimationLength = get_config_int(uconvert_ascii("TOOLTIP", section), uconvert_ascii("animationLength", var), 0);
	tooltipBack =	Color(get_config_string(uconvert_ascii("TOOLTIP", section), uconvert_ascii("backColor", var), uconvert_ascii("255,255,192", def)));
	tooltipBorder =	Color(get_config_string(uconvert_ascii("TOOLTIP", section), uconvert_ascii("borderColor", var), uconvert_ascii("0,0,0", def)));

	// dialog popup animation
	usprintf(section, uconvert_ascii("DIALOG", section));

	// should the GUI draw dotted rectangles?
	drawDots = get_config_int(uconvert_ascii("Skin", section), uconvert_ascii("dottedRect", var), 0);

	// how the focus follows the mouse
	focus = get_config_int(uconvert_ascii("Skin", section), uconvert_ascii("focus", var), 1);

	// finally change the color of the first palette entry to 0
	RGB black = { 0,  0,  0  };
	set_color(0, &black);

	return MAS::Error(MAS::Error::NONE);
}


void MAS::Skin::Reload() {
	char tmp[64];
	if (skinFilePath && ustrcmp(skinFilePath, uconvert_ascii("default", tmp)) != 0 && ustrcmp(skinFilePath, empty_string) != 0) {
		Load(skinFilePath);
	}
	else {
		Reset();
	}
}


////////////////////////////////////////////////////////////////////////////////
// A helper function for loading an Allegro dat file where bitmaps are
// supposed to be stored.
MAS::Error MAS::Skin::LoadData(const char *file) {
	if (!file_exists(file, FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH, NULL)) {
		return Error(Error::SKIN_DAT);
	}

	// load the datafile
	set_color_conversion(COLORCONV_TOTAL);
	DATAFILE *dataBitmaps = load_datafile(file);
	if (!dataBitmaps) {
		return Error(Error::LOAD_FILE);
	}

	DATAFILE *d;
	int i;
	const char *text;

	char tmp[256];

	// Try to load the palette
	d = find_datafile_object(dataBitmaps, uconvert_ascii("XTRA_PALETTE", tmp));
	if (d != NULL) {
		set_palette(*(PALETTE *)d->dat);
		unload_datafile(dataBitmaps);
		dataBitmaps = load_datafile(file);
	}

	// Look for each bitmap inside the dat file and load it if it exists
	for (i=0; i<nBitmaps; i++) {
		d = find_datafile_object(dataBitmaps, uconvert_ascii(bitmapName[i], tmp));
		if (d != NULL) {
			/*
			if (MAS::Settings::useVideoMemory) {
				bmpList[i]->Set((BITMAP *)d->dat, true, Bitmap::VIDEO);
				if (!(*bmpList[i])) {
					bmpList[i]->Set((BITMAP *)d->dat, true, Bitmap::SYSTEM);
					if (!(*bmpList[i])) {
						bmpList[i]->Set((BITMAP *)d->dat, true, Bitmap::MEMORY);
					}
				}
			}
			else {
				bmpList[i]->Set((BITMAP *)d->dat, true, Bitmap::MEMORY);
			}
			*/
			//bmpList[i]->Set((BITMAP *)d->dat, true, Bitmap::MEMORY);
			bmpList[i]->Set((BITMAP *)d->dat, true, MAS::Settings::useVideoMemory ? Bitmap::VIDEO : Bitmap::MEMORY);

			usprintf(tmp, "%s_TCKX", bitmapName[i]);
			bmpList[i]->ThickX(get_config_int("TILING", tmp, -1));
			if (bmpList[i]->ThickX() == -1) {
				text = get_datafile_property(d, DAT_ID('T','C','K','X'));
				bmpList[i]->ThickX((ustricmp(text, empty_string) == 0) ? -1 : ustrtol(text, NULL, 10));
			}

			usprintf(tmp, "%s_TCKY", bitmapName[i]);
			bmpList[i]->ThickY(get_config_int("TILING", tmp, -1));
			if (bmpList[i]->ThickY() == -1) {
				text = get_datafile_property(d, DAT_ID('T','C','K','Y'));
				bmpList[i]->ThickY((ustricmp(text, empty_string) == 0) ? -1 : ustrtol(text, NULL, 10));
			}
		}
	}

	unload_datafile(dataBitmaps);
	return Error(Error::NONE);
}


////////////////////////////////////////////////////////////////////////////////
// A helper function for loading an Allegro dat file where cursors are
// supposed to be stored.
MAS::Error MAS::Skin::LoadCursors(const char *file) {
	if (!file || !file_exists(file, FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH, NULL)) {
		return Error(Error::NO_FILE);
	}

	MAS::Cursor::ReadSkinData();

	set_color_conversion(COLORCONV_TOTAL);
	DATAFILE *dataCursors = load_datafile(file);
	if (!dataCursors) {
		return Error(Error::LOAD_FILE);
	}

	DATAFILE *d;
	int i;

	char tmp[256];
	int x, y, frames, interval;
	const char *text;

	// Look for each bitmap inside the dat file and load it if it exists
	for (i=0; i<nCursors; i++) {
		d = find_datafile_object(dataCursors, uconvert_ascii(cursorName[i], tmp));
		if (d != NULL) {
			usprintf(tmp, "%s_FCSX", cursorName[i]);
			x = get_config_int("CURSOR", tmp, -1);
			if (x == -1) {
				text = get_datafile_property(d, DAT_ID('F','C','S','X'));
				x = (ustricmp(text, empty_string) == 0) ? 0 : ustrtol(text, NULL, 10);
			}

			usprintf(tmp, "%s_FCSY", cursorName[i]);
			y = get_config_int("CURSOR", tmp, -1);
			if (y == -1) {
				text = get_datafile_property(d, DAT_ID('F','C','S','Y'));
				y = (ustricmp(text, empty_string) == 0) ? 0 : ustrtol(text, NULL, 10);
			}

			usprintf(tmp, "%s_FRMS", cursorName[i]);
			frames = get_config_int("CURSOR", tmp, -1);
			if (frames == -1) {
				text = get_datafile_property(d, DAT_ID('F','R','M','S'));
				frames = (ustricmp(text, empty_string) == 0) ? 1 : ustrtol(text, NULL, 10);
			}

			usprintf(tmp, "%s_FDUR", cursorName[i]);
			interval = get_config_int("CURSOR", tmp, -1);
			if (interval == -1) {
				text = get_datafile_property(d, DAT_ID('F','D','U','R'));
				interval = (ustricmp(text, empty_string) == 0) ? 20 : ustrtol(text, NULL, 10);
			}

			curList[i]->Create((BITMAP *)d->dat, frames);
			curList[i]->SetFocus(x, y);
			curList[i]->SetAnimationInterval(interval);
		}
	}

	unload_datafile(dataCursors);
	return Error(Error::NONE);
}


////////////////////////////////////////////////////////////////////////////////
// A helper function for loading an Allegro dat file where samples are
// supposed to be stored.
MAS::Error MAS::Skin::LoadSamples(const char *file) {
	if (!file || !file_exists(file, FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH, NULL))
		return Error(Error::NO_FILE);

	DATAFILE *dataSamples = load_datafile(file);
	if (!dataSamples) {
		return Error(Error::LOAD_FILE);
	}

	DATAFILE *d;
	int i;

	char *sampleName[] = {
		"SAMPLE_ACTIVATE",
		"SAMPLE_CLOSE",
		"SAMPLE_GOTFOCUS",
		"SAMPLE_KEY",
		"SAMPLE_LOSTFOCUS",
		"SAMPLE_OPEN",
		"SAMPLE_SCROLL"
	};

	// Look for each sample inside the dat file and load it if it exists
	char tmp[256];
	for (i=0; i<nSamples; i++) {
		d = find_datafile_object(dataSamples, uconvert_ascii(sampleName[i], tmp));
		if (d != NULL) {
			smpList[i]->Set((SAMPLE *)d->dat, true);
		}
	}

	unload_datafile(dataSamples);
	return Error(Error::NONE);
}


////////////////////////////////////////////////////////////////////////////////
// Supposed to unload the data
void MAS::Skin::Unload() {
	int i;

	for (i=0; i<nBitmapsEx; i++) {
		bmpList[i]->Destroy();
	}

	for (i=0; i<nCursorsEx; i++) {
		curList[i]->Destroy();
	}

	for (i=0; i<nSamplesEx; i++) {
		smpList[i]->Destroy();
	}

	for (i=0; i<nFontsEx; i++) {
		fntList[i]->Destroy();
	}
}


////////////////////////////////////////////////////////////////////////////////
// Draws a default skin bitmap
void MAS::Skin::GenerateDefaultBitmap(int i) {
	Bitmap *bmp = bmpList[i];
	static int w[] =  { 64,64,18,26,72,18,18,11,15,15,15,15,36,64,18, 48,64,64,64,64,12,36,26,16,18,18,16,16,16,16,16,16,16,12, 2,18,64,88,88,30,30,30, 8, 8,12,30 };
	static int h[] =  { 64,64,72,52,72,80,88,88,56,56,56,56,96,64,24,112,64,64,64,64,36,12,52,64,64,64,64,64,64,64,64,48,64, 2,12,72,64,18,11, 8,30,36,72,72,17, 8 };
	static int tw[] = { 60,58,14,13,68,18,14,11,15,15,15,15,32,60,18, 16,60,60,60,60, 8,14,13,16,14,14,12,16,16,16,12,12,12, 8, 2,14,60,18,22,-1,-1,-1,-1,-1,-1,-1 };
	static int th[] = { 48,58,14,13,68,20,18,22,14,14,14,14, 8,60,12, 16,60,60,60,60,14, 8,13,16,12,12,12,16,16,16,12, 8,12, 2, 8,14,60,14,11,-1,-1,-1,-1,-1,-1,-1 };

	bmp->Create(w[i], h[i], MAS::Settings::useVideoMemory ? Bitmap::VIDEO : Bitmap::MEMORY);
	bmp->ThickX(tw[i]);
	bmp->ThickY(th[i]);
	bmp->Clear(c_face);

	switch (i) {
		case BOX:
			bmp->Rectangle(1, 4, 63, 63, c_shad1);
			bmp->Rectangle(0, 3, 62, 62, c_shad2);
			break;

		case BOX_SHADOW:
			bmp->Draw3DFrame(0, 0, 62, 62, c_face, c_shad1, c_shad2);
			bmp->Hline(1, 63, 63, Color::black);
			bmp->Vline(63, 1, 63, Color::black);
			break;

		case BUTTON:
			bmp->Draw3DFrame(0,  0, 17, 17, c_deselect, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 18, 17, 35, c_select, c_shad2, c_shad1);
			bmp->Draw3DFrame(0, 36, 17, 53, c_face, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 54, 17, 71, c_focus, c_shad1, c_shad2);
			break;

		case CHECKBOX:
			bmp->Draw3DFrame(0,  0, 12, 12,  c_sunken, c_shad2, c_shad1);
			bmp->Draw3DFrame(13,  0, 25, 12, c_sunken, c_shad2, c_shad1);
			bmp->DrawXMark(19, 6, 6, c_font);
			bmp->Draw3DFrame(0,  13, 12, 25, c_select, c_shad2, c_shad1);
			bmp->Draw3DFrame(13, 13, 25, 25, c_select, c_shad2, c_shad1);
			bmp->DrawXMark(19, 19, 6, c_font);
			bmp->Draw3DFrame(0, 26, 12, 38,  c_face, c_shad2, c_shad1);
			bmp->Draw3DFrame(13, 26, 25, 38, c_face, c_shad2, c_shad1);
			bmp->DrawXMark(19, 32, 6, c_disable);
			bmp->Draw3DFrame(0, 39, 12, 51,  c_focus, c_shad2, c_shad1);
			bmp->Draw3DFrame(13,39, 25, 51,  c_focus, c_shad2, c_shad1);
			bmp->DrawXMark(19, 45, 6, c_font);
			break;

		case CLEAR_BACK:
			bmp->Clear(c_back);
			break;

		case COMBO_BUTTON:
			bmp->Clear(Color::magenta);
			bmp->DrawArrow(4,  9, c_font, 3);
			bmp->DrawArrow(4, 29, c_font, 3);
			bmp->DrawArrow(4, 49, c_disable, 3);
			bmp->DrawArrow(4, 69, c_font, 3);
			break;

		case HSLIDER_BACK:
			bmp->Draw3DFrame(0,  9, 17, 12, -1, c_shad2, c_shad1);
			bmp->Draw3DFrame(0, 31, 17, 34, -1, c_shad2, c_shad1);
			bmp->Draw3DFrame(0, 53, 17, 56, -1, c_shad2, c_shad1);
			bmp->Draw3DFrame(0, 75, 17, 78, -1, c_shad2, c_shad1);
			break;

		case HSLIDER_GRIP:
			bmp->Draw3DFrame(0,  0, 10, 21, c_deselect, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 22, 10, 43, c_select, c_shad2, c_shad1);
			bmp->Draw3DFrame(0, 44, 10, 65, c_face, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 66, 10, 87, c_focus, c_shad1, c_shad2);
			break;

		case ICONEXIT:
			bmp->Draw3DFrame(0,  0, 14, 13, c_face, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 14, 14, 27, c_face, c_shad2, c_shad1);
			bmp->Draw3DFrame(0, 28, 14, 41, c_face, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 42, 14, 55, c_face, c_shad1, c_shad2);
			bmp->DrawXMark(7,  7, 6, c_font);
			bmp->DrawXMark(7, 21, 6, c_font);
			bmp->DrawXMark(7, 35, 6, c_disable);
			bmp->DrawXMark(7, 49, 6, c_font);
			break;

		case ICONMAX:
			bmp->Draw3DFrame(0,  0, 14, 13, c_face, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 14, 14, 27, c_face, c_shad2, c_shad1);
			bmp->Draw3DFrame(0, 28, 14, 41, c_face, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 42, 14, 55, c_face, c_shad1, c_shad2);
			bmp->Rectangle(3,  3, 10, 10, c_font);
			bmp->Rectangle(3, 17, 10, 24, c_font);
			bmp->Rectangle(3, 31, 10, 38, c_disable);
			bmp->Rectangle(3, 45, 10, 52, c_font);
			bmp->Hline(3,  4, 10, c_font);
			bmp->Hline(3, 18, 10, c_font);
			bmp->Hline(3, 32, 10, c_disable);
			bmp->Hline(3, 46, 10, c_font);
			break;

		case ICONMIN:
			bmp->Draw3DFrame(0,  0, 14, 13, c_face, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 14, 14, 27, c_face, c_shad2, c_shad1);
			bmp->Draw3DFrame(0, 28, 14, 41, c_face, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 42, 14, 55, c_face, c_shad1, c_shad2);
			bmp->Rectangle(3,  9, 10, 10, c_font);
			bmp->Rectangle(3, 23, 10, 24, c_font);
			bmp->Rectangle(3, 37, 10, 38, c_disable);
			bmp->Rectangle(3, 51, 10, 52, c_font);
			break;

		case ICONRESTORE:
			bmp->Draw3DFrame(0,  0, 14, 13, c_face, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 14, 14, 27, c_face, c_shad2, c_shad1);
			bmp->Draw3DFrame(0, 28, 14, 41, c_face, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 42, 14, 55, c_face, c_shad1, c_shad2);
			bmp->Rectangle(3,  3, 10, 10, c_font);
			bmp->Rectangle(3, 17, 10, 24, c_font);
			bmp->Rectangle(3, 31, 10, 38, c_disable);
			bmp->Rectangle(3, 45, 10, 52, c_font);
			bmp->Hline(3,  4, 10, c_font);
			bmp->Hline(3, 18, 10, c_font);
			bmp->Hline(3, 32, 10, c_disable);
			bmp->Hline(3, 46, 10, c_font);
			break;

		case LIST:
			bmp->Rectfill(0,  0, 35, 11, c_sunken);
			bmp->Rectfill(0, 12, 35, 23, c_face);
			bmp->Rectfill(0, 24, 35, 35, c_sunken);
			bmp->Rectfill(0, 36, 35, 47, c_face);
			bmp->Rectfill(0, 48, 35, 59, c_select);
			bmp->Rectfill(0, 60, 35, 71, c_select);
			bmp->Rectfill(0, 72, 35, 83, c_face);
			bmp->Rectfill(0, 84, 35, 95, c_face);
			break;

		case MENU_BACK:
			bmp->Draw3DFrame(0, 0, 63, 63, c_face, c_shad1, c_shad2);
			break;

		case MENU_BUTTON:
			bmp->Clear(c_deselect);
			bmp->Rectfill(0, 12, 17, 23, c_select);
			break;

		case MENU_ITEM:
			bmp->Clear(c_face);

			bmp->Rectfill( 2, 2, 45, 13, c_face);

			bmp->Rectfill( 2, 18, 45, 29,	c_select);

			bmp->Hline( 4, 38, 43, c_shad2);
			bmp->Hline( 4, 39, 43, c_shad1);

			bmp->Rectfill( 2, 50, 45, 61, c_face);
			bmp->DrawArrow(40, 53, c_font, 1);

			bmp->Rectfill( 2, 66, 45, 77,	c_select);
			bmp->DrawArrow(40, 69, c_sunken, 1);

			bmp->Rectfill( 2, 82, 45, 93, c_face);
			bmp->DrawCheckMark(5, 85, c_font);

			bmp->Rectfill( 2, 98, 45, 109,	c_select);
			bmp->DrawCheckMark(5, 101, c_sunken);
			break;

		case PANEL_GROOVE:
			bmp->Rectangle(1, 1, 63, 63, c_shad2);
			bmp->Rectangle(0, 0, 62, 62, c_shad1);
			break;

		case PANEL_RAISED:
			bmp->Draw3DFrame(0, 0, 63, 63, c_face, c_shad1, c_shad2);
			break;

		case PANEL_RIDGE:
			bmp->Rectangle(1, 1, 63, 63, c_shad1);
			bmp->Rectangle(0, 0, 62, 62, c_shad2);
			break;

		case PANEL_SUNKEN:
			bmp->Draw3DFrame(0, 0, 63, 63, c_sunken, c_shad2, c_shad1);
			break;

		case PROGRESSH:
			bmp->Clear(Color::magenta);
			bmp->Draw3DFrame(0, 0, 11, 17, c_face, c_shad2, c_shad1);
			bmp->Rectfill(2, 20, 10, 33, c_select);
			break;

		case PROGRESSV:
			bmp->Clear(Color::magenta);
			bmp->Draw3DFrame(0, 0, 17, 11, c_face, c_shad2, c_shad1);
			bmp->Rectfill(20, 2, 34, 9, c_select);
			break;

		case RADIO:
			bmp->Draw3DCircle(6, 6, 6, c_sunken, c_shad2, c_shad1);
			bmp->Draw3DCircle(19, 6, 6, c_sunken, c_shad2, c_shad1);
			bmp->Circlefill(19, 6, 2, c_font);
			bmp->Draw3DCircle(6, 19, 6, c_select, c_shad2, c_shad1);
			bmp->Draw3DCircle(19, 19, 6, c_select, c_shad2, c_shad1);
			bmp->Circlefill(19, 19, 2, c_font);
			bmp->Draw3DCircle(6, 32, 6, c_face, c_shad2, c_shad1);
			bmp->Draw3DCircle(19, 32, 6, c_face, c_shad2, c_shad1);
			bmp->Circlefill(19, 32, 2, c_disable);
			bmp->Draw3DCircle(6, 45, 6, c_focus, c_shad2, c_shad1);
			bmp->Draw3DCircle(19, 45, 6, c_focus, c_shad2, c_shad1);
			bmp->Circlefill(19, 45, 2, c_font);
			break;

		case SCROLL_DOWN:
			bmp->Clear(Color::magenta);
			bmp->Draw3DFrame(0, 0, 15, 15, c_deselect, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 16, 15, 31, c_select, c_shad2, c_shad1);
			bmp->Draw3DFrame(0, 32, 15, 47, c_face, c_shad2, c_shad2);
			bmp->Draw3DFrame(0, 48, 15, 63, c_focus, c_shad1, c_shad2);
			bmp->DrawArrow(4, 6, c_font, 3);
			bmp->DrawArrow(5, 23, c_font, 3);
			bmp->DrawArrow(4, 38, c_disable, 3);
			bmp->DrawArrow(4, 54, c_font, 3);
			break;

		case SCROLL_HBACK:
			break;

		case SCROLL_HGRIP:
			bmp->Draw3DFrame(0,	0, 17, 15, c_deselect, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 16, 17, 31,	c_select, c_shad2, c_shad1);
			bmp->Draw3DFrame(0, 32, 17, 47,	 c_face, c_shad2, c_shad2);
			bmp->Draw3DFrame(0, 48, 17, 63,	c_select, c_shad1, c_shad2);
			break;

		case SCROLL_HGRIPOVERLAY:
			bmp->Clear(Color::magenta);
			break;

		case SCROLL_LEFT:
			bmp->Clear(Color::magenta);
			bmp->Draw3DFrame(0, 0, 15, 15, c_deselect, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 16, 15, 31, c_select, c_shad2, c_shad1);
			bmp->Draw3DFrame(0, 32, 15, 47, c_face, c_shad2, c_shad2);
			bmp->Draw3DFrame(0, 48, 15, 63, c_focus, c_shad1, c_shad2);
			bmp->DrawArrow(5, 4, c_font, 0);
			bmp->DrawArrow(6, 21, c_font, 0);
			bmp->DrawArrow(5, 36, c_disable, 0);
			bmp->DrawArrow(5, 52, c_font, 0);
			break;

		case SCROLL_RIGHT:
			bmp->Clear(Color::magenta);
			bmp->Draw3DFrame(0, 0, 15, 15, c_deselect, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 16, 15, 31, c_select, c_shad2, c_shad1);
			bmp->Draw3DFrame(0, 32, 15, 47, c_face, c_shad2, c_shad2);
			bmp->Draw3DFrame(0, 48, 15, 63, c_focus, c_shad1, c_shad2);
			bmp->DrawArrow(6, 4, c_font, 1);
			bmp->DrawArrow(7, 21, c_font, 1);
			bmp->DrawArrow(6, 36, c_disable, 1);
			bmp->DrawArrow(6, 52, c_font, 1);
			break;

		case SCROLL_UP:
			bmp->Clear(Color::magenta);
			bmp->Draw3DFrame(0, 0, 15, 15, c_deselect, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 16, 15, 31, c_select, c_shad2, c_shad1);
			bmp->Draw3DFrame(0, 32, 15, 47, c_face, c_shad2, c_shad2);
			bmp->Draw3DFrame(0, 48, 15, 63, c_focus, c_shad1, c_shad2);
			bmp->DrawArrow(4, 6, c_font, 2);
			bmp->DrawArrow(5, 23, c_font, 2);
			bmp->DrawArrow(4, 38, c_disable, 2);
			bmp->DrawArrow(4, 54, c_font, 2);
			break;

		case SCROLL_VBACK:
			break;

		case SCROLL_VGRIP:
			bmp->Draw3DFrame(0,	0, 15, 11, c_deselect, c_shad1, c_shad2);
			bmp->Draw3DFrame(0, 12, 15, 23,	c_select, c_shad2, c_shad1);
			bmp->Draw3DFrame(0, 24, 15, 35,	 c_face, c_shad2, c_shad2);
			bmp->Draw3DFrame(0, 36, 15, 47,	c_select, c_shad1, c_shad2);
			break;

		case SCROLL_VGRIPOVERLAY:
			bmp->Clear(Color::magenta);
			break;

		case SEPARATORH:
			bmp->Hline(0, 0, 11, c_shad2);
			bmp->Hline(0, 1, 11, c_shad1);
			break;

		case SEPARATORV:
			bmp->Vline(0, 0, 11, c_shad2);
			bmp->Vline(1, 0, 11, c_shad1);
			break;

		case TAB_BUTTON:
			bmp->Clear(Color::magenta);
			bmp->Draw3DFrame(0,  0, 17, 19, c_face, c_shad1, c_shad2);
			bmp->Hline(      1, 17, 16,     c_shad1);
			bmp->Putpixel(   0, 17,         Color::magenta);
			bmp->Putpixel(  17, 17,         Color::magenta);
			bmp->Draw3DFrame(0, 18, 17, 38, c_face, c_shad1, c_shad2);
			bmp->Putpixel(   0, 35,         Color::magenta);
			bmp->Putpixel(  17, 35,         Color::magenta);
			bmp->Draw3DFrame(0, 36, 17, 55, c_face, c_shad1, c_shad2);
			bmp->Hline(      1, 53, 16,     c_shad1);
			bmp->Putpixel(   0, 53,         Color::magenta);
			bmp->Putpixel(  17, 53,         Color::magenta);
			bmp->Draw3DFrame(0, 54, 17, 74, c_face, c_shad1, c_shad2);
			bmp->Hline(      1, 71, 16,     c_shad1);
			bmp->Putpixel(   0, 71,         Color::magenta);
			bmp->Putpixel(  17, 71,         Color::magenta);
			break;

		case TAB_WINDOW:
			bmp->Draw3DFrame(0, 0, 63, 63, c_face, c_shad1, c_shad2);
			break;

		case VSLIDER_BACK:
			bmp->Draw3DFrame( 9, 0, 12, 17, -1, c_shad2, c_shad1);
			bmp->Draw3DFrame(31, 0, 34, 17, -1, c_shad2, c_shad1);
			bmp->Draw3DFrame(53, 0, 56, 17, -1, c_shad2, c_shad1);
			bmp->Draw3DFrame(75, 0, 78, 17, -1, c_shad2, c_shad1);
			break;

		case VSLIDER_GRIP:
			bmp->Draw3DFrame( 0, 0, 21, 10, c_deselect, c_shad1, c_shad2);
			bmp->Draw3DFrame(22, 0, 43, 10, c_select, c_shad2, c_shad1);
			bmp->Draw3DFrame(44, 0, 65, 10, c_face, c_shad1, c_shad2);
			bmp->Draw3DFrame(66, 0, 87, 10, c_focus, c_shad1, c_shad2);
			break;

		case WINBOTTOM:
			bmp->Draw3DFrame(-2, 0, 31, 7, c_face, c_shad1, c_shad2);
			bmp->Draw3DFrame(-2, -2, 31, 3, c_face, c_shad1, c_shad2);
			break;

		case WINDOW:
			break;

		case WINGRIP:
			bmp->Rectfill( 0, 0, 29, 17, c_select);
			bmp->Rectfill( 0, 18, 29, 35, c_shad2);
			break;

		case WINLEFT:
			bmp->Draw3DFrame(0, 0, 7, 71, c_face, c_shad1, c_shad2);
			bmp->Draw3DFrame(4, 0, 10, 71, c_face, c_shad1, c_shad2);
			break;

		case WINRIGHT:
			bmp->Draw3DFrame(0, 0, 7, 71, c_face, c_shad1, c_shad2);
			bmp->Draw3DFrame(-2, 0, 3, 71, c_face, c_shad1, c_shad2);
			break;

		case WINTEXT:
			bmp->Clear(Color::magenta);
			break;

		case WINTOP:
			bmp->Draw3DFrame(-2, 0, 31, 7, c_face, c_shad1, c_shad2);
			bmp->Draw3DFrame(-2, 4, 31, 10, c_face, c_shad1, c_shad2);
			break;
	};
}


////////////////////////////////////////////////////////////////////////////////
// Draws all the default skin bitmaps
void MAS::Skin::GenerateDefaultBitmaps() {
	int i;
	for (i=0; i<nBitmaps; ++i) {
		GenerateDefaultBitmap(i);
	}
}


////////////////////////////////////////////////////////////////////////////////
// Sets all the cursors to the default Allegro arrow
void MAS::Skin::GenerateDefaultCursors() {
	int i;
	for (i=1; i<nCursors; i++) {
		curList[i]->Create();
	}
}


////////////////////////////////////////////////////////////////////////////////
// Sets all the fonts to the default Allegro font
void MAS::Skin::GenerateDefaultFonts() {
	for (int i=0; i<nFonts; i++) {
		fntList[i]->Create();
	}
}


////////////////////////////////////////////////////////////////////////////////
// Sets all the samples to an empty sample
void MAS::Skin::GenerateDefaultSamples() {
	for (int i=0; i<nSamples; i++) {
		smpList[i]->Create(8, false, 11025, 0);
	}
}


////////////////////////////////////////////////////////////////////////////////
// Resets the entire skin
void MAS::Skin::Reset() {
	Unload();
	ResetColors();
	ResetControls();
	GenerateDefaultFonts();
	GenerateDefaultCursors();
	GenerateDefaultSamples();
	GenerateDefaultBitmaps();
}


////////////////////////////////////////////////////////////////////////////////
// Resets all the object specific info
void MAS::Skin::ResetControls() {
	// size of the central area of the bitmaps
	for (int i=0; i<nBitmaps; i++) {
		bmpList[i]->ThickX(-1);
		bmpList[i]->ThickY(-1);
	}

	focus = 0;
	drawDots = 1;
	wallpaperStyle = 3;
	buttonDisplacement = 1;
	buttonAnimationType = 0;
	buttonAnimationLength = 0;
	boxX = 10;
	boxY = 1;
	boxBack = c_face;
	boxAlign = 0;
	winTextPos.x(10);
	winTextPos.y(8);
	winTextAlign = 0;
	winTextBack = 0;
	winExitPos.x(-22);
	winExitPos.y(6);
	winMaxPos.x(-40);
	winMaxPos.y(6);
	winMinPos.x(-56);
	winMinPos.y(6);
	comboAnimationType = 0;
	comboAnimationLength = 0;
	menuHeight = 16;
	menuDelay = 300;
	menuAnimationType = 0;
	menuAnimationLength = 0;
	winAnimationType = 0;
	winAnimationLength = 0;
	tooltipBack = Color(255,255,192);
	tooltipBorder = Color::black;
	tooltipAnimationType = 0;
	tooltipAnimationLength = 0;
}


////////////////////////////////////////////////////////////////////////////////
// For loading a bitmap from disk into the bitmap array
MAS::Error MAS::Skin::LoadBitmap(const char *filename, int i) {
	if (i<0 || i>=nBitmapsEx) {
		return Error(Error::INDEX);
	}

	set_color_conversion(COLORCONV_TOTAL);
	return bmpList[i]->Load(filename, MAS::Settings::useVideoMemory ? Bitmap::VIDEO : Bitmap::MEMORY);
}


////////////////////////////////////////////////////////////////////////////////
// For loading a cursor from disk into the cursor array
MAS::Error MAS::Skin::LoadCursor(const char *filename, int i, int n, int interval) {
	if (i<0 || i>=nCursorsEx) {
		return Error(Error::INDEX);
	}

	if (!file_exists(filename, FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH, NULL)) {
		return Error(Error::NO_FILE);
	}

	set_color_conversion(COLORCONV_TOTAL);
	Bitmap bmp;
	bmp.Load(filename);
	if (!bmp) {
		return Error(Error::LOAD_FILE);
	}

	curList[i]->Create(bmp, n);
	curList[i]->SetAnimationInterval(interval);

	return Error(Error::NONE);
}


////////////////////////////////////////////////////////////////////////////////
// Loads a bitmap or truetype font into the font array
MAS::Error MAS::Skin::LoadFont(const char *filename, int i, int size) {
	if (i<0 || i>=nFontsEx) {
		return Error(Error::INDEX);
	}

	Error e = fntList[i]->Load(filename);
	if (!e) {
		fntList[i]->SetSize(size);
	}

	return e;
}


////////////////////////////////////////////////////////////////////////////////
// For loading a sample from disk into the samples array
MAS::Error MAS::Skin::LoadSample(const char *filename, int i) {
	if (i<0 || i>=nFontsEx) {
		return Error(Error::INDEX);
	}

	return smpList[i]->Load(filename);
}


////////////////////////////////////////////////////////////////////////////////
// A helper function for setting a pointer to a bitmap in the bitmap array or
// for making a copy of it
void MAS::Skin::SetBitmap(Bitmap& b, int i, bool copy) {
	if (i>=0 && i<nBitmapsEx) {
		bmpList[i]->Set(b, copy);
	}
}


////////////////////////////////////////////////////////////////////////////////
// A helper function for setting a pointer to a cursor in the cursor array or
// making a copy of it
void MAS::Skin::SetCursor(Cursor& c, int i) {
	if (i>=0 && i<nCursorsEx) {
		delete curList[i];
		curList[i] = &c;
	}
}


////////////////////////////////////////////////////////////////////////////////
// A helper function for setting a new font
void MAS::Skin::SetFont(Font& f, int i) {
	if (i>=0 && i<nFontsEx) {
		delete fntList[i];
		fntList[i] = &f;
	}
}


////////////////////////////////////////////////////////////////////////////////
// A helper function for setting a pointer to a sample in the samples array or
// making a copy of it
void MAS::Skin::SetSample(Sample& s, int i, bool copy) {
	if (i>=0 && i<nSamplesEx) {
		smpList[i]->Set(s, copy);
	}
}



////////////////////////////////////////////////////////////////////////////////
// Colors the skin bitmaps with the specified color and alpha intensity
void MAS::Skin::Colorize(const Color &col, int alpha) {
	Bitmap *bmp;
	int i, x, y;
	Color pink = Color::magenta;

	Reload();
	for (i=0; i<nBitmaps; i++) {
		bmp = bmpList[i];
		for (y=0; y<bmp->h(); y++) {
			for (x=0; x<bmp->w(); x++) {
				Color p = bmp->Getpixel(x, y);
				if (p == pink)
					continue;

				p.Colorize(col, alpha);
				bmp->Putpixel(x, y, p);
			}
		}
	}

	//if (box_back != -1) box_back = MASColorizePixel(box_back, col, alpha);
	c_back.Colorize(col, alpha);
	c_deselect.Colorize(col, alpha);
	c_disable.Colorize(col, alpha);
	c_face.Colorize(col, alpha);
	c_focus.Colorize(col, alpha);
	c_font.Colorize(col, alpha);
	c_select.Colorize(col, alpha);
	c_shad1.Colorize(col, alpha);
	c_shad2.Colorize(col, alpha);
	c_sunken.Colorize(col, alpha);
}


void MAS::Skin::PlaySample(int i) {
	Sample s = GetSample(i);
	s.Stop();
	s.Play(MAS::Settings::soundVolume);
}


MAS::Error MAS::Skin::GetError() { return lastError; }
MAS::Bitmap& MAS::Skin::GetBitmap(int i) const { return (i>=0 && i<nBitmapsEx) ? *bmpList[i] : Bitmap::null; }
MAS::Cursor& MAS::Skin::GetCursor(int i) const { return *curList[i]; }
MAS::Font& MAS::Skin::GetFont(int i) const { return *fntList[i]; }
MAS::Sample& MAS::Skin::GetSample(int i) const { return *smpList[i]; }



void MAS::Skin::Dump() const {
	const int LOGW = 320;
	const int LOGH = 480;
	Bitmap log(LOGW,LOGH);
	log.Clear(Color::yellow);

	int x=0, y=0, i, dh=0;
	Bitmap tmp;
	for (i=0; i<nBitmaps; i++) {
		tmp = GetBitmap(i);
		if (x + tmp.w() > LOGW) {
			x = 0;
			y += dh+1;
			dh = 0;
		}
		dh = (tmp.h() > dh) ? tmp.h() : dh;
		tmp.Blit(log, 0, 0, x, y, tmp.w(), tmp.h());
		//log.DrawSprite(tmp, x, y);
		x += tmp.w()+1;
	}

	log.Save("log.bmp");
}

MAS::Skin *MAS::theSkin;
