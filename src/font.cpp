////////////////////////////////////////////////////////////////////////////////
//         MASkinG - Miran Amon's Skinnable GUI Library for Allegro           //
//    Copyright (c) 2002-2005 Miran Amon (miranamon@users.sourceforge.net)    //
//          Project websites: http://ferisrv5.uni-mb.si/~ma0747               //
//                  http://sourceforge.net/projects/masking                   //
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "../include/MASkinG/font.h"
#include "../include/MASkinG/settings.h"


MAS::Font::Font()
	:type(ALLEGRO_FONT),
	af(font),
	allocated(false)
{
}


MAS::Font::Font(FONT *f)
	:type(ALLEGRO_FONT),
	af(font),
	allocated(false)
{
	Create(f);
}


MAS::Font::Font(ALFONT_FONT *f)
	:type(FREETYPE_FONT),
	af(font),
	allocated(false)
{
	Create(f);
}


MAS::Font::Font(const char *f)
	:type(ALLEGRO_FONT),
	af(font),
	allocated(false)
{
	Load(f);
}


MAS::Font::Font(const Font &f) {
	type = f.type;
	if (type == ALLEGRO_FONT) {
		af = f.af;
#if (ALLEGRO_VERSION >= 4 && ALLEGRO_SUB_VERSION < 1)
			memcpy(&pal, &f.pal, 768);
#else
		if (is_color_font(af)) {
			memcpy(&pal, &f.pal, 768);
		}
#endif
	}
	else {
		ff = f.ff;
	}
	allocated = false;
}


MAS::Font& MAS::Font::operator=(const Font &f) {
	type = f.type;
	if (type == ALLEGRO_FONT) {
		af = f.af;
#if (ALLEGRO_VERSION >= 4 && ALLEGRO_SUB_VERSION < 1)
		memcpy(&pal, &f.pal, 768);
#else
		if (is_color_font(af)) {
			memcpy(&pal, &f.pal, 768);
		}
#endif
	}
	else {
		ff = f.ff;
	}
	allocated = false;
	return *this;
}


MAS::Font::~Font() {
	Destroy();
}


void MAS::Font::Destroy() {
	if (!allocated) return;

	if (type == ALLEGRO_FONT) {
		if (af == font || af == NULL) {
			return;
		}
		else {
			destroy_font(af);
		}
	}
	else /* if (type == FREETYPE_FONT) */ {
		if (ff == NULL) {
			return;
		}
		else {
			alfont_destroy_font(ff);
		}
	}

	allocated = false;
	af = font;
	type = ALLEGRO_FONT;
}


int MAS::Font::SetSize(int h) {
	if (type == ALLEGRO_FONT) {
		return -1;
	}
	else {
		return alfont_set_font_size(ff, h);
	}
}


void MAS::Font::Textout(Bitmap &bmp, const char *s, const Point &p, const Color &c, const Color &bg, int align) const {
	Textout(bmp, s, p.x(), p.y(), c, bg, align);
}


void MAS::Font::Textout(Bitmap &bmp, const char *s, int x, int y, int color, int bg, int align) const {
	if (type == ALLEGRO_FONT) {
#if (ALLEGRO_VERSION >= 4 && ((ALLEGRO_SUB_VERSION == 1 && ALLEGRO_WIP_VERSION >= 18) || ALLEGRO_SUB_VERSION >= 2))
		if (is_color_font(af) && color == -1) {
			set_palette(pal);
			//color = -1;
		}
#endif

#if (ALLEGRO_VERSION >= 4 && ALLEGRO_SUB_VERSION >= 1)
		switch (align) {
			case 0:
				textout_ex(bmp, af, s, x, y, color, bg);
				break;

			case 1:
				textout_right_ex(bmp, af, s, x, y, color, bg);
				break;

			case 2:
				textout_centre_ex(bmp, af, s, x, y, color, bg);
				break;
		};
#else
		text_mode(bg);
		switch (align) {
			case 0:
				textout(bmp, af, s, x, y, color);
				break;

			case 1:
				textout_right(bmp, af, s, x, y, color);
				break;

			case 2:
				textout_centre(bmp, af, s, x, y, color);
				break;
		};
#endif
	}
	else {
		alfont_text_mode(bg);
		if (MAS::Settings::antialiasing && IsScalable()) {
			switch (align) {
				case 0:
					alfont_textout_aa(bmp, ff, s, x, y, color);
					break;

				case 1:
					alfont_textout_right_aa(bmp, ff, s, x, y, color);
					break;

				case 2:
					alfont_textout_centre_aa(bmp, ff, s, x, y, color);
					break;
			};
		}
		else {
			switch (align) {
				case 0:
					alfont_textout(bmp, ff, s, x, y, color);
					break;

				case 1:
					alfont_textout_right(bmp, ff, s, x, y, color);
					break;

				case 2:
					alfont_textout_centre(bmp, ff, s, x, y, color);
					break;
			};
		}
	}
}


void MAS::Font::TextPrintf(Bitmap &bmp, int x, int y, int color, int bg, int align, const char *format, ...) const {
	char buf[512];

	va_list ap;
	va_start(ap, format);
	uvszprintf(buf, sizeof(buf), format, ap);
	va_end(ap);

	Textout(bmp, buf, x, y, color, bg, align);
}


void MAS::Font::TextPrintf(Bitmap &bmp, const Point &p, const Color &color, const Color &bg, int align, const char *format, ...) const {
	char buf[512];

	va_list ap;
	va_start(ap, format);
	uvszprintf(buf, sizeof(buf), format, ap);
	va_end(ap);

	Textout(bmp, buf, p, color, bg, align);
}


int MAS::Font::TextHeight() const {
	if (type == ALLEGRO_FONT) {
		return text_height(af);
	}
	else {
		return alfont_text_height(ff);
	}
}


int MAS::Font::TextLength(const char *str) const {
	if (type == ALLEGRO_FONT) {
		return text_length(af, str);
	}
	else {
		return alfont_text_length(ff, str);
	}
}


bool MAS::Font::IsFixed() const {
	if (type == ALLEGRO_FONT) {
		return false;
	}
	else {
		return alfont_is_fixed_font(ff) ? true : false;
	}
}


bool MAS::Font::IsScalable() const {
	if (type == ALLEGRO_FONT) {
		return false;
	}
	else {
		return alfont_is_scalable_font(ff) ? true : false;
	}
}


const int *MAS::Font::GetAvailableFixedSizes() const {
	if (type == ALLEGRO_FONT) {
		return NULL;
	}
	else {
		return alfont_get_available_fixed_sizes(ff);
	}
}


int MAS::Font::GetNOfAvailableFixedSizes() const {
	if (type == ALLEGRO_FONT) {
		return 0;
	}
	else {
		return alfont_get_nof_available_fixed_sizes(ff);
	}
}


void MAS::Font::SetSpacing(int s) {
	if (type == ALLEGRO_FONT) {
		return;
	}
	else {
		alfont_set_char_extra_spacing(ff, s);
	}
}


int MAS::Font::GetSpacing() const {
	if (type == ALLEGRO_FONT) {
		return 0;
	}
	else {
		return alfont_get_char_extra_spacing(ff);
	}
}


MAS::Font& MAS::Font::Create(FONT *f) {
	Destroy();
	type = ALLEGRO_FONT;
	af = f;
	return *this;
}


MAS::Font& MAS::Font::Create(ALFONT_FONT *f) {
	Destroy();
	type = FREETYPE_FONT;
	ff = f;
	return *this;
}


MAS::Font& MAS::Font::Create() {
	return Create(font);
}


MAS::Font& MAS::Font::operator=(FONT *f) {
	return Create(f);
}


MAS::Font& MAS::Font::operator=(ALFONT_FONT *f) {
	return Create(f);
}


//////////////////////////////////////////////////////////////////////////////
// The following code came from datfont.c, the font plugin for the Grabber
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include <allegro/internal/aintern.h>

#if (ALLEGRO_VERSION == 4 && ALLEGRO_SUB_VERSION <= 1 && ALLEGRO_WIP_VERSION <= 17)

/* creates a new font object */
static void* makenew_font(long* size)
{
    FONT *f;
    FONT_MONO_DATA* mf = 0;
	FONT_MONO_DATA* mfread = (FONT_MONO_DATA *)font->data;

    f = (FONT *)_al_malloc(sizeof(FONT));

    f->height = font->height;
    f->vtable = font->vtable;

    while(mfread) {
        int i;

        if(mf) {
            mf->next = (FONT_MONO_DATA *)_al_malloc(sizeof(FONT_MONO_DATA));
            mf = mf->next;
        } else f->data = mf = (FONT_MONO_DATA *)_al_malloc(sizeof(FONT_MONO_DATA));

        mf->begin = mfread->begin;
        mf->end = mfread->end;
        mf->next = 0;
        mf->glyphs = (FONT_GLYPH **)_al_malloc(sizeof(FONT_GLYPH*) * (mf->end - mf->begin));

        for(i = mf->begin; i < mf->end; i++) {
            FONT_GLYPH *gsrc = mfread->glyphs[i - mf->begin], *gdest;
            int sz = ((gsrc->w + 7) / 8) * gsrc->h;

            gdest = (FONT_GLYPH *)_al_malloc(sizeof(FONT_GLYPH) + sz);
            gdest->w = gsrc->w;
            gdest->h = gsrc->h;
            memcpy(gdest->dat, gsrc->dat, sz);

            mf->glyphs[i - mf->begin] = gdest;
        }

        mfread = mfread->next;
    }

    return f;
}



/* magic number for GRX format font files */
#define FONTMAGIC    0x19590214L



/* import routine for the GRX font format */
static FONT* import_grx_font(AL_CONST char* filename)
{
    PACKFILE *pack;
    FONT *f;
    FONT_MONO_DATA *mf;
    FONT_GLYPH **gl;
    int w, h, num, i;
    int* wtab = 0;

    pack = pack_fopen(filename, F_READ);
    if(!pack) return 0;

    if(pack_igetl(pack) != FONTMAGIC) {
        pack_fclose(pack);
        return 0;
    }
    pack_igetl(pack);

    f = (FONT *)_al_malloc(sizeof(FONT));
    mf = (FONT_MONO_DATA *)_al_malloc(sizeof(FONT_MONO_DATA));

    f->data = mf;
    f->vtable = font_vtable_mono;
    mf->next = 0;

    w = pack_igetw(pack);
    h = pack_igetw(pack);

    f->height = h;

    mf->begin = pack_igetw(pack);
    mf->end = pack_igetw(pack) + 1;
    num = mf->end - mf->begin;

    gl = mf->glyphs = (FONT_GLYPH **)_al_malloc(sizeof(FONT_GLYPH*) * num);

    if(pack_igetw(pack) == 0) {
        for(i = 0; i < 38; i++) pack_getc(pack);
        wtab = (int *)_al_malloc(sizeof(int) * num);
        for(i = 0; i < num; i++) wtab[i] = pack_igetw(pack);
    } else {
        for(i = 0; i < 38; i++) pack_getc(pack);
    }

    for(i = 0; i < num; i++) {
        int sz;

        if(wtab) w = wtab[i];

        sz = ((w + 7) / 8) * h;
        gl[i] = (FONT_GLYPH *)_al_malloc(sizeof(FONT_GLYPH) + sz);
        gl[i]->w = w;
        gl[i]->h = h;

        pack_fread(gl[i]->dat, sz, pack);
    }

    pack_fclose(pack);
    if(wtab) free(wtab);

    return f;
}



/* import routine for the 8x8 and 8x16 BIOS font formats */
static FONT* import_bios_font(AL_CONST char* filename)
{
    PACKFILE *pack;
    FONT *f;
    FONT_MONO_DATA *mf;
    FONT_GLYPH **gl;
    int i, h;

    f = (FONT *)_al_malloc(sizeof(FONT));
    mf = (FONT_MONO_DATA *)_al_malloc(sizeof(FONT_MONO_DATA));
    gl = (FONT_GLYPH **)_al_malloc(sizeof(FONT_GLYPH*) * 256);

    pack = pack_fopen(filename, F_READ);
    if(!pack) return 0;

    h = (pack->todo == 2048) ? 8 : 16;

    for(i = 0; i < 256; i++) {
        gl[i] = (FONT_GLYPH *)_al_malloc(sizeof(FONT_GLYPH) + 8);
        gl[i]->w = gl[i]->h = 8;
        pack_fread(gl[i]->dat, 8, pack);
    }

    f->vtable = font_vtable_mono;
    f->data = mf;
    f->height = h;

    mf->begin = 0;
    mf->end = 256;
    mf->glyphs = gl;
    mf->next = 0;

    pack_fclose(pack);

    return f;
}


/* state information for the bitmap font importer */
static BITMAP *import_bmp = NULL;

static int import_x = 0;
static int import_y = 0;


/* splits bitmaps into sub-sprites, using regions bounded by col #255 */
void datedit_find_character(BITMAP *bmp, int *x, int *y, int *w, int *h)
{
   int c1;
   int c2;

   if (bitmap_color_depth(bmp) == 8) {
      c1 = 255;
      c2 = 255;
   }
   else {
      c1 = makecol_depth(bitmap_color_depth(bmp), 255, 255, 0);
      c2 = makecol_depth(bitmap_color_depth(bmp), 0, 255, 255);
   }

   /* look for top left corner of character */
   while ((getpixel(bmp, *x, *y) != c1) ||
	  (getpixel(bmp, *x+1, *y) != c2) ||
	  (getpixel(bmp, *x, *y+1) != c2) ||
	  (getpixel(bmp, *x+1, *y+1) == c1) ||
	  (getpixel(bmp, *x+1, *y+1) == c2)) {
      (*x)++;
      if (*x >= bmp->w) {
	 *x = 0;
	 (*y)++;
	 if (*y >= bmp->h) {
	    *w = 0;
	    *h = 0;
	    return;
	 }
      }
   }

   /* look for right edge of character */
   *w = 0;
   while ((getpixel(bmp, *x+*w+1, *y) == c2) &&
	  (getpixel(bmp, *x+*w+1, *y+1) != c2) &&
	  (*x+*w+1 <= bmp->w))
      (*w)++;

   /* look for bottom edge of character */
   *h = 0;
   while ((getpixel(bmp, *x, *y+*h+1) == c2) &&
	  (getpixel(bmp, *x+1, *y+*h+1) != c2) &&
	  (*y+*h+1 <= bmp->h))
      (*h)++;
}



/* import_bitmap_font_mono:
 *  Helper for import_bitmap_font, below.
 */
static int import_bitmap_font_mono(FONT_GLYPH** gl, int num)
{
    int w = 1, h = 1, i;

    for(i = 0; i < num; i++) {
        if(w > 0 && h > 0) datedit_find_character(import_bmp, &import_x, &import_y, &w, &h);
        if(w <= 0 || h <= 0) {
            int j;

            gl[i] = (FONT_GLYPH *)_al_malloc(sizeof(FONT_GLYPH) + 8);
            gl[i]->w = 8;
            gl[i]->h = 8;

            for(j = 0; j < 8; j++) gl[i]->dat[j] = 0;
        } else {
            int sx = ((w + 7) / 8), j, k;

            gl[i] = (FONT_GLYPH *)_al_malloc(sizeof(FONT_GLYPH) + sx * h);
            gl[i]->w = w;
            gl[i]->h = h;

            for(j = 0; j < sx * h; j++) gl[i]->dat[j] = 0;
            for(j = 0; j < h; j++) {
                for(k = 0; k < w; k++) {
                    if(getpixel(import_bmp, import_x + k + 1, import_y + j + 1))
                        gl[i]->dat[(j * sx) + (k / 8)] |= 0x80 >> (k & 7);
                }
            }

            import_x += w;
        }
    }

    return 0;
}



/* import_bitmap_font_color:
 *  Helper for import_bitmap_font, below.
 */
static int import_bitmap_font_color(BITMAP** bits, int num)
{
    int w = 1, h = 1, i;

    for(i = 0; i < num; i++) {
        if(w > 0 && h > 0) datedit_find_character(import_bmp, &import_x, &import_y, &w, &h);
        if(w <= 0 || h <= 0) {
            bits[i] = create_bitmap_ex(8, 8, 8);
            if(!bits[i]) return -1;
            clear_to_color(bits[i], 255);
        } else {
            bits[i] = create_bitmap_ex(8, w, h);
            if(!bits[i]) return -1;
            blit(import_bmp, bits[i], import_x + 1, import_y + 1, 0, 0, w, h);
            import_x += w;
        }
    }

    return 0;
}



/* bitmap_font_ismono:
 *  Helper for import_bitmap_font, below.
 */
static int bitmap_font_ismono(BITMAP *bmp)
{
    int x, y, col = -1, pixel;

    for(y = 0; y < bmp->h; y++) {
        for(x = 0; x < bmp->w; x++) {
            pixel = getpixel(bmp, x, y);
            if(pixel == 0 || pixel == 255) continue;
            if(col > 0 && pixel != col) return 0;
            col = pixel;
        }
    }

    return 1;
}



/* upgrade_to_color, upgrade_to_color_data:
 *  Helper functions. Upgrades a monochrome font to a color font.
 */
static FONT_COLOR_DATA* upgrade_to_color_data(FONT_MONO_DATA* mf)
{
    FONT_COLOR_DATA* cf = (FONT_COLOR_DATA *)_al_malloc(sizeof(FONT_COLOR_DATA));
    BITMAP** bits = (BITMAP **)_al_malloc(sizeof(BITMAP*) * (mf->end - mf->begin));
    int i;

    cf->begin = mf->begin;
    cf->end = mf->end;
    cf->bitmaps = bits;
    cf->next = 0;

    for(i = mf->begin; i < mf->end; i++) {
        FONT_GLYPH* g = mf->glyphs[i - mf->begin];
        BITMAP* b = create_bitmap_ex(8, g->w, g->h);
        clear_to_color(b, 0);
#if ALLEGRO_SUB_VERSION < 1
    text_mode(-1);
	b->vtable->draw_glyph(b, g, 0, 0, 1);
#else
	b->vtable->draw_glyph(b, g, 0, 0, 1, 0);
#endif


        bits[i - mf->begin] = b;
        free(g);
    }

    free(mf->glyphs);
    free(mf);

    return cf;
}



static void upgrade_to_color(FONT* f)
{
    FONT_MONO_DATA* mf = (FONT_MONO_DATA *)f->data;
    FONT_COLOR_DATA * cf, *cf_write = 0;

    if(f->vtable == font_vtable_color) return;
    f->vtable = font_vtable_color;

    while(mf) {
        FONT_MONO_DATA* mf_next = mf->next;

        cf = upgrade_to_color_data(mf);
        if(!cf_write) f->data = cf;
        else cf_write->next = cf;

        cf_write = cf;
        mf = mf_next;
    }
}



/* bitmap_font_count:
 *  Helper for `import_bitmap_font', below.
 */
static int bitmap_font_count(BITMAP* bmp)
{
    int x = 0, y = 0, w = 0, h = 0;
    int num = 0;

    while(1) {
        datedit_find_character(bmp, &x, &y, &w, &h);
        if (w <= 0 || h <= 0) break;
        num++;
        x += w;
    }

    return num;
}



/* import routine for the Allegro .pcx font format */
static FONT* import_bitmap_font(AL_CONST char* fname, int begin, int end, int cleanup)
{
    /* NB: `end' is -1 if we want every glyph */
    FONT *f;

    if(fname) {
		set_color_conversion(COLORCONV_NONE);
        PALETTE junk;

        if(import_bmp) destroy_bitmap(import_bmp);
        import_bmp = load_bitmap(fname, junk);

        import_x = 0;
        import_y = 0;
    }

    if(!import_bmp) return 0;

    if(bitmap_color_depth(import_bmp) != 8) {
        destroy_bitmap(import_bmp);
        import_bmp = 0;
        return 0;
    }

    f = (FONT *)_al_malloc(sizeof(FONT));
    if(end == -1) end = bitmap_font_count(import_bmp) + begin;

    if (bitmap_font_ismono(import_bmp)) {

        FONT_MONO_DATA* mf = (FONT_MONO_DATA *)_al_malloc(sizeof(FONT_MONO_DATA));

        mf->glyphs = (FONT_GLYPH **)_al_malloc(sizeof(FONT_GLYPH*) * (end - begin));

        if( import_bitmap_font_mono(mf->glyphs, end - begin) ) {

            free(mf->glyphs);
            free(mf);
            free(f);
            f = 0;

        } else {

            f->data = mf;
            f->vtable = font_vtable_mono;
            f->height = mf->glyphs[0]->h;

            mf->begin = begin;
            mf->end = end;
            mf->next = 0;
        }

    } else {

        FONT_COLOR_DATA* cf = (FONT_COLOR_DATA *)_al_malloc(sizeof(FONT_COLOR_DATA));
        cf->bitmaps = (BITMAP **)_al_malloc(sizeof(BITMAP*) * (end - begin));

        if( import_bitmap_font_color(cf->bitmaps, end - begin) ) {

            free(cf->bitmaps);
            free(cf);
            free(f);
            f = 0;

        } else {

            f->data = cf;
            f->vtable = font_vtable_color;
            f->height = cf->bitmaps[0]->h;

            cf->begin = begin;
            cf->end = end;
            cf->next = 0;

        }

    }

    if(cleanup) {
        destroy_bitmap(import_bmp);
        import_bmp = 0;
    }

    return f;
}



/* import routine for the multiple range .txt font format */
static FONT* import_scripted_font(AL_CONST char* filename)
{
    char buf[1024], *bmp_str, *start_str = 0, *end_str = 0;
    FONT *f, *f2;
    PACKFILE *pack;
    int begin, end;

    pack = pack_fopen(filename, F_READ);
    if(!pack) return 0;

    f = (FONT *)_al_malloc(sizeof(FONT));
    f->data = NULL;
    f->height = 0;
    f->vtable = NULL;

    while(pack_fgets(buf, sizeof(buf)-1, pack)) {
        bmp_str = strtok(buf, " \t");
        if(bmp_str) start_str = strtok(0, " \t");
        if(start_str) end_str = strtok(0, " \t");

        if(!bmp_str || !start_str || !end_str) {
            _al_free(f);
            pack_fclose(pack);

            return 0;
        }

        if(bmp_str[0] == '-') bmp_str = 0;
        begin = strtol(start_str, 0, 0);
        if(end_str) end = strtol(end_str, 0, 0) + 1;
        else end = -1;

        if(begin <= 0 || (end > 0 && end < begin)) {
            _al_free(f);
            pack_fclose(pack);

            return 0;
        }

        f2 = import_bitmap_font(bmp_str, begin, end, FALSE);
        if(!f2) {
            _al_free(f);
            pack_fclose(pack);

            return 0;
        }

        if(!f->vtable) f->vtable = f2->vtable;
        if(!f->height) f->height = f2->height;

        if(f2->vtable != f->vtable) {
            upgrade_to_color(f);
            upgrade_to_color(f2);
        }

        /* add to end of linked list */

        if(f->vtable == font_vtable_mono) {
            FONT_MONO_DATA* mf = (FONT_MONO_DATA *)f->data;
            if(!mf) f->data = f2->data;
            else {
                while(mf->next) mf = mf->next;
                mf->next = (FONT_MONO_DATA *)f2->data;
            }
            free(f2);
        } else {
            FONT_COLOR_DATA* cf = (FONT_COLOR_DATA *)f->data;
            if(!cf) f->data = f2->data;
            else {
                while(cf->next) cf = cf->next;
                cf->next = (FONT_COLOR_DATA *)f2->data;
            }
            free(f2);
        }
    }

    destroy_bitmap(import_bmp);
    import_bmp = 0;

    pack_fclose(pack);
    return f;
}

#endif	//Alegro version <= 4.1.17


/* import routine for the Impulse Tracker fonts (8x8 only) */
static FONT* import_it_font(AL_CONST char* filename)
{
    PACKFILE *pack;
    FONT *f;
    FONT_MONO_DATA *mf;
    FONT_GLYPH **gl;
    int i, h;

    f = (FONT *)_al_malloc(sizeof(FONT));
    mf = (FONT_MONO_DATA *)_al_malloc(sizeof(FONT_MONO_DATA));
    gl = (FONT_GLYPH **)_al_malloc(sizeof(FONT_GLYPH*) * 256);

    pack = pack_fopen(filename, F_READ);
    if(!pack) return 0;

    h = 8;

    for(i = 0; i < 256; i++) {
        gl[i] = (FONT_GLYPH *)_al_malloc(sizeof(FONT_GLYPH) + 8);
        gl[i]->w = gl[i]->h = 8;
        pack_fread(gl[i]->dat, 8, pack);
    }

    f->vtable = font_vtable_mono;
    f->data = mf;
    f->height = h;

    mf->begin = 0;
    mf->end = 256;
    mf->glyphs = gl;
    mf->next = 0;

    pack_fclose(pack);

    return f;
}


MAS::Error MAS::Font::Load(const char *file) {
	// find the file
	char filename[2048];
	char tmp[2048];

	char *path = NULL;
	if (!file_exists(file, FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH, NULL)) {
		path = getenv("WINDIR");
		if (!path) {
			path = getenv("windir");
		}
		if (!path) {
			return Error(Error::NO_FILE);
		}
		ustrcpy(filename, uconvert_ascii(path, tmp));
		ustrcat(filename, uconvert_ascii("/fonts/", tmp));
		append_filename(filename, filename, get_filename(file), 2048);
		fix_filename_slashes(filename);
		if (!file_exists(filename, FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH, NULL)) {
			return Error(Error::NO_FILE);
		}
	}
	else {
		ustrcpy(filename, file);
	}

	// possibly destroy the old font
	Destroy();

#if (ALLEGRO_VERSION == 4 && ALLEGRO_SUB_VERSION <= 1 && ALLEGRO_WIP_VERSION <= 17)
	/*
	This was commented out because loading bitmap fonts like that won't work
	if formats other thant the ones Allegro natively supports are supported
	through an add-on library (e.g. loadpng)
	if (ustricmp(get_extension(filename), uconvert_ascii("bmp", tmp)) == 0 ||
		ustricmp(get_extension(filename), uconvert_ascii("pcx", tmp)) == 0 ||
		ustricmp(get_extension(filename), uconvert_ascii("lbm", tmp)) == 0 ||
		ustricmp(get_extension(filename), uconvert_ascii("tga", tmp)) == 0)
	{
		// load a bitmap font
		af = import_bitmap_font(filename, ' ', -1, TRUE);
		allocated = true;
		set_color_conversion(COLORCONV_TOTAL);
	}
	*/
	// try loading the font as a bitmap font and only if that doesn't work, try
	// something else
	af = import_bitmap_font(filename, ' ', -1, TRUE);
	if (af) {
		allocated = true;
		set_color_conversion(COLORCONV_TOTAL);
	}
	// get the font format (bitmap, grx, freetype) and load the file
	else if (ustricmp(get_extension(filename), uconvert_ascii("txt", tmp)) == 0) {
		// load a scripted font
		af = import_scripted_font(filename);
		allocated = true;
	}
	else if (ustricmp(get_extension(filename), uconvert_ascii("itf", tmp)) == 0) {
		// load an Impulse Tracker font
		af = import_it_font(filename);
		allocated = true;
	}
	else if (ustricmp(get_extension(filename), uconvert_ascii("fnt", tmp)) == 0) {
		// load a grx font or a bios font
		PACKFILE *f = pack_fopen(filename, F_READ);
		if (f) {
			int id = pack_igetl(f);
			pack_fclose(f);
			if (id == FONTMAGIC) {
				af = import_grx_font(filename);
				allocated = true;
			}
			else {
				af = import_bios_font(filename);
				allocated = true;
			}
		}
	}
	else {
		// other type of font --> use alfont
		type = FREETYPE_FONT;
		char tmp2[2048];
		ff = alfont_load_font(uconvert_toascii(filename, tmp2));
		allocated = true;
	}
#else
	// get the font format (bitmap, grx, freetype) and load the file
	if (ustricmp(get_extension(filename), uconvert_ascii("bmp", tmp)) == 0 ||
		ustricmp(get_extension(filename), uconvert_ascii("pcx", tmp)) == 0 ||
		ustricmp(get_extension(filename), uconvert_ascii("lbm", tmp)) == 0 ||
		ustricmp(get_extension(filename), uconvert_ascii("tga", tmp)) == 0 ||
		ustricmp(get_extension(filename), uconvert_ascii("txt", tmp)) == 0 ||
		ustricmp(get_extension(filename), uconvert_ascii("fnt", tmp)) == 0)
	{
		// load an Allegro native font
		af = load_font(filename, pal, NULL);
		allocated = true;
	}
	else if (ustricmp(get_extension(filename), uconvert_ascii("itf", tmp)) == 0) {
		// load an Impulse Tracker font
		af = import_it_font(filename);
		allocated = true;
	}
	else {
		// other type of font --> use alfont
		type = FREETYPE_FONT;
		char tmp2[2048];
		ff = alfont_load_font(uconvert_toascii(filename, tmp2));
		allocated = true;
	}
#endif

	// if the font wasn't loaded set the default font
	if ((type == FREETYPE_FONT && !ff) || (type == ALLEGRO_FONT && !af)) {
		allocated = false;
		type = ALLEGRO_FONT;
		af = font;
		return Error(Error::LOAD_FILE);
	}

	return Error(Error::NONE);
}


////////////////////////////////////////////////////////////////////////////////
// GUITextout:
//	Wrapper function for drawing text to the screen, which interprets the
//	& character as an underbar for displaying keyboard shortcuts. Returns
//	the width of the output string in pixels.
int MAS::Font::GUITextoutEx(Bitmap &bmp, const char *s, int x, int y, int col1, int col2, int bg, int align) const {
	char tmp[1024];
	int hline_pos = -1;
	int len = 0;
	int in_pos = 0;
	int out_pos = 0;
	int pix_len, c;

	while (((c = ugetc(s+in_pos)) != 0) && (out_pos<(int)(sizeof(tmp)-ucwidth(0)))) {
		if (c == '&') {
			in_pos += uwidth(s+in_pos);
			c = ugetc(s+in_pos);
			if (c == '&') {
				out_pos += usetc(tmp+out_pos, '&');
				in_pos += uwidth(s+in_pos);
				len++;
			}
			else
				hline_pos = len;
		}
		else {
			out_pos += usetc(tmp+out_pos, c);
			in_pos += uwidth(s+in_pos);
			len++;
		}
	}

	usetc(tmp+out_pos, 0);
	pix_len = TextLength(tmp);

	if (align == 1) {
		x -= pix_len;
	}
	else if (align == 2) {
		x -= pix_len/2;
	}

	if (bmp) {
		if (col2 != -1) {
			Textout(bmp, tmp, x+1, y+1, col2, bg, 0);
			Textout(bmp, tmp, x, y, col1, -1, 0);
		}
		else {
			Textout(bmp, tmp, x, y, col1, bg, 0);
		}

		if (hline_pos >= 0) {
			c = ugetat(tmp, hline_pos);
			usetat(tmp, hline_pos, 0);
			hline_pos = TextLength(tmp);
			c = usetc(tmp, c);
			usetc(tmp+c, 0);
			c = TextLength(tmp);
			bmp.Hline(x+hline_pos, y+TextHeight(), x+hline_pos+c-1, col1);
		}
	}

	return pix_len;
}


////////////////////////////////////////////////////////////////////////////////
// Returns the length of a string in pixels, ignoring '&' characters.
int MAS::Font::GUITextLength(const char *s) const {
	Bitmap null;
	return GUITextoutEx(null, s, 0, 0, 0, 0, -1, 0);
}

int MAS::Font::GUITextoutEx(Bitmap &bmp, const char *text, const Point &p, const Color &col1, const Color &col2, const Color &bg, int align) const {
	return GUITextoutEx(bmp, text, p.x(), p.y(), col1, col2, bg, align);
}


void MAS::Font::GUITextout(Bitmap &bmp, const char *text, int x, int y, int col1, int col2, int bg, int align) const {
	if (col2 != -1) {
		Textout(bmp, text, x+1, y+1, col2, bg, align);
		Textout(bmp, text, x, y, col1, -1, align);
	}
	else {
		Textout(bmp, text, x, y, col1, bg, align);
	}
}

void MAS::Font::GUITextout(Bitmap &bmp, const char *text, const Point &p, const Color &col1, const Color &col2, const Color &bg, int align) const {
	GUITextout(bmp, text, p.x(), p.y(), col1, col2, bg, align);
}


/**
  returns number of characters of the string text that fit on this line, while performing word-wrapping.
  Maxwidth is the width of this line.  Preferredwidth is the actual width of that number of characters,
  so you can calculate the remaining space by substracting preferredwidth from maxwidth and align the text as you wish.

  treats \n characters as hard (=forced) line breaks, so even if more characters fit on a line it always stops at a \n.
*/
int MAS::Font::GetWrapPos(const char *text, int maxwidth, int &preferredwidth) const
{
    int ww = 0; // word width in pixels
    int wl = 0; // number of chars in word
    int lw = 0; // line width, excluding current word
    int ll = 0; // line length

    const char *pos = text;
    char temp[2] = {0, 0}; // used to calculate length of a single character
    bool quit = false;

    while (!quit)
    {
        if (*pos == '\0')
        {
            if (lw + ww <= maxwidth || lw == 0)
            {
                lw += ww;
                ll += wl; // don't count 0 itself
            }
            quit = true;
        } else if (*pos == '\n') // newline
        {
            if (lw + ww <= maxwidth || lw == 0)
            {
                lw += ww;
                ll += wl + 1; // do count '\n' itself
            }
            quit = true;
        } else if (*pos <= 32) // some kind of space, not newline or \0
        {
            // end of word
            if (lw + ww <= maxwidth || lw == 0)
            {
                lw += ww;
                ll += wl;
                temp[0] = *pos;
                ww = GUITextLength (temp);
                wl = 1;
                pos++;
            }
            else
            {
                quit = true;
            }
        } else
        {
            temp[0] = *pos;
            ww += GUITextLength (temp);
            wl++;
            pos++;
        }
    }
    preferredwidth = lw;
    return ll;
}

/**
   simple wrapper for when you don't need the preferredwidth parameter
*/
int MAS::Font::GetWrapPos(const char *text, int maxwidth) const
{
    int temp;
    return GetWrapPos (text, maxwidth, temp);
}

/**
  returns number of whitespace at the start of text. Doesn't count \0 or \n as whitespace.
*/
int MAS::Font::SkipWhiteSpace (const char *text) const
{
    int result = 0;
    const char *pos = text;
    while ((*pos <= 32) && (*pos > 0) && (*pos != '\n')) { pos++; result++; }
    return result;
}

/**
  counts number of lines needed to print text in an area with width = maxwidth.
  empty string is 0 lines. Only whitespace is 1 line.

  usefull for calculating the height of a textbox
*/
int MAS::Font::CountLines (const char *text, int maxwidth) const
{
    const char *pos = text;
    int result = 0;
    while (*pos != 0)
    {
        result++; // count a line
        pos += SkipWhiteSpace (pos);
        if (*pos != 0)
        {
            pos += GetWrapPos (pos, maxwidth);
        }
    }
    return result;
}

/**
  prints text word-wrapped on a column with a specified width but unspecified height.
  note: hal = horizontal alignment
  performs horizontal alignment but not vertical.
*/
void MAS::Font::ColumnPrint (MAS::Bitmap &bmp, const char *text, int fg, int bg, int textMode, int x, int y, int w, int hal) const
{
    char *pos = (char *)text;	// ugly hack because this function needs to temporarily change some of the text
    int xco, yco = y;
    switch (hal)
    {
        case 0: xco = x;                break;
        case 1:	xco = x + w;	        break;
        case 2:
	default:	xco = x + (w / 2);	break;
    };

    while (*pos != 0)
    {
        pos += SkipWhiteSpace (pos);
        if (*pos != 0)
        {
            char temp;
            int n = GetWrapPos (pos, w);
            int n2 = n;
            if (pos[n-1] == '\n') n2 = n-1; // little hack to skip trailing newlines
            temp = pos[n2];
            pos[n2] = 0;
            GUITextout(bmp, pos, xco, yco, fg, bg, textMode, hal);
            pos[n2] = temp;
            pos += n;
            yco += TextHeight();
        }
    }
}

/**
   prints text word-wrapped in a box with specified width and height. Performs horizontal and vertical alignment.
*/
void MAS::Font::BoxPrint (MAS::Bitmap &bmp, const char *text, int fg, int bg, int textMode, int x, int y, int w, int h, int hal, int val) const
{
    int minh = TextHeight() * CountLines (text, w);
    int yco;
    switch (val)
    {
        case 0: yco = 0;                break;
        case 1: yco = y + h - minh;     break;
        case 2:
	default: yco = y + (h - minh) / 2;       break;
    }
    ColumnPrint (bmp, text, fg, bg, textMode, x, yco, w, hal);
}
