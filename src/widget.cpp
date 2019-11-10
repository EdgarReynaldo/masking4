////////////////////////////////////////////////////////////////////////////////
//         MASkinG - Miran Amon's Skinnable GUI Library for Allegro           //
//    Copyright (c) 2002-2005 Miran Amon (miranamon@users.sourceforge.net)    //
//          Project websites: http://ferisrv5.uni-mb.si/~ma0747               //
//                  http://sourceforge.net/projects/masking                   //
////////////////////////////////////////////////////////////////////////////////

#include "../include/MASkinG/debug.h"
#include "../include/MASkinG/widget.h"
#include "../include/MASkinG/dialog.h"
#include "../include/MASkinG/settings.h"


MAS::Widget::Widget()
	:MAS::Rect(0,0,0,0),
	flags(0),
	key(0),
	callbackID(-1),
	parent(NULL),
	bitmap(-1),
	animator(NULL),
	animLength(1),
	animType(Animator::NONE),
	tooltipText(NULL)
{
	skin = NULL;
	SetSkin(MAS::theSkin);

	SetAnimator(new Animator);

	normalizedRect = Rect(-1,-1,-1,-1);
}


MAS::Widget::~Widget() {
	parent = NULL;
	skin = NULL;
	if (animator) {
		delete animator;
		animator = NULL;
	}
}


void MAS::Widget::x(int i, bool normalized) {
	if (normalized) {
		normalizedRect.x(i);
	}
	else {
		int realPos = i < 0 ? GetParent()->w() + i : i;
		relativePos.x(realPos);
		if (parent && parent != this) {
			Point p = parent->GetOffset();
			Rect::x(realPos + p.x());
		}
		else {
			Rect::x(realPos);
		}
		//normalizedRect.x(-1);
	}

	Redraw();
	SetFlag(D_MOVED);
}


void MAS::Widget::y(int i, bool normalized) {
	if (normalized) {
		normalizedRect.y(i);
	}
	else {
		int realPos = i < 0 ? GetParent()->h() + i : i;
		relativePos.y(realPos);
		if (parent && parent != this) {
			Point p = parent->GetOffset();
			Rect::y(realPos + p.y());
		}
		else {
			Rect::y(realPos);
		}
		//normalizedRect.y(-1);
	}

	Redraw();
	SetFlag(D_MOVED);
}


void MAS::Widget::w(int i, bool normalized) {
	if (normalized) {
		normalizedRect.w(i);
	}
	else {
		Rect::w(i);
		//normalizedRect.w(-1);
	}

	Redraw();
	SetFlag(D_RESIZED);
}


void MAS::Widget::h(int i, bool normalized) {
	if (normalized) {
		normalizedRect.h(i);
	}
	else {
		Rect::h(i);
		//normalizedRect.h(-1);
	}

	Redraw();
	SetFlag(D_RESIZED);
}


void MAS::Widget::UpdatePosition() {
	if (parent && parent != this) {
		Point p = parent->GetOffset();
		Rect::x(relativePos.x() + p.x());
		Rect::y(relativePos.y() + p.y());
	}
}


int MAS::Widget::SendMessage(int msg, int c) {
	int ret = D_O_K;

	// convert the Allegro message constant to a MASkinG message handler
	switch (msg) {
		case (MSG_START):	MsgStart();	Animate();	break;
		case (MSG_END):		MsgEnd();		break;
		case (MSG_DRAW):	MsgDraw();		break;
		case (MSG_CLICK):	MsgClick();		break;
		case (MSG_DCLICK):	MsgDClick();	break;
		case (MSG_KEY):		MsgKey();		break;
		case (MSG_CHAR):	if (MsgChar(c))		ret |= D_USED_CHAR;		break;
		case (MSG_UCHAR):	if (MsgUChar(c))	ret |= D_USED_CHAR;		break;
		case (MSG_XCHAR):	if (MsgXChar(c))	ret |= D_USED_CHAR;		break;
		case (MSG_WANTFOCUS):	if (MsgWantfocus())	ret |= D_WANTFOCUS;	break;
		case (MSG_GOTFOCUS):	MsgGotfocus();		break;
		case (MSG_LOSTFOCUS):	MsgLostfocus();		break;
		case (MSG_GOTMOUSE):	MsgGotmouse();		break;
		case (MSG_LOSTMOUSE):	MsgLostmouse();		break;
		case (MSG_IDLE):		MsgIdle();			break;
		case (MSG_RADIO):		MsgRadio(c);		break;
		case (MSG_WHEEL):		MsgWheel(c);		break;
		case (MSG_LPRESS):		MsgLPress();		break;
		case (MSG_MPRESS):		MsgMPress();		break;
		case (MSG_RPRESS):		MsgRPress();		break;
		case (MSG_LRELEASE):	MsgLRelease();		break;
		case (MSG_MRELEASE):	MsgMRelease();		break;
		case (MSG_RRELEASE):	MsgRRelease();		break;
		case (MSG_MOVE):		MsgMove();			break;
		case (MSG_RESIZE):		MsgResize();		break;
		case (MSG_SHAPE):		MsgShape();			break;
		case (MSG_TIMER):		MsgTimer(c);		break;
		case (MSG_TICK):		MsgTick();			break;
		case (MSG_INITSKIN):	MsgInitSkin();		break;
		case (MSG_CLOSE):		if (MsgClose()) ret |= D_CLOSE;		break;
		case (MSG_WANTMOUSE):	if (MsgWantmouse())	ret |= D_WANTFOCUS;	break;
		case (MSG_WANTTAB):		if (MsgWantTab())	ret |= D_WANTFOCUS;	break;
	};

	return ret;
}


void MAS::Widget::MsgStart()		{
	UpdatePosition();
	GetParent()->HandleEvent(*this, MSG_START);
}

void MAS::Widget::MsgEnd()			{
	GetParent()->HandleEvent(*this, MSG_END);
	ClearFlag(D_GOTFOCUS | D_GOTMOUSE | D_CHANGEDSKIN | D_MOVED | D_RESIZED);
	ResetAnimation();
}


void MAS::Widget::MsgDraw()			{
	Bitmap canvas = GetCanvas();

	if (canvas) {
		while (!redrawRegion.empty()) {
			Rect clipRect = redrawRegion.back();
			canvas.SetClip(clipRect - topLeft());
			if ((flags & D_ANIMATING) && !animator->Animating()) {
				animator->Create(canvas, animLength);
				Bitmap aBuffer = animator->GetBuffer();
				if (aBuffer) {
					aBuffer.SetClip(clipRect - topLeft());
					Draw(aBuffer);
					animator->Draw(canvas, animType);
				}
				ClearFlag(D_ANIMATING);
			}
			else if (!(flags & D_ANIMATING) && animator->Animating()) {
				animator->Draw(canvas, animType);
			}
			else {
				Draw(canvas);
#ifdef DEBUGMODE
				if (HasMouse() || parent->GetMouseObject() == this) {
					canvas.Rectangle(0, 0, canvas.w()-1, canvas.h()-1, MAS::Color::red);
				}
				if (HasFocus() || parent->GetFocusObject() == this) {
					canvas.Rectangle(1, 1, canvas.w()-2, canvas.h()-2, MAS::Color::blue);
				}
#endif
			}

			parent->GetDriver()->InvalidateRect(clipRect);
			redrawRegion.pop_back();
		}

		ReleaseCanvas(canvas);
	}

	GetParent()->HandleEvent(*this, MSG_DRAW);
}


void MAS::Widget::Draw(Bitmap &canvas) {
}

void MAS::Widget::MsgClick()		{ GetParent()->HandleEvent(*this, MSG_CLICK); }
void MAS::Widget::MsgDClick()		{ GetParent()->HandleEvent(*this, MSG_DCLICK); }
void MAS::Widget::MsgKey() 			{ GetParent()->HandleEvent(*this, MSG_KEY); }
bool MAS::Widget::MsgChar(int c)	{ GetParent()->HandleEvent(*this, MSG_CHAR, c);	return false; }
bool MAS::Widget::MsgUChar(int c)	{ GetParent()->HandleEvent(*this, MSG_UCHAR, c);	return false; }
bool MAS::Widget::MsgXChar(int xc)	{ GetParent()->HandleEvent(*this, MSG_XCHAR, xc);	return false; }
bool MAS::Widget::MsgWantfocus()	{ GetParent()->HandleEvent(*this, MSG_WANTFOCUS);	return false; }
bool MAS::Widget::MsgWantmouse()	{ GetParent()->HandleEvent(*this, MSG_WANTMOUSE);	return true; }

void MAS::Widget::MsgGotfocus()		{
	SetFlag(D_GOTFOCUS);
	Redraw();
	GetParent()->HandleEvent(*this, MSG_GOTFOCUS);
	PlaySample(Skin::SAMPLE_GOTFOCUS);
}

void MAS::Widget::MsgLostfocus()	{
	ClearFlag(D_GOTFOCUS);
	Redraw();
	GetParent()->HandleEvent(*this, MSG_LOSTFOCUS);
	PlaySample(Skin::SAMPLE_LOSTFOCUS);
}

void MAS::Widget::MsgGotmouse()		{ SetFlag(D_GOTMOUSE); GetParent()->HandleEvent(*this, MSG_GOTMOUSE); }
void MAS::Widget::MsgLostmouse()	{ ClearFlag(D_GOTMOUSE); GetParent()->HandleEvent(*this, MSG_LOSTMOUSE); }
void MAS::Widget::MsgIdle() {}
void MAS::Widget::MsgRadio(int r)	{ GetParent()->HandleEvent(*this, MSG_RADIO, r); }
void MAS::Widget::MsgWheel(int wh)	{ GetParent()->HandleEvent(*this, MSG_WHEEL, wh); }
void MAS::Widget::MsgLPress()		{ GetParent()->HandleEvent(*this, MSG_LPRESS); }
void MAS::Widget::MsgMPress()		{ GetParent()->HandleEvent(*this, MSG_MPRESS); }
void MAS::Widget::MsgRPress()		{ GetParent()->HandleEvent(*this, MSG_RPRESS); }
void MAS::Widget::MsgLRelease()		{ GetParent()->HandleEvent(*this, MSG_LRELEASE); }
void MAS::Widget::MsgMRelease()		{ GetParent()->HandleEvent(*this, MSG_MRELEASE); }
void MAS::Widget::MsgRRelease()		{ GetParent()->HandleEvent(*this, MSG_RRELEASE); }
void MAS::Widget::MsgTimer(int t)	{ GetParent()->HandleEvent(*this, MSG_TIMER); }

void MAS::Widget::MsgTick() {
	if (animator->Animating()) {
		if (!animator->Update()) {
			StopAnimating();
		}
		else {
			//Redraw();
			parent->Root()->Redraw(*this);
		}
	}
}

void MAS::Widget::MsgInitSkin() {
	GetParent()->HandleEvent(*this, MSG_INITSKIN);
	ClearFlag(D_CHANGEDSKIN);
}

void MAS::Widget::MsgMove() {
	GetParent()->HandleEvent(*this, MSG_MOVE);
	ClearFlag(D_MOVED);

	if (normalizedRect.x() != -1) {
		x(GetParent()->w()*normalizedRect.x()/100);
	}

	if (normalizedRect.y() != -1) {
		y(GetParent()->h()*normalizedRect.y()/100);
	}
}

void MAS::Widget::MsgResize() {
	GetParent()->HandleEvent(*this, MSG_RESIZE);
	ClearFlag(D_RESIZED);

	if (normalizedRect.w() != -1) {
		w(GetParent()->w()*normalizedRect.w()/100);
	}

	if (normalizedRect.h() != -1) {
		h(GetParent()->h()*normalizedRect.h()/100);
	}
}

void MAS::Widget::MsgShape() {
	GetParent()->HandleEvent(*this, MSG_SHAPE);
	ClearFlag(D_MOVED | D_RESIZED);

	if (normalizedRect.x() != -1) {
		x(GetParent()->w()*normalizedRect.x()/100);
	}

	if (normalizedRect.y() != -1) {
		y(GetParent()->h()*normalizedRect.y()/100);
	}

	if (normalizedRect.w() != -1) {
		w(GetParent()->w()*normalizedRect.w()/100);
	}

	if (normalizedRect.h() != -1) {
		h(GetParent()->h()*normalizedRect.h()/100);
	}
}

void MAS::Widget::MsgMousemove(const Point& d) {
	GetParent()->HandleEvent(*this, MSG_MOUSEMOVE, d.x(), d.y());
}
bool MAS::Widget::MsgClose() {		return true;	}
bool MAS::Widget::MsgWantTab() {	return MsgWantfocus();	}


void MAS::Widget::Setup(int x, int y, int w, int h, int key, int flags) {
	Shape(x, y, w, h);
	this->key = key;
	this->flags |= flags;
}


BITMAP *MAS::Widget::GetCanvas() {
	return parent->GetCanvas(this);
}


void MAS::Widget::ReleaseCanvas(BITMAP *bmp) {
	if (bmp && is_sub_bitmap(bmp)) destroy_bitmap(bmp);
}


void MAS::Widget::SetSkin(MAS::Skin *skin) {
	int i;
	for (i=0; i<4; i++) {
		fontInfo.fntCol[i] = -1;
		fontInfo.shdCol[i] = -1;
		fontInfo.fnt[i] = -1;
	}
	fontInfo.textMode = -1;

	for (i=0; i<7; i++) {
		sample[i] = -1;
	}

	SetFlag(D_CHANGEDSKIN);
	if (skin) {
		this->skin = skin;
	}
	else {
		this->skin = MAS::theSkin;
	}
	if (!Hidden()) {
		Redraw();
	}
}


MAS::Skin *MAS::Widget::GetSkin() {
	return skin;
}


MAS::Point MAS::Widget::GetMousePos() const {
	return parent->GetMouse()->pos - topLeft();
	//return MAS::Point(mouse_x - x(), mouse_y - y());
	//return parent->GetMousePos() - topLeft();
}


void MAS::Widget::SetCursor(int i) {
	if (parent) {
		parent->SetMouseCursor(i);
	}
}


void MAS::Widget::PlaySample(int i) const {
	if (MAS::Settings::guiSound && sample[i] >= 0) {
		skin->PlaySample(sample[i]);
	}
}


void MAS::Widget::SetAnimator(Animator *a) {
	if (!a) {
		return;
	}

	if (animator) {
		delete animator;
	}

	animator = a;
}


void MAS::Widget::SetAnimationProperties(int length, int type) {
	animLength = length;
	animType = type;
}


void MAS::Widget::Animate() {
	StopAnimating();
	if (animType != Animator::NONE) {
		SetFlag(D_ANIMATING);
	}
}


void MAS::Widget::Animate(int type) {
	animType = type;
	Animate();
}


void MAS::Widget::StopAnimating() {
	ClearFlag(D_ANIMATING);
	//animator->Reset();
}


void MAS::Widget::ResetAnimation() {
	ClearFlag(D_ANIMATING);
	animator->Reset();
}


void MAS::Widget::SetTooltipText(const char *text) {
	if (tooltipText) {
		delete [] tooltipText;
		tooltipText = NULL;
	}

	if (text) {
		tooltipText = new char[ustrsizez(text)];
		ustrcpy(tooltipText, text);
	}
}


const char *MAS::Widget::GetTooltipText() {
	if (tooltipText) {
		return tooltipText;
	}
	else if (parent != this) {
		return parent->GetTooltipText();
	}

	return NULL;
}


int MAS::Widget::x()	const { return Rect::x(); }
int MAS::Widget::y()	const { return Rect::y(); }
int MAS::Widget::x2()	const { return Rect::x2(); }
int MAS::Widget::y2()	const { return Rect::y2(); }
int MAS::Widget::w()	const { return Rect::w(); }
int MAS::Widget::h()	const { return Rect::h(); }

MAS::Point MAS::Widget::RelativePos() const { return relativePos; }
int MAS::Widget::Key()		const { return key; };
int MAS::Widget::Flags()		const { return flags; };
bool MAS::Widget::TestFlag(int i)	const { return ((flags & i) == i); }
bool MAS::Widget::HasFocus() { return ((flags & D_GOTFOCUS) ? true : false); }
bool MAS::Widget::HasMouse() { return ((flags & D_GOTMOUSE) ? true : false); }
bool MAS::Widget::Hidden()	const { return ((flags & D_HIDDEN) ? true : false); }
bool MAS::Widget::WillExit() const { return ((flags & D_EXIT) ? true : false); }
bool MAS::Widget::Selected() const { return ((flags & D_SELECTED) ? true : false); }
bool MAS::Widget::Disabled() const { return ((flags & D_DISABLED) ? true : false); }
bool MAS::Widget::Dirty()	const { return ((flags & D_DIRTY) ? true : false); }
bool MAS::Widget::HasChildren() const { return ((flags & D_HASCHILDREN) ? true : false); }
bool MAS::Widget::IsWindow() const { return ((flags & D_WINDOW) ? true : false); }
MAS::Dialog *MAS::Widget::GetParent() const { return parent; }

bool MAS::Widget::IsChildOf(MAS::Widget *w) const {
	if (parent == w) {
		return true;
	}
	else if (parent != this) {
		return parent->IsChildOf(w);
	}
	else {
		return false;
	}
}


void MAS::Widget::Key(int i) 		{ key = i; }
void MAS::Widget::SetFlag(int i)	{ flags |= i; }
void MAS::Widget::SetFlags(int f)	{ flags = f; }
void MAS::Widget::ClearFlag(int i)	{ flags &= ~i; }
void MAS::Widget::Enable()			{ flags &= ~D_DISABLED;		Redraw(); }
void MAS::Widget::Disable()			{ flags |= D_DISABLED;		Redraw(); }

void MAS::Widget::Hide() {
	flags |= D_HIDDEN;
	/*
	if (GetParent()) {
		Dialog *root = GetParent()->Root();
		if (root) {
			root->Redraw(*this);
		}
	}
	*/
}

void MAS::Widget::Unhide()			{ flags &= ~D_HIDDEN;	Redraw(); }
void MAS::Widget::MakeExit()		{ flags |= D_EXIT;		Redraw(); }
void MAS::Widget::DontExit()		{ flags &= ~D_EXIT;		Redraw(); }
void MAS::Widget::Place(int nx, int ny, bool normalized)	{ x(nx,normalized); y(ny,normalized); }
void MAS::Widget::Place(const Point& p, bool normalized) { x(p.x(),normalized); y(p.y(),normalized); }
void MAS::Widget::Resize(int nw, int nh, bool normalized) { w(nw,normalized); h(nh,normalized); }
void MAS::Widget::Resize(const Size& s, bool normalized) { w(s.w(),normalized); h(s.h(),normalized); }
void MAS::Widget::Shape(int nx, int ny, int nw, int nh, bool normalized) { Resize(nw, nh, normalized); Place(nx, ny, normalized); }
void MAS::Widget::Shape(const Point& p, const Size& s, bool normalized) { Resize(s, normalized); Place(p, normalized); }
void MAS::Widget::Shape(const Rect &r, bool normalized) { Shape(r.x(), r.y(), r.w(), r.h(), normalized); }
void MAS::Widget::Select()			{ flags |= D_SELECTED;		Redraw(); }
void MAS::Widget::Deselect()		{ flags &= ~D_SELECTED;		Redraw(); }
void MAS::Widget::SetParent(Dialog *f) { parent = f; }

bool MAS::Widget::operator==(const Widget& obj) const { return (this == &obj); }
bool MAS::Widget::operator!=(const Widget& obj) const { return (this != &obj); }

void MAS::Widget::SetTextMode(Color m)	{ fontInfo.textMode = m;	}
void MAS::Widget::SetFontColor(Color col, Color shd, int type) { fontInfo.fntCol[type] = col; fontInfo.shdCol[type] = shd; }
void MAS::Widget::SetFont(int f, int type) { fontInfo.fnt[type] = f; }
MAS::Font& MAS::Widget::GetFont(int type) { return skin->GetFont(fontInfo.fnt[type] >= 0 ? fontInfo.fnt[type] : 0); }
MAS::Color& MAS::Widget::GetTextMode() { return fontInfo.textMode; }
MAS::Color& MAS::Widget::GetFontColor(int type) { return fontInfo.fntCol[type]; }
MAS::Color& MAS::Widget::GetShadowColor(int type) { return fontInfo.shdCol[type]; }
int MAS::Widget::GetFontIndex(int type) { return fontInfo.fnt[type]; }
void MAS::Widget::SetBitmap(int i) { bitmap = i; }
int MAS::Widget::GetBitmapIndex() const { return bitmap; }
MAS::Bitmap& MAS::Widget::GetBitmap() const { return skin->GetBitmap(bitmap); }
void MAS::Widget::SetSample(int i, int event) { sample[event] = i; }
int MAS::Widget::GetSampleIndex(int event) const { return sample[event]; }
MAS::Sample& MAS::Widget::GetSample(int event) const { return skin->GetSample(sample[event]); }

void MAS::Widget::Redraw() {
	/*
	if (parent) {
		parent->Root()->RedrawRegion(*this);
	}
	*/

	if (w() && h()) {
		redrawRegion.clear();
		redrawRegion.push_back(*this);
		flags |= D_DIRTY;
	}
}


////////////////////////////////////////////////////////////////////////////////
// Check if a rectangle can be safely added to the redraw region list
// (copied&pasted from the DRS update driver)
MAS::Rect *MAS::Widget::CanAdd(const MAS::Rect& r) {
	// See if there is a rectangle in the way
	MAS::Rect topLeft(r.x2() - MAXINT, r.y2() - MAXINT, MAXINT, MAXINT);
	MAS::Rect bottomRight(r.x(), r.y(), MAXINT, MAXINT);
	MAS::Point topLeftS, bottomRightS;
	for (std::list<MAS::Rect>::iterator i = redrawRegion.begin(); i != redrawRegion.end(); ++i) {
		if (i->topLeft() < topLeft && i->bottomRight() < bottomRight) {
			// found one!
			return &*i;
		}
	}

	return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// Marks the widget dirty by setting the appropriate flag and adding a
// rectangle into the redraw region list. If it can't be added it is split in
// 4 parts which are then added recursively.
// (mostly copied&pasted from the DRS update driver)
void MAS::Widget::Redraw(const Rect& dirtyRect) {
	Rect r = dirtyRect;

	if (!redrawRegion.empty()) {
		if (TestFlag(D_DIRTY) && redrawRegion.front() == *this) {
			return;
		}
	}

	// If we're redrawing the whole widget, then there's no point in splitting
	// anything into little rects...
	if (r == *this) {
		redrawRegion.clear();
		redrawRegion.push_back(*this);
		flags |= D_DIRTY;
		return;
	}

	// make sure the rect is on the screen and inside the widget otherwise
	// there's no point in redrawing anything
	int tmp;
	tmp = MAX(0, x());
	if (r.x() < tmp) {
		r.w(r.x2() - tmp);
		r.x(tmp);
		if (r.w() <= 0) return;
	}
	tmp = MAX(0, y());
	if (r.y() < tmp) {
		r.h(r.y2() - tmp);
		r.y(tmp);
		if (r.h() <= 0) return;
	}
	tmp = MIN(SCREEN_W, x2());
	if (r.x2() >= tmp) {
		r.w(tmp - r.x());
		if (r.w() <= 0) return;
	}
	tmp = MIN(SCREEN_H, y2());
	if (r.y2() >= tmp) {
		r.h(tmp - r.y());
		if (r.h() <= 0) return;
	}

	// Check if the rect can be added to the list in its entirety. If not, break it
	// up into smaller pieces (recursively).
	MAS::Rect *s = CanAdd(r);
	if (!s) {
		redrawRegion.push_back(r);
		flags |= D_DIRTY;
	}
	else {
		int x1 = r.x();
		int x2 = r.x2();
		int y1 = r.y();
		int y2 = r.y2();

		// Get the coordinates of the rectangle that is in the way
		int wx1 = s->x();
		int wy1 = s->y();
		int wx2 = s->x2();
		int wy2 = s->y2();

		// Now check which corner points of the rect we're trying to add are
		// inside the rect that is on top and split the first rect accordingly
		int inside = 0;

		inside |= (((MAS::Point(x1, y1) <= *s) ? 1 : 0)<<0);		// upper-left
		inside |= (((MAS::Point(x2, y1) <= *s) ? 1 : 0)<<1);		// upper-right
		inside |= (((MAS::Point(x1, y2) <= *s) ? 1 : 0)<<2);		// lower-left
		inside |= (((MAS::Point(x2, y2) <= *s) ? 1 : 0)<<3);		// lower right

		switch (inside) {
			// no corners are inside the window (the window is inside the rectangle) -->
			// --> split the rectangle in 3 or 4 pieces
			case 0x00:
				if (wy1 > y1)	Redraw(MAS::Rect(x1, y1, r.w(), wy1-y1));
				if (wx1 > x1)	Redraw(MAS::Rect(x1, MAX(y1, wy1), wx1-x1, MIN(y2, wy2) - MAX(y1, wy1)));
				if (wx2 < x2)	Redraw(MAS::Rect(wx2, MAX(y1, wy1), x2-wx2, MIN(y2, wy2) - MAX(y1, wy1)));
				if (wy2 < y2)	Redraw(MAS::Rect(x1, wy2, r.w(), y2-wy2));
				break;

			// only the top left is inside --> split in 2 rectangles
			case 0x01:
				Redraw(MAS::Rect(wx2, y1, x2-wx2, wy2-y1));
				Redraw(MAS::Rect(x1, wy2, r.w(), y2-wy2));
				break;

			// only the top right is inside --> 2 rectangles
			case 0x02:
				Redraw(MAS::Rect(x1, y1, wx1-x1, wy2-y1));
				Redraw(MAS::Rect(x1, wy2, r.w(), y2-wy2));
				break;

			// both top corners are inside --> add the bottom half
			case 0x03:
				Redraw(MAS::Rect(x1, wy2, r.w(), y2-wy2));
				break;

			// only the lower left is inside --> 2 rectangles
			case 0x04:
				Redraw(MAS::Rect(x1, y1, r.w(), wy1-y1));
				Redraw(MAS::Rect(wx2, wy1, x2-wx2, y2-wy1));
				break;

			// both left corners are inside --> add the right half
			case 0x05:
				Redraw(MAS::Rect(wx2, y1, x2-wx2, r.h()));
				break;

			// only the lower right is inside --> 2 rectangles
			case 0x08:
				Redraw(MAS::Rect(x1, y1, r.w(), wy1-y1));
				Redraw(MAS::Rect(x1, wy1, wx1-x1, y2-wy1));
				break;

			// both right corners are inside --> add the left half
			case 0x0A:
				Redraw(MAS::Rect(x1, y1, wx1-x1, r.h()));
				break;

			// both lower corners are inside --> add the top half
			case 0x0C:
				Redraw(MAS::Rect(x1, y1, r.w(), wy1-y1));
				break;
		};
	}
}


void MAS::Widget::SetCallbackID(int ID) {
	callbackID = ID;
}


int MAS::Widget::GetCallbackID() {
	return callbackID;
}
