/*
 *  $Id$
 *
 *  Copyright (C) 2004, Jon Gettler
 *  http://mvpmc.sourceforge.net/
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MVP_WIDGET_H
#define MVP_WIDGET_H

#include <stdint.h>

#define MVPW_DIR_UP	0
#define MVPW_DIR_DOWN	1
#define MVPW_DIR_LEFT	2
#define MVPW_DIR_RIGHT	3

typedef struct mvp_widget_s mvp_widget_t;

typedef struct {
	int x;
	int y;
	int w;
	int h;
} mvpw_widget_info_t;

typedef struct {
	int rows;
	int cols;
} mvpw_screen_info_t;

extern int mvpw_init(void);
extern int mvpw_event_loop(void);
extern mvp_widget_t *mvpw_get_root(void);
extern void mvpw_set_idle(void (*callback)(void));

extern void mvpw_get_widget_info(mvp_widget_t *widget, mvpw_widget_info_t *info);
extern void mvpw_get_screen_info(mvpw_screen_info_t *info);

extern void mvpw_set_bg(mvp_widget_t *widget, uint32_t bg);
extern uint32_t mvpw_get_bg(mvp_widget_t *widget);
extern void mvpw_set_timer(mvp_widget_t *widget,
			   void (*callback)(mvp_widget_t*), uint32_t timeout);
extern void mvpw_focus(mvp_widget_t *widget);
extern void mvpw_show(mvp_widget_t *widget);
extern void mvpw_hide(mvp_widget_t *widget);
extern void mvpw_raise(mvp_widget_t *widget);
extern void mvpw_lower(mvp_widget_t *widget);
extern void mvpw_expose(mvp_widget_t *widget);
extern int mvpw_font_height(int font);
extern int mvpw_font_width(int font, char *str);
extern int mvpw_load_font(char *file);
extern void mvpw_move(mvp_widget_t *widget, int x, int y);
extern void mvpw_moveto(mvp_widget_t *widget, int x, int y);
extern void mvpw_resize(mvp_widget_t *widget, int w, int h);
extern int mvpw_attach(mvp_widget_t *w1, mvp_widget_t *w2, int direction);
extern void mvpw_unattach(mvp_widget_t *widget, int direction);
extern void mvpw_set_key(mvp_widget_t *widget,
			 void (*callback)(mvp_widget_t*, char));
extern void mvpw_destroy(mvp_widget_t *widget);

/*
 * container widget
 */
extern mvp_widget_t *mvpw_create_container(mvp_widget_t *parent,
					   int x, int y, int w, int h,
					   uint32_t bg, uint32_t border_color,
					   int border_size);

/*
 * text widget
 */
#define MVPW_TEXT_LEFT		0
#define MVPW_TEXT_RIGHT		1
#define MVPW_TEXT_CENTER	2

typedef struct {
	int	 	wrap;
	int	 	justify;
	int	 	margin;
	int	 	font;
	uint32_t 	fg;
} mvpw_text_attr_t;

extern mvp_widget_t *mvpw_create_text(mvp_widget_t *parent,
				      int x, int y, int w, int h,
				      uint32_t bg, uint32_t border_color,
				      int border_size);
extern void mvpw_set_text_str(mvp_widget_t *widget, char *str);
extern void mvpw_set_text_attr(mvp_widget_t *widget, mvpw_text_attr_t *attr);
extern void mvpw_get_text_attr(mvp_widget_t *widget, mvpw_text_attr_t *attr);
extern void mvpw_set_text_fg(mvp_widget_t *widget, uint32_t fg);

/*
 * menu widget
 */
typedef struct {
	int	 	font;
	uint32_t 	fg;
	uint32_t 	hilite_fg;
	uint32_t 	hilite_bg;
	uint32_t 	title_fg;
	uint32_t 	title_bg;
	int	 	title_justify;
	int	 	checkboxes;
} mvpw_menu_attr_t;

typedef struct {
	int	 	  selectable;
	uint32_t 	  fg;
	uint32_t 	  bg;
	uint32_t 	  checkbox_fg;
	void 		(*destroy)(mvp_widget_t*, char*, void*);
	void 		(*select)(mvp_widget_t*, char*, void*);
	void 		(*hilite)(mvp_widget_t*, char*, void*, int);
} mvpw_menu_item_attr_t;

extern mvp_widget_t *mvpw_create_menu(mvp_widget_t *parent,
				      int x, int y, int w, int h,
				      uint32_t bg, uint32_t border_color,
				      int border_size);
extern void mvpw_set_menu_attr(mvp_widget_t *widget, mvpw_menu_attr_t *attr);
extern int mvpw_set_menu_title(mvp_widget_t *widget, char *title);
extern int mvpw_add_menu_item(mvp_widget_t *widget, char *label, void *key,
			      mvpw_menu_item_attr_t *item_attr);
extern void mvpw_clear_menu(mvp_widget_t *widget);
extern int mvpw_delete_menu_item(mvp_widget_t *widget, void *key);
extern void mvpw_check_menu_item(mvp_widget_t *widget, void *key, int checked);
extern int mvpw_menu_hilite_item(mvp_widget_t *widget, void *key);

/*
 * image widget
 */
typedef struct {
	int 		width;
	int 		height;
} mvpw_image_info_t;

extern mvp_widget_t* mvpw_create_image(mvp_widget_t *parent,
				       int x, int y, int w, int h,
				       uint32_t bg, uint32_t border_color,
				       int border_size);
extern int mvpw_set_image(mvp_widget_t *widget, char *file);
extern int mvpw_get_image_info(char *file, mvpw_image_info_t *data);

/*
 * graph widget
 */
typedef struct {
	int 		min;
	int 		max;
	uint32_t 	fg;
} mvpw_graph_attr_t;

extern mvp_widget_t* mvpw_create_graph(mvp_widget_t *parent,
				       int x, int y, int w, int h,
				       uint32_t bg, uint32_t border_color,
				       int border_size);
extern void mvpw_set_graph_attr(mvp_widget_t *widget, mvpw_graph_attr_t *attr);
extern void mvpw_set_graph_current(mvp_widget_t *widget, int value);

/*
 * checkbox widget
 */
extern mvp_widget_t* mvpw_create_checkbox(mvp_widget_t *parent,
					  int x, int y, int w, int h,
					  uint32_t bg, uint32_t border_color,
					  int border_size);
extern void mvpw_set_checkbox_fg(mvp_widget_t *widget, uint32_t fg);
extern void mvpw_set_checkbox(mvp_widget_t *widget, int checked);

/*
 * bitmap widget
 */
typedef struct {
	int 		colors;
	char 		*image;
} mvpw_bitmap_attr_t;

extern mvp_widget_t* mvpw_create_bitmap(mvp_widget_t *parent,
					int x, int y, int w, int h,
					uint32_t bg, uint32_t border_color,
					int border_size);
extern int mvpw_set_bitmap(mvp_widget_t *widget, mvpw_bitmap_attr_t *bitmap);

/*
 * common colors
 */
#define MVPW_TRANSPARENT	MVPW_RGBA(0,0,0,0)
#define MVPW_RED		MVPW_RGBA(255,0,0,255)
#define MVPW_GREEN		MVPW_RGBA(0,255,0,255)
#define MVPW_BLUE		MVPW_RGBA(0,0,255,255)
#define MVPW_WHITE		MVPW_RGBA(255,255,255,255)
#define MVPW_BLACK		MVPW_RGBA(0,0,0,255)
#define MVPW_ORANGE		MVPW_RGBA(255,110,0,255)
#define MVPW_PURPLE		MVPW_RGBA(255,0,255,255)
#define MVPW_LIGHTGREY		MVPW_RGBA(128,128,128,255)
#define MVPW_DARKGREY		MVPW_RGBA(96,96,96,255)
#define MVPW_DARKGREY2		MVPW_RGBA(64,64,64,255)

#define MVPW_RGBA(r,g,b,a)	((a<<24) | (b<<16) | (g<<8) | r)

static inline unsigned long
mvpw_rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return (a<<24) | (b<<16) | (g<<8) | r;
}

#endif /* MVP_WIDGET_H */