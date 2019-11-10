////////////////////////////////////////////////////////////////////////////////
//         MASkinG - Miran Amon's Skinnable GUI Library for Allegro           //
//    Copyright (c) 2002-2005 Miran Amon (miranamon@users.sourceforge.net)    //
//          Project websites: http://ferisrv5.uni-mb.si/~ma0747               //
//                  http://sourceforge.net/projects/masking                   //
////////////////////////////////////////////////////////////////////////////////

#include "../include/MASkinG/bitmap.h"
#include "../include/MASkinG/settings.h"
#include <string.h>


MAS::Bitmap::Bitmap()
	:data(NULL),
	type(MEMORY),
	allocated(false),
	thick(-1,-1)
{
	TRACE("ctor (default)\n");
}


MAS::Bitmap::Bitmap(int w, int h, Type t)
	:data(NULL),
	thick(-1,-1)
{
	TRACE("ctor (w, h)\n");
	Create(w, h, t);
}


MAS::Bitmap::Bitmap(const Size &s, Type t)
	:data(NULL),
	thick(-1,-1)
{
	TRACE("ctor (size)\n");
	Create(s, t);
}


MAS::Bitmap::Bitmap(const char *file, Type t)
	:data(NULL),
	type(t),
	thick(-1,-1)
{
	TRACE("ctor (path)\n");
	Load(file, t);
}


MAS::Bitmap::Bitmap(BITMAP *bmp) :thick(-1,-1) {
	TRACE("ctor (BITMAP)\n");
	if (!bmp) {
		type = Bitmap::UNDEFINED;
		data = NULL;
	}
	else {
		if (is_video_bitmap(bmp)) {
			type = Bitmap::VIDEO;
		}
		else if (is_system_bitmap(bmp)) {
			type = Bitmap::SYSTEM;
		}
		else {
			type = Bitmap::MEMORY;
		}
	}
	data = bmp;
	allocated = false;
}


MAS::Bitmap::Bitmap(const Bitmap &bmp) {
	TRACE("ctor (Bitmap)\n");
	data = bmp.data;
	type = bmp.type;
	thick = bmp.thick;
	allocated = false;
}


MAS::Bitmap::~Bitmap() {
	TRACE("dtor\n");
	Destroy();
}


/*
MAS::Bitmap::operator bool() const {
	TRACE("bool()\n");
	return (data != NULL);
}
*/


MAS::Bitmap::operator BITMAP*() const {
	TRACE("BITMAP*()\n");
	return data;
}


MAS::Bitmap& MAS::Bitmap::operator=(BITMAP *bmp) {
	TRACE("=(BITMAP*)\n");
	Destroy();
	if (!bmp) {
		type = Bitmap::UNDEFINED;
		data = NULL;
	}
	else {
		if (is_video_bitmap(bmp)) {
			type = Bitmap::VIDEO;
		}
		else if (is_system_bitmap(bmp)) {
			type = Bitmap::SYSTEM;
		}
		else {
			type = Bitmap::MEMORY;
		}
	}
	data = bmp;
	allocated = false;
	thick.w(-1);
	thick.h(-1);
	return *this;
}


MAS::Bitmap& MAS::Bitmap::operator=(const Bitmap &bmp) {
	TRACE("=(Bitmap&)\n");
	Destroy();
	data = bmp.data;
	type = bmp.type;
	thick = bmp.thick;
	allocated = false;
	return *this;
}


MAS::Bitmap& MAS::Bitmap::Set(BITMAP *bmp, bool copy, Type t) {
	TRACE("set (BITMAP*)\n");
	Destroy();
	if (!bmp) {
		data = NULL;
		type = UNDEFINED;
		allocated = false;
	}
	else {
		if (copy && bmp) {
			if (t != UNDEFINED) {
				type = t;
			}
			else {
				type = MEMORY;
				if (is_system_bitmap(bmp))
					type = SYSTEM;
				if (is_video_bitmap(bmp))
					type = VIDEO;
			}
			Create(bmp->w, bmp->h, type);
			blit(bmp, data, 0, 0, 0, 0, bmp->w, bmp->h);
			
			allocated = true;
		}
		else {
			data = bmp;
			if (is_video_bitmap(bmp)) {
				type = Bitmap::VIDEO;
			}
			else if (is_system_bitmap(bmp)) {
				type = Bitmap::SYSTEM;
			}
			else {
				type = MEMORY;
			}
			allocated = false;
		}
	}
	return *this;
}


MAS::Bitmap& MAS::Bitmap::Set(const Bitmap &bmp, bool copy, Type t) {
	TRACE("set (Bitmap&)\n");
	Destroy();
	if (copy && bmp) {
		if (t != UNDEFINED) {
			type = t;
		}
		else {
			type = bmp.type;
		}
		Create(bmp.w(), bmp.h(), type);
		blit(bmp, data, 0, 0, 0, 0, w(), h());
		allocated = true;
	}
	else {
		data = bmp.data;
		type = bmp.type;
		allocated = false;
	}
	return *this;
}


MAS::Error MAS::Bitmap::Load(const char *file, Type t) {
	TRACE("load\n");
	if (!file_exists(file, FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH, NULL)) {
		return Error(Error::NO_FILE);
	}

	BITMAP *tmp = load_bitmap(file, NULL);
	if (!tmp) {
		return Error(Error::LOAD_FILE);
	}

	Destroy();
	if (t == SYSTEM || t == VIDEO) {
		Create(Size(tmp->w, tmp->h));
		blit(tmp, data, 0, 0, 0, 0, tmp->w, tmp->h);
		destroy_bitmap(tmp);
		type = t;
		allocated = true;
	}
	else {
		data = tmp;
		type = MEMORY;
		allocated = true;
	}

	return Error(Error::NONE);
}


void MAS::Bitmap::Save(const char *file) {
	TRACE("save\n");
	save_bitmap(file, data, NULL);
}


MAS::Bitmap& MAS::Bitmap::Create(int w, int h, Type t) {
	type = t;
	switch (t) {
		case MEMORY:	data = create_bitmap(w, h);			break;
		case SYSTEM:	data = create_system_bitmap(w, h);	break;
		case VIDEO:		data = create_video_bitmap(w, h);	break;
	}
	allocated = true;
	return *this;
}


MAS::Bitmap& MAS::Bitmap::CreateEx(int bpp, int w, int h) {
	type = MEMORY;
	data = create_bitmap_ex(bpp, w, h);
	allocated = true;
	return *this;
}


MAS::Bitmap& MAS::Bitmap::CreateSubBitmap(const Bitmap &parent, int x, int y, int w, int h) {
	data = create_sub_bitmap(parent.data, x, y, w, h);
	type = parent.type;
	allocated = true;
	return *this;
}


void MAS::Bitmap::Destroy() {
	if (data && allocated && !is_screen_bitmap(data)) {
		destroy_bitmap(data);
		data = NULL;
		allocated = false;
	}
}


////////////////////////////////////////////////////////////////////////////////
// Draws a dotted rectangle.
void MAS::Bitmap::DrawDottedRect(int x1, int y1, int x2, int y2, int c) {
	int x, y;
	for (x=x1; x<=x2; x+=2) {
		Putpixel(x, y1, c);
		Putpixel(x, y2, c);
	}
	for(y=y1; y<=y2; y+=2) {
		Putpixel(x1, y, c);
		Putpixel(x2, y, c);
	}
}


////////////////////////////////////////////////////////////////////////////////
// Draws a 3D frame from (x1,y1) to (x2,y2) with colors c2 and c3 and fills it with
// c1. If c1<0, it is considered transparent.
void MAS::Bitmap::Draw3DFrame(int x1, int y1, int x2, int y2, int c1, int c2, int c3) {
	if (c1 >= 0) Rectfill(x1, y1, x2, y2, c1);

	int c4 = makecol(getr(c2) + (getr(c3) - getr(c2))/3, getg(c2) + (getg(c3) - getg(c2))/3, getb(c2) + (getb(c3) - getb(c2))/3);
	int c5 = makecol(getr(c3) - (getr(c3) - getr(c2))/3, getg(c3) - (getg(c3) - getg(c2))/3, getb(c3) - (getb(c3) - getb(c2))/3);

	Vline(x1, y1, y2, c4);
	Hline(x1, y1, x2, c4);
	Vline(x1+1, y1+1, y2-1, c2);
	Hline(x1+1, y1+1, x2-1, c2);
	Vline(x2-1, y1+1, y2, c5);
	Hline(x1+1, y2-1, x2, c5);
	Hline(x1, y2, x2, c3);
	Vline(x2, y1, y2, c3);
}


////////////////////////////////////////////////////////////////////////////////
// Draws a 3D circle on (x,y) wth the radius r
void MAS::Bitmap::Draw3DCircle(int x, int y, int r, int c1, int c2, int c3) {
	if (c1 >= 0) Circlefill(x, y, r, c1);

	int c4 = makecol(getr(c2) + (getr(c3) - getr(c2))/3, getg(c2) + (getg(c3) - getg(c2))/3, getb(c2) + (getb(c3) - getb(c2))/3);
	int c5 = makecol(getr(c3) - (getr(c3) - getr(c2))/3, getg(c3) - (getg(c3) - getg(c2))/3, getb(c3) - (getb(c3) - getb(c2))/3);

	SetClip(0, 0, x + (r>>1), y + (r>>1));
	Circle(x, y, r, c4);
	Circle(x, y, r-1, c2);

	SetClip(x - (r>>1), y - (r>>1), w()-1, h()-1);
	Circle(x, y, r-1, c5);
	Circle(x, y, r, c3);

	SetClip(0, 0, w()-1, h()-1);
}

	
////////////////////////////////////////////////////////////////////////////////
// Draws an X mark with the centre on (x,y), size s and color col
void MAS::Bitmap::DrawXMark(int x, int y, int s, int c) {
	int x1 = x - (s>>1);
	int x2 = x + (s>>1);
	int y1 = y - (s>>1);
	int y2 = y + (s>>1);

	Line(x1, y1, x2, y2, c);
	Line(x1, y1+1, x2-1, y2, c);
	Line(x1+1, y1, x2, y2-1, c);

	Line(x1, y2, x2, y1, c);
	Line(x1, y2-1, x2-1, y1, c);
	Line(x1+1, y2, x2, y1+1, c);
}


////////////////////////////////////////////////////////////////////////////////
// Draws a check mark
void MAS::Bitmap::DrawCheckMark(int x, int y, int c) {
	Line(x, y+2, x+2, y+4, c);
	Line(x, y+3, x+2, y+5, c);
	Line(x, y+4, x+2, y+6, c);
	Line(x+3, y+3, x+6, y, c);
	Line(x+3, y+4, x+6, y+1, c);
	Line(x+3, y+5, x+6, y+2, c);
}

	
////////////////////////////////////////////////////////////////////////////////
// Draws an arrow in the given direction:
//    0 - left
//    1 - right
//    2 - up
//    3 - down
void MAS::Bitmap::DrawArrow(int x, int y, int c, int dir) {
	switch (dir) {
	case 0:
		Putpixel(x, y+3, c);
		Vline(x+1, y+2, y+4, c);
		Vline(x+2, y+1, y+5, c);
		Vline(x+3, y, y+6, c);
		break;
	case 1:
		Vline(x, y, y+6, c);
		Vline(x+1, y+1, y+5, c);
		Vline(x+2, y+2, y+4, c);
		Putpixel(x+3, y+3, c);
		break;
	case 2:
		Putpixel(x+3, y, c);
		Hline(x+2, y+1, x+4, c);
		Hline(x+1, y+2, x+5, c);
		Hline(x, y+3, x+6, c);
		break;
	case 3:
		Hline(x, y, x+6, c);
		Hline(x+1, y+1, x+5, c);
		Hline(x+2, y+2, x+4, c);
		Putpixel(x+3, y+3, c);
		break;
	};
}


////////////////////////////////////////////////////////////////////////////////
// TiledBlit(...) tiles the source bitmap onto the destination bitmap. It
// does so by cutting the source in 9 smaller pieces as shown bellow.
//       1       2       3
//   +-------+-------+-------+  The corner subbitmaps: (1,1), (1,3), (3,1) and (3,3) get
//   |       |       |       |  mapped directly on the destination bitmap and usually
// 1 | (1,1) | (1,2) | (1,3) |  occupy most of the source bitmap.
//   |       |       |       |
//   +-------+-------+-------+  The parts in column 2 are 'thick_x' pixels wide and the parts
// 2 | (2,1) | (2,2) | (2,3) |  in row 2 are 'thick_y' pixels high. Those parts get tiled
//   +-------+-------+-------+  onto the destination bitmap horizontally or vertically.
//   |       |       |       |
// 3 | (3,1) | (3,2) | (3,3) |  The centre tile is tiled across the rest of the destination bitmap.
//   |       |       |       |  
//   +-------+-------+-------+
// The source bitmap can comprise several subbitmaps, which all heva to be of equal dimensions.
// Only one of those subbitmaps is then used for tiling.
// Parameters passed:
//  &dest - the destination bitmap
//  dest_x|y - the starting point in the destination bitmap (def = 0)
//  blit_w|h - the size of the rectangle on destination rectangle (def = dest->w|h)
//  offset_x|y - the horizontal|vertical offset of the subbitmap that is used (def = 0)
//  count_x|y - the number of subbitmaps in the source bitmap (def = 0)
//  thick_x|y - the thickness of the second column|row in the subbitmap (def = 0)
// Important:
//  Because the source bitmap is divided into (count_x * count_y) subbitmaps its
//  width has to be divisable with count_x and the height has to be divisable with
//  count_y (e.g. the standard button bitmap has 1 column of 4 subbitmaps which
//  means that its width is not important while the height has to be a multiple
//  of 4, like 60, 72, 80, etc.). Furthermore, if the thickness of the middle
//  row and column is not specified then 1/3 of the source subbitmap is used
//  (i.e. the subbitmap is divided into 9 equal parts). This means that the source
//  bitmap's height and width have to be divisable with 3*count_x and 3*count_y
//  respectively (e.g. the button bitmap has to have the width that is a multiple
//  of 3 and the height has to be a multiple of 12, like 72, 96, etc.).
//  If this is not true the results may be unexpected.

void blitRow(MAS::Bitmap& src, MAS::Bitmap& dest, int sx, int sy, int dx, int dy, int sw, int sh, int tw, int destw)
{
	src.Blit(dest, sx, sy, dx, dy, sw, sh);
	
	int x2 = destw - sw;
	int mbw = tw;
	for (int cx = sw; cx < x2; cx += tw)
	{
		// check width of piece we're blitting
		if(cx+tw > x2)
			mbw -= (cx+tw) - x2;

		src.Blit(dest, sx + sw, sy, dx + cx, dy, mbw, sh);
	}

	src.Blit(dest, sx + sw + tw, sy, dx + destw - sw, dy, sw, sh);
}

void blitRowMasked(MAS::Bitmap& src, MAS::Bitmap& dest, int sx, int sy, int dx, int dy, int sw, int sh, int tw, int destw)
{
	src.MaskedBlit(dest, sx, sy, dx, dy, sw, sh);
	
	int x2 = destw - sw;
	int mbw = tw;
	for (int cx = sw; cx < x2; cx += tw)
	{
		// check width of piece we're blitting
		if(cx+tw > x2)
			mbw -= (cx+tw) - x2;

		src.MaskedBlit(dest, sx + sw, sy, dx + cx, dy, mbw, sh);
	}

	src.MaskedBlit(dest, sx + sw + tw, sy, dx + destw - sw, dy, sw, sh);
}


// we could easily merge this function with the above one but its complex enough already
void blitCol(MAS::Bitmap& src, MAS::Bitmap& dest, int sx, int sy, int dx, int dy, int sw, int sh, int th, int desth)
{
	src.Blit(dest, sx, sy, dx, dy, sw, sh);
	
	int y2 = desth - sh;
	int mbh = th;
	for (int cy = sh; cy < y2; cy += th)
	{
		// check height of piece we're blitting
		if(cy+th > y2)
			mbh -= (cy+th) - y2;

		src.Blit(dest, sx, sy + sh, dx, dy + cy, sw, mbh);
	}

	src.Blit(dest, sx, sy + sh + th, dx, dy + desth - sh, sw, sh);
}

void blitColMasked(MAS::Bitmap& src, MAS::Bitmap& dest, int sx, int sy, int dx, int dy, int sw, int sh, int th, int desth)
{
	src.MaskedBlit(dest, sx, sy, dx, dy, sw, sh);
	
	int y2 = desth - sh;
	int mbh = th;
	for (int cy = sh; cy < y2; cy += th)
	{
		// check height of piece we're blitting
		if(cy+th > y2)
			mbh -= (cy+th) - y2;

		src.MaskedBlit(dest, sx, sy + sh, dx, dy + cy, sw, mbh);
	}

	src.MaskedBlit(dest, sx, sy + sh + th, dx, dy + desth - sh, sw, sh);
}



void MAS::Bitmap::TiledBlit(
	Bitmap &dst,
	int sectionx, int sectiony,
	int destx, int desty,
	int destw, int desth,
	int countx, int county,
	bool mask)
{
	int srcw = w()/countx;
	int srch = h()/county;
	int srcx = sectionx*srcw;
	int srcy = sectiony*srch;
	int tw = (thick.w() != -1) ? thick.w() : srcw/3;
	int th = (thick.h() != -1) ? thick.h() : srch/3;
	int sw = (srcw - tw)/2;
	int sh = (srch - th)/2;
	
	// first row
	if (mask)
		blitRowMasked(*this, dst, srcx, srcy, destx, desty, sw, sh, tw, destw);
	else
		blitRow(*this, dst, srcx, srcy, destx, desty, sw, sh, tw, destw);

	// centre rows
	int y2 = desth - sh;
	int mbh = th;
	for (int y = sh; y < y2; y += th)
	{
		// check width of piece we're blitting
		if(y+th > y2)
			mbh -= (y+th) - y2;

		if (mask)
			blitRowMasked(*this, dst, srcx, srcy + sh, destx, desty + y, sw, mbh, tw, destw);
		else
			blitRow(*this, dst, srcx, srcy + sh, destx, desty + y, sw, mbh, tw, destw);
	}

	// last row
	if (mask)
		blitRowMasked(*this, dst, srcx, srcy + sh + th, destx, desty + desth - sh, sw, sh, tw, destw);
	else
		blitRow(*this, dst, srcx, srcy + sh + th, destx, desty + desth - sh, sw, sh, tw, destw);
}


void MAS::Bitmap::MaskedTiledBlit(
	Bitmap &dst,
	int sectionx, int sectiony,
	int destx, int desty,
	int destw, int desth,
	int countx, int county)
{
	TiledBlit(dst, sectionx, sectiony, destx, desty, destw, desth, countx, county, true);
}


void MAS::Bitmap::HorizontalTile(Bitmap &dst, int section, int destx, int desty, int destw, int count, bool mask) {
	int srcw = w();
	int srch = h()/count;
	int srcx = 0;
	int srcy = section*srch;
	int tw = (thick.w() != -1) ? thick.w() : srcw/3;
	int sw = (srcw - tw)/2;
	if (mask)
		blitRowMasked(*this, dst, srcx, srcy, destx, desty, sw, srch, tw, destw);
	else
		blitRow(*this, dst, srcx, srcy, destx, desty, sw, srch, tw, destw);
}


void MAS::Bitmap::VerticalTile(Bitmap &dst, int section, int destx, int desty, int desth, int count, bool mask) {
	int srcw = w()/count;
	int srch = h();
	int srcx = section*srcw;
	int srcy = 0;
	int th = (thick.h() != -1) ? thick.h() : srch/3;
	int sh = (srch - th)/2;
	if (mask)
		blitColMasked(*this, dst, srcx, srcy, destx, desty, srcw, sh, th, desth);
	else
		blitCol(*this, dst, srcx, srcy, destx, desty, srcw, sh, th, desth);
}


void MAS::Bitmap::MaskedHorizontalTile(Bitmap &dst, int section, int destx, int desty, int destw, int count) {
	HorizontalTile(dst, section, destx, desty, destw, count, true);
}


void MAS::Bitmap::MaskedVerticalTile(Bitmap &dst, int section, int destx, int desty, int desth, int count) {
	VerticalTile(dst, section, destx, desty, desth, count, true);
}



/* original implementations with subbitmaps (slow!)
void MAS::Bitmap::TiledBlit(
	Bitmap &dst,
	int sectionx, int sectiony,
	int destx, int desty,
	int destw, int desth,
	int countx, int county)
{
	int srcw = data->w/countx;
	int srch = data->h/county;
	BITMAP *src = create_sub_bitmap(data, sectionx*srcw, sectiony*srch, srcw, srch);
	BITMAP *dest = create_sub_bitmap(dst, destx, desty, destw, desth);
	BITMAP *s = NULL;
	if (!src || !dest) return;

	int tw = (thick.w() != -1) ? thick.w() : srcw/3;
	int th = (thick.h() != -1) ? thick.h() : srch/3;
	int sw = (src->w - tw)/2;
	int sh = (src->h - th)/2;
	
	int x, y;
	int x2, y2;
	
	// first row
	x = 0;
	y = 0;
	s = create_sub_bitmap(src, 0, 0, sw, sh);
	if (s) {
		blit(s, dest, 0, 0, 0, 0, sw, sh);
		destroy_bitmap(s);
	}
	x2 = destw - sw;
	s = create_sub_bitmap(src, sw, 0, tw, sh);
	if (s) {
		for (x = sw; x < x2; x += tw) {
			blit(s, dest, 0, 0, x, 0, tw, sh);
		}
		destroy_bitmap(s);
	}
	s = create_sub_bitmap(src, sw + tw, 0, sw, sh);
	if (s) {
		blit(s, dest, 0, 0, destw - sw, 0, sw, sh);
		destroy_bitmap(s);
	}
		
	// centre rows
	BITMAP *s2, *s3;
	y2 = desth - sh;
	s = create_sub_bitmap(src, 0, sh, sw, th);
	s2 = create_sub_bitmap(src, sw, sh, tw, th);
	s3 = create_sub_bitmap(src, sw + tw, sh, sw, th);
	for (y = sh; y < y2; y += th) {
		x = 0;
		if (s) blit(s, dest, 0, 0, 0, y, sw, th);
		if (s2) {
			for (x = sw; x < x2; x += tw) {
				blit(s2, dest, 0, 0, x, y, tw, th);
			}
		}
		if (s3) blit(s3, dest, 0, 0, destw - sw, y, sw, th);
	}
	if (s) destroy_bitmap(s);
	if (s2) destroy_bitmap(s2);
	if (s3) destroy_bitmap(s3);
	
	// last row
	x = 0;
	y = desth - sh;
	s = create_sub_bitmap(src, 0, sh + th, sw, sh);
	if (s) {
		blit(s, dest, 0, 0, 0, y, sw, sh);
		destroy_bitmap(s);
	}
	s = create_sub_bitmap(src, sw, sh + th, tw, sh);
	if (s) {
		for (x = sw; x < x2; x += tw) {
			blit(s, dest, 0, 0, x, y, tw, sh);
		}
		destroy_bitmap(s);
	}
	s = create_sub_bitmap(src, sw + tw, sh + th, sw, sh);
	if (s) {
		blit(s, dest, 0, 0, destw - sw, y, sw, sh);
		destroy_bitmap(s);
	}

	destroy_bitmap(src);
	destroy_bitmap(dest);
}


void MAS::Bitmap::MaskedTiledBlit(
	Bitmap &dst,
	int sectionx, int sectiony,
	int destx, int desty,
	int destw, int desth,
	int countx, int county)
{
	int srcw = data->w/countx;
	int srch = data->h/county;
	BITMAP *src = create_sub_bitmap(data, sectionx*srcw, sectiony*srch, srcw, srch);
	BITMAP *dest = create_sub_bitmap(dst, destx, desty, destw, desth);
	BITMAP *s = NULL;
	if (!src || !dest) return;

	int tw = (thick.w() != -1) ? thick.w() : srcw/3;
	int th = (thick.h() != -1) ? thick.h() : srch/3;
	int sw = (src->w - tw)/2;
	int sh = (src->h - th)/2;
	
	int x, y;
	int x2, y2;
	
	// first row
	x = 0;
	y = 0;
	s = create_sub_bitmap(src, 0, 0, sw, sh);
	if (s) {
		masked_blit(s, dest, 0, 0, 0, 0, sw, sh);
		destroy_bitmap(s);
	}
	x2 = destw - sw;
	s = create_sub_bitmap(src, sw, 0, tw, sh);
	if (s) {
		for (x = sw; x < x2; x += tw) {
			masked_blit(s, dest, 0, 0, x, 0, tw, sh);
		}
		destroy_bitmap(s);
	}
	s = create_sub_bitmap(src, sw + tw, 0, sw, sh);
	if (s) {
		masked_blit(s, dest, 0, 0, destw - sw, 0, sw, sh);
		destroy_bitmap(s);
	}
		
	// centre rows
	BITMAP *s2, *s3;
	y2 = desth - sh;
	s = create_sub_bitmap(src, 0, sh, sw, th);
	s2 = create_sub_bitmap(src, sw, sh, tw, th);
	s3 = create_sub_bitmap(src, sw + tw, sh, sw, th);
	for (y = sh; y < y2; y += th) {
		x = 0;
		if (s) masked_blit(s, dest, 0, 0, 0, y, sw, th);
		if (s2) {
			for (x = sw; x < x2; x += tw) {
				masked_blit(s2, dest, 0, 0, x, y, tw, th);
			}
		}
		if (s3) masked_blit(s3, dest, 0, 0, destw - sw, y, sw, th);
	}
	if (s) destroy_bitmap(s);
	if (s2) destroy_bitmap(s2);
	if (s3) destroy_bitmap(s3);
	
	// last row
	x = 0;
	y = desth - sh;
	s = create_sub_bitmap(src, 0, sh + th, sw, sh);
	if (s) {
		masked_blit(s, dest, 0, 0, 0, y, sw, sh);
		destroy_bitmap(s);
	}
	s = create_sub_bitmap(src, sw, sh + th, tw, sh);
	if (s) {
		for (x = sw; x < x2; x += tw) {
			masked_blit(s, dest, 0, 0, x, y, tw, sh);
		}
		destroy_bitmap(s);
	}
	s = create_sub_bitmap(src, sw + tw, sh + th, sw, sh);
	if (s) {
		masked_blit(s, dest, 0, 0, destw - sw, y, sw, sh);
		destroy_bitmap(s);
	}

	destroy_bitmap(src);
	destroy_bitmap(dest);
}


void MAS::Bitmap::HorizontalTile(Bitmap &dst, int section, int destx, int desty, int destw, int count) {
	int srcw = data->w;
	int srch = data->h/count;
	BITMAP *src = create_sub_bitmap(data, 0, section*srch, srcw, srch);
	BITMAP *dest = create_sub_bitmap(dst, destx, desty, destw, srch);
	BITMAP *s = NULL;
	if (!src || !dest) return;

	int tw = (thick.w() != -1) ? thick.w() : srcw/3;
	int sw = (src->w - tw)/2;
	
	int x = 0;
	s = create_sub_bitmap(src, 0, 0, sw, srch);
	if (s) {
		blit(s, dest, 0, 0, 0, 0, sw, srch);
		destroy_bitmap(s);
	}
	s = create_sub_bitmap(src, sw, 0, tw, srch);
	if (s) {
		int x2 = destw - sw;
		for (x = sw; x < x2; x += tw) {
			blit(s, dest, 0, 0, x, 0, tw, srch);
		}
		destroy_bitmap(s);
	}
	s = create_sub_bitmap(src, sw + tw, 0, sw, srch);
	if (s) {
		blit(s, dest, 0, 0, destw - sw, 0, sw, srch);
		destroy_bitmap(s);
	}
		
	destroy_bitmap(src);
	destroy_bitmap(dest);
}


void MAS::Bitmap::VerticalTile(Bitmap &dst, int section, int destx, int desty, int desth, int count) {
	int srcw = data->w/count;
	int srch = data->h;
	BITMAP *src = create_sub_bitmap(data, section*srcw, 0, srcw, srch);
	BITMAP *dest = create_sub_bitmap(dst, destx, desty, srcw, desth);
	BITMAP *s = NULL;
	if (!src || !dest) return;

	int th = (thick.h() != -1) ? thick.h() : srch/3;
	int sh = (src->h - th)/2;
	
	int y = 0;
	s = create_sub_bitmap(src, 0, 0, srcw, sh);
	if (s) {
		blit(s, dest, 0, 0, 0, 0, srcw, sh);
		destroy_bitmap(s);
	}
	s = create_sub_bitmap(src, 0, sh, srcw, th);
	if (s) {
		int y2 = desth - sh;
		for (y = sh; y < y2; y += th) {
			blit(s, dest, 0, 0, 0, y, srcw, th);
		}
		destroy_bitmap(s);
	}
	s = create_sub_bitmap(src, 0, sh + th, srcw, sh);
	if (s) {
		blit(s, dest, 0, 0, 0, desth - sh, srcw, sh);
		destroy_bitmap(s);
	}
		
	destroy_bitmap(src);
	destroy_bitmap(dest);
}


void MAS::Bitmap::MaskedHorizontalTile(Bitmap &dst, int section, int destx, int desty, int destw, int count) {
	int srcw = data->w;
	int srch = data->h/count;
	BITMAP *src = create_sub_bitmap(data, 0, section*srch, srcw, srch);
	BITMAP *dest = create_sub_bitmap(dst, destx, desty, destw, srch);
	BITMAP *s = NULL;
	if (!src || !dest) return;

	int tw = (thick.w() != -1) ? thick.w() : srcw/3;
	int sw = (src->w - tw)/2;
	
	int x = 0;
	s = create_sub_bitmap(src, 0, 0, sw, srch);
	if (s) {
		masked_blit(s, dest, 0, 0, 0, 0, sw, srch);
		destroy_bitmap(s);
	}
	s = create_sub_bitmap(src, sw, 0, tw, srch);
	if (s) {
		int x2 = destw - sw;
		for (x = sw; x < x2; x += tw) {
			masked_blit(s, dest, 0, 0, x, 0, tw, srch);
		}
		destroy_bitmap(s);
	}
	s = create_sub_bitmap(src, sw + tw, 0, sw, srch);
	if (s) {
		masked_blit(s, dest, 0, 0, destw - sw, 0, sw, srch);
		destroy_bitmap(s);
	}
		
	destroy_bitmap(src);
	destroy_bitmap(dest);
}


void MAS::Bitmap::MaskedVerticalTile(Bitmap &dst, int section, int destx, int desty, int desth, int count) {
	int srcw = data->w/count;
	int srch = data->h;
	BITMAP *src = create_sub_bitmap(data, section*srcw, 0, srcw, srch);
	BITMAP *dest = create_sub_bitmap(dst, destx, desty, srcw, desth);
	BITMAP *s = NULL;
	if (!src || !dest) return;

	int th = (thick.h() != -1) ? thick.h() : srch/3;
	int sh = (src->h - th)/2;
	
	int y = 0;
	s = create_sub_bitmap(src, 0, 0, srcw, sh);
	if (s) {
		masked_blit(s, dest, 0, 0, 0, 0, srcw, sh);
		destroy_bitmap(s);
	}
	s = create_sub_bitmap(src, 0, sh, srcw, th);
	if (s) {
		int y2 = desth - sh;
		for (y = sh; y < y2; y += th) {
			masked_blit(s, dest, 0, 0, 0, y, srcw, th);
		}
		destroy_bitmap(s);
	}
	s = create_sub_bitmap(src, 0, sh + th, srcw, sh);
	if (s) {
		masked_blit(s, dest, 0, 0, 0, desth - sh, srcw, sh);
		destroy_bitmap(s);
	}
		
	destroy_bitmap(src);
	destroy_bitmap(dest);
}
*/


MAS::Bitmap MAS::Bitmap::null;
