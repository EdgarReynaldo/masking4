#include <time.h>
#include "../include/MASkinG.h"
using namespace MAS;


//------------------------------------------------------------------------------
// The MineField widget class (declaration):


#define	MAX_WIDTH	50
#define	MAX_HEIGHT	50
#define TILE_SIZE	16

#define	MSG_GAMEOVER		1000
#define	MSG_STARTPLAYING	1001
#define MSG_MARKMINE		1002

class MineField : public Widget {
	private:
		int width;
		int height;
		int mines;

		struct Tile {
			bool haveMine;
			bool uncovered;
			bool marked;
			int neighbours;
		};

		Tile field[MAX_WIDTH][MAX_HEIGHT];

		int focusX, focusY;
		bool gameOver;
		bool playing;
		int nMarkedMines;

		void Uncover(int x, int y);
		void CheckForEndGame();

	protected:
		void Draw(Bitmap &canvas);
		void MsgMousemove(const Point &d);
		void MsgLPress();
		void MsgLRelease();
		void MsgRPress();

	public:
		MineField();
		~MineField();

		void NewGame(int width, int height, int mines);
};


//------------------------------------------------------------------------------
// The MineField widget class (implementation):


MineField::MineField() : Widget(), width(0), height(0), mines(0) {
	focusX = -1;
	focusY = -1;

	NewGame(9, 9, 10);
}


MineField::~MineField() {
}


void MineField::NewGame(int width, int height, int mines) {
	int i,j;

	this->width = width;
	this->height = height;
	this->mines = mines;

	// generate mine positions
	int nPos = width*height;
	int *mine_pos = new int[nPos];
	for (i=0; i<nPos; i++) mine_pos[i] = i;

	// shuffle mine positions
	for (i=0; i<10000; i++) {
		int p1 = rand()%nPos;
		int p2 = rand()%nPos;
		int tmp = mine_pos[p1];
		mine_pos[p1] = mine_pos[p2];
		mine_pos[p2] = tmp;
	}

	// reset the field
	for (j=0; j<MAX_HEIGHT; j++) {
		for (i=0; i<MAX_WIDTH; i++) {
			field[i][j].uncovered = false;
			field[i][j].marked = false;
			field[i][j].haveMine = false;
			field[i][j].neighbours = 0;
		}
	}

	// place the mines
	for (i=0; i<mines; i++) {
		int x = mine_pos[i]%width;
		int y = mine_pos[i]/width;
		field[x][y].haveMine = true;
	}

	// count mine neighbours
	for (j=0; j<height; j++) {
		for (i=0; i<width; i++) {
			if (!field[i][j].haveMine) {
				int mCount = 0;
				for (int dj=-1; dj<=1; dj++) {
					for (int di=-1; di<=1; di++) {
						int ii = i + di;
						int jj = j + dj;
						if (ii >= 0 && ii < width && jj >= 0 && jj < height) {
							if (field[ii][jj].haveMine) {
								++mCount;
							}
						}
					}
				}
				field[i][j].neighbours = mCount;
			}
		}
	}

	delete [] mine_pos;

	gameOver = false;
	playing = false;
	nMarkedMines = 0;

	Redraw();
}


void MineField::Draw(Bitmap &canvas) {
	int c1 = skin->c_face;
	int c2 = skin->c_shad1;
	int c3 = skin->c_shad2;

	static Color cNum[] = {
		Color::blue,
		Color::darkgreen,
		Color::red,
		Color::darkblue,
		Color::brown,
		Color(0, 128, 128),
		Color::black,
		Color::darkgray
	};

	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			int x1 = i*TILE_SIZE;
			int y1 = j*TILE_SIZE;
			int x2 = x1 + TILE_SIZE - 1;
			int y2 = y1 + TILE_SIZE - 1;

			if ((i == focusX && j == focusY) || (field[i][j].uncovered)) {
				if (gameOver && field[i][j].haveMine) {
					canvas.Rectfill(x1, y1, x2, y2, Color::red);
				}
				else {
					canvas.Rectfill(x1, y1, x2, y2, c1);
				}
				canvas.Hline(x1, y1, x2, c3);
				canvas.Vline(x1, y1, y2, c3);

				if (field[i][j].uncovered) {
					if (field[i][j].haveMine) {
						canvas.Circlefill(x1+8, y1+8, 4, Color::black);
						canvas.Hline(x1+2, y1+8, x1+14, Color::black);
						canvas.Vline(x1+8, y1+2, y1+14, Color::black);
						canvas.Line(x1+4, y1+4, x1+12, y1+12, Color::black);
						canvas.Line(x1+12, y1+4, x1+4, y1+12, Color::black);
						canvas.Rectfill(x1+6, y1+6, x1+7, y1+7, Color::white);
					}
					else if (field[i][j].neighbours > 0) {
#if (ALLEGRO_VERSION >= 4 && ALLEGRO_SUB_VERSION >= 1)
						textprintf_ex(canvas, font, x1 + 5, y1 + 5, cNum[field[i][j].neighbours-1], -1, "%d", field[i][j].neighbours);
#else
						text_mode(-1);
						textprintf(canvas, font, x1 + 5, y1 + 5, cNum[field[i][j].neighbours-1], "%d", field[i][j].neighbours);
#endif
					}
				}
			}
			else {
				canvas.Draw3DFrame(x1, y1, x2, y2, c1, c2, c3);

				if (field[i][j].marked) {
					canvas.Triangle(Point(x1+3, y1+13), Point(x1+12, y1+13), Point(x1+8, y1+9), Color::black);
					canvas.Putpixel(Point(x1+8, y1+8), Color::black);
					canvas.Triangle(Point(x1+8, y1+8), Point(x1+4, y1+5), Point(x1+8, y1+3), Color::red);

					if (gameOver && !field[i][j].haveMine) {
						canvas.DrawXMark(Point((x1+x2)/2, (y1+y2)/2), 10, Color::red);
					}
				}
			}

		}
	}
}


void MineField::MsgMousemove(const Point &d) {
	Widget::MsgMousemove(d);
	if (gameOver) return;

	if (mouse_b&1) {
		Point mp = GetMousePos();
		focusX = mp.x()/TILE_SIZE;
		focusY = mp.y()/TILE_SIZE;
		Redraw();
	}
}


void MineField::MsgLPress() {
	Widget::MsgLPress();
	if (gameOver) return;

	Point mp = GetMousePos();
	focusX = mp.x()/TILE_SIZE;
	focusY = mp.y()/TILE_SIZE;
	Redraw();
}


void MineField::MsgLRelease() {
	Widget::MsgLRelease();
	if (gameOver) return;

	if (focusX >= 0 && focusX < width && focusY >= 0 && focusY < height) {
		if (field[focusX][focusY].haveMine) {
			field[focusX][focusY].uncovered = true;
			gameOver = true;
			GetParent()->HandleEvent(*this, MSG_GAMEOVER, 0);
		}
		else {
			if (field[focusX][focusY].neighbours > 0) {
				field[focusX][focusY].uncovered = true;
			}
			else {
				Uncover(focusX, focusY);
			}

			if (!playing) {
				playing = true;
				GetParent()->HandleEvent(*this, MSG_STARTPLAYING);
			}

			CheckForEndGame();
		}
	}

	focusX = focusY = -1;
	Redraw();
}


void MineField::MsgRPress() {
	Widget::MsgRPress();
	if (gameOver) return;

	Point mp = GetMousePos();
	int focusX = mp.x()/TILE_SIZE;
	int focusY = mp.y()/TILE_SIZE;

	if (!field[focusX][focusY].uncovered) {
		if (field[focusX][focusY].marked) {
			field[focusX][focusY].marked = false;
			--nMarkedMines;
		}
		else {
			field[focusX][focusY].marked = true;
			++nMarkedMines;
		}
		GetParent()->HandleEvent(*this, MSG_MARKMINE, mines - nMarkedMines);
		Redraw();
	}
}


void MineField::Uncover(int x, int y) {
	if (x < 0 || x >= width || y < 0 || y >= height) return;

	if (!field[x][y].haveMine && !field[x][y].uncovered && !field[x][y].marked) {
		field[x][y].uncovered = true;
		if (field[x][y].neighbours == 0) {
			Uncover(x-1, y);
			Uncover(x+1, y);
			Uncover(x, y-1);
			Uncover(x, y+1);
			Uncover(x+1, y+1);
			Uncover(x-1, y+1);
			Uncover(x+1, y-1);
			Uncover(x-1, y-1);
		}
	}
}


void MineField::CheckForEndGame() {
	bool endGame = true;

	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			if (!field[i][j].uncovered && !field[i][j].marked) {
				endGame = false;
				break;
			}
		}
	}

	if (endGame) {
		bool wonGame = true;

		for (int j=0; j<height; j++) {
			for (int i=0; i<width; i++) {
				if (field[i][j].haveMine && !field[i][j].marked) {
					field[i][j].uncovered = true;
				}

				if (field[i][j].marked && !field[i][j].haveMine) {
					wonGame = false;
				}
			}
		}

		playing = false;
		gameOver = true;

		if (wonGame) {
			GetParent()->HandleEvent(*this, MSG_GAMEOVER, 1);
		}
		else {
			GetParent()->HandleEvent(*this, MSG_GAMEOVER, 0);
		}
	}

	Redraw();
}


//------------------------------------------------------------------------------
// The MainDialog class (declaration):


class MainDialog : public Dialog {
	private:
		ClearScreen desktop;
		Accelerator acc;
		EditBox eMinesLeft, eSeconds;
		Button bNewGame;
		PanelRaised panel1;
		PanelSunken panel2;
		MineField minefield;

		Menu topMenu, gameMenu, helpMenu;
		enum {
			GAME_NEW = MSG_SUSER,
			GAME_BEGINNER,
			GAME_INTERMEDIATE,
			GAME_ADVANCED,
			GAME_CUSTOM,
			GAME_HIGHSCORES,
			GAME_EXIT,
			HELP_INDX,
			HELP_ABOUT
		};

		int type;
		int width;
		int height;
		int mines;

		int timerID;

		void UpdateAfterChangingGameSettings();

	protected:
		void MsgStart();
		void MsgTimer(int ID);

		void OnNew();
		void OnBeginner();
		void OnIntermediate();
		void OnAdvanced();
		void OnCustom();
		void OnHighscores();
		void OnHelp();
		void OnAbout();

		void OnGameOver(int won);
		void OnMarkMine(int minesLeft);
		void OnStartPlaying();

	public:
		MainDialog();
		~MainDialog();

		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0);
};


//------------------------------------------------------------------------------
// The MainDialog class (implementation):


MainDialog::MainDialog() : Dialog(), timerID(-1) {
	Add(desktop);

	eMinesLeft.Setup(0, 0, 64, 22, 0, D_DISABLED, "0", 8);
	eSeconds.Setup(0, 0, 64, 22, 0, D_DISABLED, "0", 8);
	bNewGame.Setup(0, 0, 80, 22, 0, 0, "New Game");

	Add(panel1);
	Add(eMinesLeft);
	Add(eSeconds);
	Add(bNewGame);
	Add(panel2);
	Add(minefield);

	gameMenu.Add("&New\tF2", GAME_NEW);				acc.Add(KEY_F2, 0, GAME_NEW);
	gameMenu.Add();
	gameMenu.Add("&Beginner", GAME_BEGINNER);
	gameMenu.Add("&Intermediate", GAME_INTERMEDIATE);
	gameMenu.Add("&Advanced", GAME_ADVANCED);
	//gameMenu.Add("&Custom", GAME_CUSTOM);
	gameMenu.Add();
	gameMenu.Add("&High scores...", GAME_HIGHSCORES);
	gameMenu.Add();
	gameMenu.Add("E&xit\tCtrl-X", GAME_EXIT);		acc.Add(KEY_X, KB_CTRL_FLAG, GAME_EXIT);

	helpMenu.Add("&Index\tF1", HELP_INDX);			acc.Add(KEY_F1, 0, HELP_INDX);
	helpMenu.Add("&About\tCtrl-A", HELP_ABOUT);		acc.Add(KEY_A, KB_CTRL_FLAG, HELP_ABOUT);

	topMenu.Add("&Game", gameMenu);
	topMenu.Add("&Help", helpMenu);
	Add(topMenu);

	Add(acc);

	push_config_state();
	char fullPath[1024];
	MakeFullPath(fullPath, "minesweeper.cfg");
	set_config_file(fullPath);

	type = get_config_int("GAME", "type", 0);
	width = get_config_int("GAME", "width", 9);
	height = get_config_int("GAME", "height", 9);
	mines = get_config_int("GAME", "mines", 10);

	pop_config_state();
}


MainDialog::~MainDialog() {
	push_config_state();
	char fullPath[1024];
	MakeFullPath(fullPath, "minesweeper.cfg");
	set_config_file(fullPath);

	set_config_int("GAME", "type", type);
	set_config_int("GAME", "width", width);
	set_config_int("GAME", "height", height);
	set_config_int("GAME", "mines", mines);

	pop_config_state();
}


void MainDialog::MsgStart() {
	Dialog::MsgStart();
	UpdateAfterChangingGameSettings();
	srand((unsigned)time(NULL));
}


void MainDialog::HandleEvent(Widget &obj, int msg, int arg1, int arg2) {
	Dialog::HandleEvent(obj, msg, arg1, arg2);

	switch (msg) {
		case MSG_ACTIVATE: {
			if (obj == bNewGame) {
				OnNew();
			}
		}
		break;

		case GAME_EXIT:			Close();						break;
		case GAME_NEW:			OnNew();						break;
		case GAME_BEGINNER:		OnBeginner();					break;
		case GAME_INTERMEDIATE:	OnIntermediate();				break;
		case GAME_ADVANCED:		OnAdvanced();					break;
		case GAME_CUSTOM:		OnCustom();						break;
		case GAME_HIGHSCORES:	OnHighscores();					break;

		case HELP_INDX:			OnHelp();						break;
		case HELP_ABOUT:		OnAbout();						break;

		case MSG_GAMEOVER:		OnGameOver(arg1);				break;
		case MSG_MARKMINE:		OnMarkMine(arg1);				break;
		case MSG_STARTPLAYING:	OnStartPlaying();				break;
	};
}


void MainDialog::OnNew() {
	minefield.NewGame(width, height, mines);

	Timer::Kill(timerID);
	timerID = -1;
	eSeconds.SetNumber(0);
}


void MainDialog::OnBeginner() {
	if (type != 0) {
		type = 0;
		width = 9;
		height = 9;
		mines = 10;
		UpdateAfterChangingGameSettings();
	}
}


void MainDialog::OnIntermediate() {
	if (type != 1) {
		type = 1;
		width = 16;
		height = 16;
		mines = 40;
		UpdateAfterChangingGameSettings();
	}
}


void MainDialog::OnAdvanced() {
	if (type != 2) {
		type = 2;
		width = 30;
		height = 16;
		mines = 99;
		UpdateAfterChangingGameSettings();
	}
}


void MainDialog::OnCustom() {
	//TODO: setup custom game
}


void MainDialog::OnHighscores() {
	//TODO: show highscores
}


void MainDialog::OnHelp() {
	//TODO:: show help
}


void MainDialog::OnAbout() {
	//TODO: show about box
}


void MainDialog::UpdateAfterChangingGameSettings() {
	for (int i=0; i<3; i++) {
		gameMenu.Uncheck(i+2);
	}
	gameMenu.Check(type+2);

	int dw = width*TILE_SIZE + 16;
	int dh = height*TILE_SIZE + 68;
	dw = MAX(dw, 240);
	dh = MAX(dh, 270);
	ChangeResolution(Settings::gfxMode, Settings::fullscreen, dw, dh, Settings::colorDepth, this);
	desktop.Resize(Settings::screenWidth, Settings::screenHeight);

	eMinesLeft.Place(8, 30);
	eMinesLeft.SetNumber(mines);

	eSeconds.Place(-(8 + eSeconds.w()), 30);
	eSeconds.SetNumber(0);

	bNewGame.Place((Settings::screenWidth - bNewGame.w())/2, 30);

	int x = (dw - width*TILE_SIZE)/2;
	int y = (dh - height*TILE_SIZE)/2 + 26;
	int w = width*TILE_SIZE;
	int h = height*TILE_SIZE;
	panel2.Shape(x - 3, y - 3, w + 6, h + 6);
	minefield.NewGame(width, height, mines);
	minefield.Shape(x, y, w, h);
	panel1.Shape(0, 24, Settings::screenWidth, Settings::screenHeight - 24);
}


void MainDialog::OnGameOver(int won) {
	Timer::Kill(timerID);
	timerID = -1;

	if (won) {
		MessageBox msg("Game over!", "You won!", NULL, NULL, "OK");
		msg.Popup(this);
	}
}


void MainDialog::OnMarkMine(int minesLeft) {
	eMinesLeft.SetNumber(minesLeft);
}


void MainDialog::OnStartPlaying() {
	timerID = Timer::InstallEx(1);
}


void MainDialog::MsgTimer(int ID) {
	Dialog::MsgTimer(ID);

	if (ID == timerID) {
		eSeconds.SetNumber(eSeconds.GetInt()+1);
	}
}


//------------------------------------------------------------------------------
// The main method


int main() {
	Error err = InstallMASkinG("minesweeper.cfg");
	if (err) {
		err.Report();
	}

	MainDialog *dlg = new MainDialog;
	dlg->Execute();
	delete dlg;

	ExitMASkinG();
	return 0;
}
END_OF_MAIN();
