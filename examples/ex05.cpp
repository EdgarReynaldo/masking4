// example 5: A simple Tetris clone. Shows it's possible to make games with
// MASkinG too.

#include "../include/MASkinG.h"
#include <time.h>
#include <list>
using namespace MAS;
using namespace std;

class PlayArea;
class MyDialog;

class MyColor {
	public:
		Color c1, c2, c3;
		void Make();
};

// I use defines here because Micro$oft Vi$ual $tudio doesn't seem to
// support anything more sensible
#define		NPIECES		19
#define		PIECESIZE	16
#define		PLAYWIDTH	10
#define		PLAYHEIGHT	20

class Piece {
	public:
		bool matrix[4][4];
		int wide, high, type;
		MyColor col;
		int posx, posy;
		PlayArea *map;

	protected:
		bool CanMove();

	public:
		Piece();
		void Create(int type);
		void CreateRandomPiece();
		void RotateRight();
		void RotateLeft();
		void MoveLeft();
		void MoveRight();
		bool MoveDown();
		bool CanMoveLeft();
		bool CanMoveRight();
		bool CanMoveDown();
		void Draw(Bitmap &canvas);
		void Draw(Bitmap &canvas, int x, int y);
};

class NextPiece : public Widget {
	public:
		NextPiece() : Widget(), piece(NULL) {}
		Piece *piece;
		void Draw(Bitmap &canvas);
};

class PlayArea : public Widget {
	protected:
	    MyColor tile[PLAYWIDTH][PLAYHEIGHT];
		Piece *curPiece, *nextPiece;
		int timerID;
		bool playing;
		int lines, score;
		int frameInterval;

		void MsgTick();
		void MsgTimer(int t);
		bool MsgChar(int c);
		bool MsgWantfocus() { return true; }
		void Draw(Bitmap &canvas);

		void DropCurrentPiece();
		void CheckLines();
		bool LineFull(int j);
		void ClearLine(int j);
		void ClearPlayArea();
		void GameOver();

	public:
		PlayArea();
		~PlayArea();
		MyColor GetTile(int x, int y);
		void SetTile(int x, int y, const MyColor &val);
		void Play(int level);
		Piece *GetNextPiece() { return nextPiece; }
};

class HighscoreItem {
	public:
		HighscoreItem(const char *n, int s, int l);
		char name[8];
		int score;
		int lines;
};

class Highscore {
	public:
		list<HighscoreItem *> items;

	protected:
		int nItems;
		char *fileName;

		void MakeDefaultTable();
		void DestroyTable();
		void Load(PACKFILE *f);
		void Save(PACKFILE *f);

	public:
		Highscore(const char *file);
		~Highscore();

		bool CanAdd(int score);
		void Add(const char *name, int score, int lines);
};

class TetrisDialog : public Dialog {
	public:
		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0) {
			parent->HandleEvent(obj, msg, arg1, arg2);
		}
};

class MyDialog : public Dialog {
	private:
		ClearScreen desktop;
		Button bExit, bScores;
		FPS fpsCounter;
		Label driver, title, lFPS, lDriver, lTitle;

		TetrisDialog tetrisDialog;
		Panel pan;
		Button buttonPlay;
		EditBox eLines, eScore;
		SpinBox eLevel;
		Label lLines, lScore, lLevel, lNext;
		Label lLeft, lRight, lDown, lRotate;
		NextPiece nextPiece;
		PlayArea map;

		Highscore *hsc;

	protected:
		void MsgStart();
		void GetHiscoreName();
		void ShowScores();
		bool MsgClose();

	public:
		MyDialog();
		~MyDialog();
		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0);
		enum { MSG_GAMEOVER = MSG_SUSER, MSG_UPDATESCORE, MSG_INCREASELEVEL, MSG_NEWPIECE };
};


Piece::Piece() :wide(4), high(4), posx(rand()%(PLAYWIDTH-4)), posy(PLAYHEIGHT-1), map(NULL) {
	col.c1 = Color::black;
}

void Piece::CreateRandomPiece() {
	int i = rand()%7;
	int type = i;
	switch (i) {
		case 0:		type = i;					break;
		case 1:		type = rand()%2 + 1;		break;
		case 2:		type = rand()%2 + 3;		break;
		case 3:		type = rand()%4 + 5;		break;
		case 4:		type = rand()%4 + 9;		break;
		case 5:		type = rand()%2 + 13;		break;
		case 6:		type = rand()%4 + 15;		break;
	};
	Create(type);
}


void Piece::Create(int type) {
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			matrix[i][j] = false;
		}
	}

	this->type = type;
	switch (type) {
		case 0:
			matrix[0][0] = matrix[1][0] = matrix[0][1] = matrix[1][1] = true;
			wide = 2;
			high = 2;
			col.c1 = Color::red;
			break;

		case 1:
			matrix[0][0] = matrix[0][1] = matrix[1][1] = matrix[1][2] = true;
			wide = 2;
			high = 3;
			col.c1 = Color::green;
			break;

		case 2:
			matrix[0][1] = matrix[1][0] = matrix[1][1] = matrix[2][0] = true;
			wide = 3;
			high = 2;
			col.c1 = Color::green;
			break;

		case 3:
			matrix[0][1] = matrix[0][2] = matrix[1][0] = matrix[1][1] = true;
			wide = 2;
			high = 3;
			col.c1 = Color::cyan;
			break;

		case 4:
			matrix[0][0] = matrix[1][0] = matrix[1][1] = matrix[2][1] = true;
			wide = 3;
			high = 2;
			col.c1 = Color::cyan;
			break;

		case 5:
			matrix[0][0] = matrix[0][1] = matrix[0][2] = matrix[1][0] = true;
			wide = 2;
			high = 3;
			col.c1 = Color::yellow;
			break;

		case 6:
			matrix[0][0] = matrix[0][1] = matrix[1][1] = matrix[2][1] = true;
			wide = 3;
			high = 2;
			col.c1 = Color::yellow;
			break;

		case 7:
			matrix[0][2] = matrix[1][0] = matrix[1][1] = matrix[1][2] = true;
			wide = 2;
			high = 3;
			col.c1 = Color::yellow;
			break;

		case 8:
			matrix[0][0] = matrix[1][0] = matrix[2][0] = matrix[2][1] = true;
			wide = 3;
			high = 2;
			col.c1 = Color::yellow;
			break;

		case 9:
			matrix[0][0] = matrix[1][0] = matrix[1][1] = matrix[1][2] = true;
			wide = 2;
			high = 3;
			col.c1 = Color::orange;
			break;

		case 10:
			matrix[0][0] = matrix[0][1] = matrix[1][0] = matrix[2][0] = true;
			wide = 3;
			high = 2;
			col.c1 = Color::orange;
			break;

		case 11:
			matrix[0][0] = matrix[0][1] = matrix[0][2] = matrix[1][2] = true;
			wide = 2;
			high = 3;
			col.c1 = Color::orange;
			break;

		case 12:
			matrix[0][1] = matrix[1][1] = matrix[2][1] = matrix[2][0] = true;
			wide = 3;
			high = 2;
			col.c1 = Color::orange;
			break;

		case 13:
			matrix[0][0] = matrix[0][1] = matrix[0][2] = matrix[0][3] = true;
			wide = 1;
			high = 4;
			col.c1 = Color::magenta;
			break;

		case 14:
			matrix[0][0] = matrix[1][0] = matrix[2][0] = matrix[3][0] = true;
			wide = 4;
			high = 1;
			col.c1 = Color::magenta;
			break;

		case 15:
			matrix[0][0] = matrix[1][0] = matrix[2][0] = matrix[1][1] = true;
			wide = 3;
			high = 2;
			col.c1 = Color::blue;
			break;

		case 16:
			matrix[0][0] = matrix[0][1] = matrix[0][2] = matrix[1][1] = true;
			wide = 2;
			high = 3;
			col.c1 = Color::blue;
			break;

		case 17:
			matrix[0][1] = matrix[1][1] = matrix[2][1] = matrix[1][0] = true;
			wide = 3;
			high = 2;
			col.c1 = Color::blue;
			break;

		case 18:
			matrix[1][0] = matrix[1][1] = matrix[1][2] = matrix[0][1] = true;
			wide = 2;
			high = 3;
			col.c1 = Color::blue;
			break;
	};

	col.Make();
}

void MyColor::Make() {
	float h,s,v;
	c1.ToHSV(h,s,v);
	c1 = Color(h, s*0.7f, v*1.0f);
	c2 = Color(h, s*1.0f, v*0.9f);
	c3 = Color(h, s*0.7f, v*0.8f);
}

void Piece::RotateRight() {
	int newType = type;
	int oldType = type;
	switch (type) {
		case 0:		newType = 0;	break;
		case 1:		newType = 2;	break;
		case 2:		newType = 1;	break;
		case 3:		newType = 4;	break;
		case 4:		newType = 3;	break;
		case 5:		newType = 6;	break;
		case 6:		newType = 7;	break;
		case 7:		newType = 8;	break;
		case 8:		newType = 5;	break;
		case 9:		newType = 10;	break;
		case 10:	newType = 11;	break;
		case 11:	newType = 12;	break;
		case 12:	newType = 9;	break;
		case 13:	newType = 14;	break;
		case 14:	newType = 13;	break;
		case 15:	newType = 16;	break;
		case 16:	newType = 17;	break;
		case 17:	newType = 18;	break;
		case 18:	newType = 15;	break;
	};

	Create(newType);

	if (posx+wide > PLAYWIDTH) Create(oldType);

	if (!CanMove()) RotateLeft();
}

void Piece::RotateLeft() {
	int newType = type;
	switch (type) {
		case 0:		newType = 0;	break;
		case 1:		newType = 2;	break;
		case 2:		newType = 1;	break;
		case 3:		newType = 4;	break;
		case 4:		newType = 3;	break;
		case 5:		newType = 8;	break;
		case 6:		newType = 5;	break;
		case 7:		newType = 6;	break;
		case 8:		newType = 7;	break;
		case 9:		newType = 12;	break;
		case 10:	newType = 9;	break;
		case 11:	newType = 10;	break;
		case 12:	newType = 11;	break;
		case 13:	newType = 14;	break;
		case 14:	newType = 13;	break;
		case 15:	newType = 18;	break;
		case 16:	newType = 15;	break;
		case 17:	newType = 16;	break;
		case 18:	newType = 17;	break;
	};

	Create(newType);
	if (!CanMove()) RotateRight();
}

void Piece::MoveLeft() {
	if (posx <= 0) return;
	posx--;
	if (!CanMove()) posx++;
}

void Piece::MoveRight() {
	if (posx+wide >= PLAYWIDTH) return;
	posx++;
	if (!CanMove()) posx--;
}

bool Piece::MoveDown() {
	if (!CanMoveDown()) return false;
	posy--;
	return true;
}

bool Piece::CanMove() {
	int mx, my;
	for (int j=0; j<high; j++) {
		for (int i=0; i<wide; i++) {
			mx = posx + i;
			my = posy - j;

			if (matrix[i][j] && map->GetTile(mx, my).c1 != -1) {
				return false;
			}
		}
	}

	return true;
}

bool Piece::CanMoveLeft() {
	if (posx <= 0) return false;
	posx--;
	bool ret = CanMove();
	posx++;
	return ret;
}

bool Piece::CanMoveRight() {
	if (posx+wide >= PLAYWIDTH) return false;
	posx++;
	bool ret = CanMove();
	posx--;
	return ret;
}

bool Piece::CanMoveDown() {
	if (posy-high < 0) return false;
	posy--;
	bool ret = CanMove();
	posy++;
	return ret;
}

void Piece::Draw(Bitmap &canvas) {
	Draw(canvas, posx*PIECESIZE, (PLAYHEIGHT - 1 - posy)*PIECESIZE);
}

void Piece::Draw(Bitmap &canvas, int x, int y) {
	int px, py;
	for (int j=0; j<high; j++) {
		for (int i=0; i<wide; i++) {
			if (matrix[i][j]) {
				px = i*PIECESIZE + x;
				py = j*PIECESIZE + y;
				canvas.Rectfill( px,   py,   px+PIECESIZE-1, py+PIECESIZE-1, col.c1);
				canvas.Rectangle(px+1, py+1, px+PIECESIZE-2, py+PIECESIZE-2, col.c2);
				canvas.Rectangle(px+3, py+3, px+PIECESIZE-4, py+PIECESIZE-4, col.c3);
			}
		}
	}
}


void NextPiece::Draw(Bitmap &canvas) {
	if (piece) {
		canvas.Clear(skin->c_face);
		piece->Draw(canvas, 0, 0);
	}
}

PlayArea::PlayArea() {
	ClearPlayArea();
	Resize(PLAYWIDTH*PIECESIZE, PLAYHEIGHT*PIECESIZE);

	curPiece = NULL;
	nextPiece = NULL;
	timerID = -1;
	playing = false;
	lines = 0;
	score = 0;
}

PlayArea::~PlayArea() {
	if (playing) {
		GameOver();
	}

	if (curPiece) {
		delete curPiece;
		curPiece = NULL;
	}

	if (nextPiece) {
		delete nextPiece;
		nextPiece = NULL;
	}
}

void PlayArea::ClearPlayArea() {
	MyColor c;
	c.c1 = -1;
	for (int y=0; y<PLAYHEIGHT; y++) {
		for (int x=0; x<PLAYWIDTH; x++) {
			SetTile(x, y, c);
		}
	}
}

MyColor PlayArea::GetTile(int x, int y) {
	return tile[x][y];
}

void PlayArea::SetTile(int x, int y, const MyColor &val) {
	tile[x][y] = val;
}

void PlayArea::Draw(Bitmap &canvas) {
	canvas.Clear(Color::black);
	int x = 0;
	int y = PIECESIZE*PLAYHEIGHT;
	MyColor c;
	for (int j=0; j<PLAYHEIGHT; j++) {
		x = 0;
		y -= PIECESIZE;
		for (int i=0; i<PLAYWIDTH; i++) {
			//canvas.Rectangle(x, y, x+PIECESIZE, y+PIECESIZE, Color::darkgray);
			c = GetTile(i,j);
			if (c.c1) {
				//canvas.Rectfill(x, y, x+PIECESIZE, y+PIECESIZE, c);
				canvas.Rectfill( x,   y,   x+PIECESIZE-1, y+PIECESIZE-1, c.c1);
				canvas.Rectangle(x+1, y+1, x+PIECESIZE-2, y+PIECESIZE-2, c.c2);
				canvas.Rectangle(x+3, y+3, x+PIECESIZE-4, y+PIECESIZE-4, c.c3);
			}
			x += PIECESIZE;
		}
	}

	if (curPiece) {
		curPiece->Draw(canvas);
	}
}

void PlayArea::Play(int level) {
	if (curPiece) {
		delete curPiece;
		curPiece = NULL;
	}

	if (nextPiece) {
		delete nextPiece;
		nextPiece = NULL;
	}

	srand((unsigned)time(NULL));
	curPiece = new Piece;
	curPiece->map = this;
	curPiece->CreateRandomPiece();
	nextPiece = new Piece;
	nextPiece->map = this;
	nextPiece->CreateRandomPiece();
	parent->HandleEvent(*this, MyDialog::MSG_NEWPIECE);

	ClearPlayArea();
	frameInterval = 550 - level*50;
	timerID = Timer::Install(frameInterval);
	playing = true;
	lines = 0;
	score = 0;
	parent->HandleEvent(*this, MyDialog::MSG_UPDATESCORE, lines, score);
}

void PlayArea::MsgTimer(int t) {
	Widget::MsgTimer(t);

	if (t == timerID) {
		if (curPiece->MoveDown() == false) {
			DropCurrentPiece();
			delete curPiece;
			curPiece = nextPiece;
			nextPiece = new Piece;
			nextPiece->map = this;
			nextPiece->CreateRandomPiece();

			if (!curPiece->CanMoveDown()) {
				GameOver();
			}
			else {
				parent->HandleEvent(*this, MyDialog::MSG_NEWPIECE);
			}
		}
	}
}

void PlayArea::GameOver() {
	playing = false;
	Timer::Kill(timerID);
	timerID = -1;
	if (curPiece) {
		delete curPiece;
		curPiece = NULL;
	}
	parent->HandleEvent(*this, MyDialog::MSG_GAMEOVER);
}


void PlayArea::MsgTick() {
	Widget::MsgTick();

	if (playing) {
		CheckLines();
		Redraw();
	}
}


void PlayArea::DropCurrentPiece() {
	int mx, my;
	for (int j=0; j<curPiece->high; j++) {
		for (int i=0; i<curPiece->wide; i++) {
			if (curPiece->matrix[i][j]) {
				mx = curPiece->posx + i;
				my = curPiece->posy - j;
				SetTile(mx, my, curPiece->col);
			}
		}
	}
}

bool PlayArea::MsgChar(int c) {
	if (!curPiece || !playing) return false;

	switch (c>>8) {
		case KEY_LEFT:
			curPiece->MoveLeft();
			break;

		case KEY_RIGHT:
			curPiece->MoveRight();
			break;

		case KEY_DOWN:
			if (curPiece->MoveDown()) {
				parent->HandleEvent(*this, MyDialog::MSG_UPDATESCORE, lines, score += ((600 - frameInterval)/100));
			}
			break;

		case KEY_UP:
			curPiece->RotateRight();
			break;

		case KEY_SPACE:
			while (curPiece->MoveDown()) {
				parent->HandleEvent(*this, MyDialog::MSG_UPDATESCORE, lines, score += ((600 - frameInterval)/100));
			}
			break;

		case KEY_ESC:
			GameOver();
			break;

		default:
			return false;
	};

	return true;
}

void PlayArea::CheckLines() {
	int fullLines = 0;
	int oldLines = lines;

	for (int j=0; j<PLAYHEIGHT; j++) {
		if (LineFull(j)) {
			++fullLines;
			ClearLine(j);
			--j;
		}
	}

	lines += fullLines;
	switch (fullLines) {
		case 4:	score += 700;
		case 3:	score += 500;
		case 2:	score += 300;
		case 1:	score += 100;
	};

	if (fullLines) {
		parent->HandleEvent(*this, MyDialog::MSG_UPDATESCORE, lines, score);
		if (lines%20 < oldLines%20 && frameInterval > 120) {
			frameInterval -= 50;
			Timer::Adjust(timerID, frameInterval);
			parent->HandleEvent(*this, MyDialog::MSG_INCREASELEVEL);
		}
	}
}

bool PlayArea::LineFull(int j) {
	for (int i=0; i<PLAYWIDTH; i++) {
		if (GetTile(i, j).c1 == -1) {
			return false;
		}
	}

	return true;
}

void PlayArea::ClearLine(int j) {
	int i=0;
	for (; j<PLAYHEIGHT-1; j++) {
		for (i=0; i<PLAYWIDTH; i++) {
			SetTile(i, j, GetTile(i, j+1));
		}
	}
	MyColor c;
	c.c1 = -1;
	for (i=0; i<PLAYWIDTH; i++) {
		SetTile(i, j, c);
	}
}

HighscoreItem::HighscoreItem(const char *n, int s, int l) : score(s), lines(l) {
	ustrncpy(name, n, 8);
}

Highscore::Highscore(const char *fileName) : nItems(10), fileName(NULL) {
	packfile_password("TTRSHSCR");

	this->fileName = new char[ustrsizez(fileName)];
	ustrcpy(this->fileName, fileName);

	PACKFILE *file = pack_fopen(fileName, "r");
	if (!file) {
		MakeDefaultTable();
	}
	else {
		Load(file);
		pack_fclose(file);
	}

	packfile_password(NULL);
}

Highscore::~Highscore() {
	packfile_password("TTRSHSCR");

	PACKFILE *file = pack_fopen(fileName, "w");
	if (file) {
		Save(file);
		pack_fclose(file);
	}

	delete [] fileName;
	fileName = NULL;

	DestroyTable();

	packfile_password(NULL);
}

bool Highscore::CanAdd(int score) {
	return ((*(--items.end()))->score < score);
}

void Highscore::Add(const char *name, int score, int lines) {
	list<HighscoreItem *>::iterator i;
	for (i = items.begin(); i != items.end(); ++i) {
		if ((*i)->score < score) {
			break;
		}
	}

	items.insert(i, new HighscoreItem(name, score, lines));
	i = --items.end();
	delete *i;
	*i = NULL;
	items.erase(i);
}

void Highscore::MakeDefaultTable() {
	for (int i=0; i<nItems; ++i) {
		items.push_back(new HighscoreItem("---", 0, 0));
	}
}

void Highscore::Load(PACKFILE *f) {
	char buf[8];
	int score;
	int lines;

	DestroyTable();
	for (int i=0; i<nItems; ++i) {
		pack_fread(buf, 8, f);
		score = pack_igetl(f);
		lines = pack_getc(f);
		items.push_back(new HighscoreItem(buf, score, lines));
	}
}

void Highscore::Save(PACKFILE *f) {
	HighscoreItem *item;
	for (list<HighscoreItem *>::iterator i = items.begin(); i != items.end(); ++i) {
		item = *i;
		pack_fwrite(item->name, 8, f);
		pack_iputl(item->score, f);
		pack_putc(item->lines, f);
	}
}

void Highscore::DestroyTable() {
	for (list<HighscoreItem *>::iterator i = items.begin(); i != items.end(); ++i) {
		delete *i;
		*i = NULL;
	}
	items.clear();
}

MyDialog::MyDialog() {
	Add(desktop);

	lTitle.Shape(0,0,80,12);	lTitle.ClearFlag(D_AUTOSIZE);	lTitle.AlignRight();	lTitle.SetText("example 5:");
	lDriver.Shape(0,16,80,12);	lDriver.ClearFlag(D_AUTOSIZE);	lDriver.AlignRight();	lDriver.SetText("driver:");
	lFPS.Shape(0,32,80,12);		lFPS.ClearFlag(D_AUTOSIZE);		lFPS.AlignRight();		lFPS.SetText("FPS:");

	title.Shape(88,0,240,12);		title.SetText("Tetris");
	driver.Shape(88,16,240,12);
	fpsCounter.Shape(88,32,40,12);

	Add(lTitle);
	Add(lDriver);
	Add(lFPS);
	Add(title);
	Add(driver);
	Add(fpsCounter);

	bScores.Setup(500, 420, 134, 24, KEY_H, 0, "&High Scores");
	Add(bScores);

	bExit.Setup(500, 450, 134, 24, KEY_X, D_EXIT, "E&xit");
	Add(bExit);

	tetrisDialog.Add(pan);

	map.Place(10, 10);
	tetrisDialog.Add(map);

	buttonPlay.Setup(map.x2() + 10, map.y2() - 24, 130, 24, KEY_P, 0, "&Play");
	tetrisDialog.Add(buttonPlay);

	lLines.Setup(map.x2() + 10, map.y(), 60, 20, 0, 0, "Lines:", 0);
	eLines.Setup(map.x2() + 10, map.y()+20, 60, 24, 0, D_DISABLED, "0");
	lScore.Setup(map.x2() + 80, map.y(), 60, 20, 0, 0, "Score:", 0);
	eScore.Setup(map.x2() + 80, map.y()+20, 60, 24, 0, D_DISABLED, "0");
	lLevel.Setup(map.x2() + 10, map.y()+50, 60, 20, 0, 0, "Level:", 0);
	eLevel.Setup(map.x2() + 10, map.y()+70,130, 24, 0, 0, 1, 8, 1, 1);
	lNext.Setup(map.x2() + 10, map.y()+100, 70, 20, 0, 0, "Next piece:");
	nextPiece.Setup(map.x2() + 40, map.y()+130, PIECESIZE*4, PIECESIZE*4, 0, 0);
	lLeft.Setup(map.x2() + 16, map.y()+210, 70, 20, 0, 0, "LEFT - move left");
	lRight.Setup(map.x2() + 16, map.y()+230, 70, 20, 0, 0, "RIGHT - move right");
	lDown.Setup(map.x2() + 16, map.y()+250, 70, 20, 0, 0, "DOWN, SPACE - drop");
	lRotate.Setup(map.x2() + 16, map.y()+270, 70, 20, 0, 0, "UP - rotate");
	tetrisDialog.Add(lLines);
	tetrisDialog.Add(eLines);
	tetrisDialog.Add(lScore);
	tetrisDialog.Add(eScore);
	tetrisDialog.Add(lLevel);
	tetrisDialog.Add(eLevel);
	tetrisDialog.Add(lNext);
	tetrisDialog.Add(nextPiece);
	tetrisDialog.Add(lLeft);
	tetrisDialog.Add(lRight);
	tetrisDialog.Add(lDown);
	tetrisDialog.Add(lRotate);

	pan.Resize(buttonPlay.x2() + 20, map.h() + 20);

	tetrisDialog.Shape(140, 60, pan.w(), pan.h());
	tetrisDialog.SetFlag(D_WINDOW | D_MOVABLE);
	Add(tetrisDialog);

	char hscPath[1024];
	MakeFullPath(hscPath, "tetris.hsc");
	hsc = new Highscore(hscPath);
}

MyDialog::~MyDialog() {
	delete hsc;
}

void MyDialog::HandleEvent(Widget &obj, int msg, int arg1, int arg2) {
	Dialog::HandleEvent(obj, msg, arg1, arg2);

	switch (msg) {
		case MSG_ACTIVATE:
			if (obj == buttonPlay) {
				buttonPlay.Disable();
				MoveFocusTo(&map);
				map.Play((int)eLevel.GetPosition());
				eLevel.Disable();
				Settings::showMouse = false;
			}
			else if (obj == bScores) {
				ShowScores();
			}
		break;

		case MSG_GAMEOVER: {
			Settings::showMouse = true;

			if (hsc->CanAdd(eScore.GetInt())) {
				GetHiscoreName();
				ShowScores();
			}
			else {
				MessageBox dlg("Message", "Game over! You lose!", NULL, NULL, "OK", NULL, NULL);
				dlg.Popup(this);
			}
			buttonPlay.Enable();
			eLevel.Enable();
		}
		break;

		case MSG_UPDATESCORE: {
			eLines.SetNumber(arg1);
			eScore.SetNumber(arg2);
		}
		break;

		case MSG_INCREASELEVEL: {
			eLevel.Increment();
		}
		break;

		case MSG_NEWPIECE: {
			nextPiece.piece = map.GetNextPiece();
			nextPiece.Redraw();
		}
		break;
	};
}

void MyDialog::MsgStart() {
	Dialog::MsgStart();

	char buf[128];
	usprintf(buf, "%s, %d bpp", GetDriver()->GetDescription(), bitmap_color_depth(::screen));
	driver.SetText(buf);
}


void MyDialog::GetHiscoreName() {
	MAS::Window win;
	Dialog clientArea;
	ClearScreen clr;
	EditBox edit;
	Label l1, l2;
	Button but;

	clientArea.Add(clr);
	win.title.SetText("Game Over!");
	clientArea.Resize(278, 98);

	l1.Setup(10, 10, 200, 16, 0, 0, "You have a high score!", 0);
	l2.Setup(10, 38, 128, 16, 0, 0, "Enter your name:", 0);
	edit.Setup(150, 26, 100, 28, 0, D_EXIT, "player", 8);
	but.Setup(50, 60, 130, 24, KEY_O, D_EXIT, "&OK");

	clientArea.Add(l1);
	clientArea.Add(l2);
	clientArea.Add(edit);
	clientArea.Add(but);

	win.SetClientArea(&clientArea);
	win.Centre();
	edit.Select();
	win.Popup(this, win.x(), win.y(), &edit);
	hsc->Add(edit.GetText(), eScore.GetInt(), eLines.GetInt());
}


void MyDialog::ShowScores() {
	MAS::Window win;
	Dialog clientArea;
	ClearScreen clr;
	Panel pan;
	Label lName[10], lScore[10], lLines[10];
	Button but;

	clientArea.Add(clr);
	clientArea.Add(pan);
	list<HighscoreItem *>::iterator iter = hsc->items.begin();
	for (int i=0; i<10; i++, ++iter) {
		lName[i].ClearFlag(D_AUTOSIZE);
		lName[i].Shape(16, i*20 + 10, 80, 20);
		lName[i].SetText((*iter)->name);
		clientArea.Add(lName[i]);

		lScore[i].ClearFlag(D_AUTOSIZE);
		lScore[i].AlignCentre();
		lScore[i].Shape(108, i*20 + 10, 40, 20);
		lScore[i].SetNumber((*iter)->score);
		clientArea.Add(lScore[i]);

		lLines[i].ClearFlag(D_AUTOSIZE);
		lLines[i].AlignCentre();
		lLines[i].Shape(150, i*20 + 10, 40, 20);
		lLines[i].SetNumber((*iter)->lines);
		clientArea.Add(lLines[i]);
	}

	but.Setup(40, 220, 120, 24, KEY_O, D_EXIT, "&OK");
	clientArea.Add(but);
	pan.Shape(4, 4, 192, 248);
	clientArea.Resize(200,256);
	win.title.SetText("High Scores");
	win.SetClientArea(&clientArea);
	win.ClearFlag(D_RESIZABLE);
	win.Centre();
	win.Popup(this, win.x(), win.y(), &but);
}

bool MyDialog::MsgClose() {
	Dialog::MsgClose();
	MessageBox msg("example 5: Tetris", "Are you sure you want to exit the game?", NULL, NULL, "Yes", "No");
	return (msg.Popup(this) == 1 ? true : false);
}


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
