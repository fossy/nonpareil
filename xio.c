/*
CSIM is a simulator for the processor used in the HP "Classic" series
of calculators, which includes the HP-35, HP-45, HP-55, HP-65, HP-70,
and HP-80.

$Id$
Copyright 1995, 2003 Eric L. Smith

CSIM is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License version 2 as published by the Free
Software Foundation.  Note that I am not granting permission to redistribute
or modify CSIM under the terms of any later version of the General Public
License.

This program is distributed in the hope that it will be useful (or at least
amusing), but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with
this program (in the file "COPYING"); if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "xio.h"


Display *mydisplay;
int myscreen;
Window mywindow;
GC mygc;
XFontStruct *myfont;

/* pixel values */
unsigned long black, white;

int window_width, window_height;

char disp_buf [20];

int pressed_key = -1;


#define MAX_KEY 256
int *keymap [MAX_KEY];
int escape_code;


#define WINDOW_WIDTH  270
#define WINDOW_HEIGHT 504

#define DISPLAY_HEIGHT 72

XRectangle display_rect = { 0, 0, WINDOW_WIDTH, DISPLAY_HEIGHT };


#define BLACK    0
#define WHITE    1
#define LT_GREY  2
#define MED_GREY 3
#define DK_GREY  4
#define GOLD     5
#define BLUE     6
#define DK_RED   7
#define BR_RED   8

#define MAX_COL 9


unsigned long pixval [MAX_COL];

typedef struct
{
  unsigned short red;
  unsigned short green;
  unsigned short blue;
} rgb;

#if 1 
rgb color [MAX_COL] =
{
  { 0x0000, 0x0000, 0x0000 },  
  { 0xffff, 0xffff, 0xffff },  
  { 0xa000, 0xa000, 0xa000 },  
  { 0x5800, 0x5800, 0x5800 },  
  { 0x3800, 0x3800, 0x3800 },  
  { 0xffff, 0xd7d7, 0x0000 },  
  { 0x4000, 0x4000, 0xffff },  
  { 0x5000, 0x0000, 0x0000 },  
  { 0xffff, 0x4000, 0x0000 }   
};

static void init_colors (void)
{
  int i;
  XColor col;
  Colormap cmap;

  cmap = DefaultColormap (mydisplay, myscreen);

  for (i = 0; i < MAX_COL; i++)
    {
      col.red   = color [i].red;
      col.green = color [i].green;
      col.blue  = color [i].blue;
      if (XAllocColor (mydisplay, cmap, & col) == 0)
	{
	  fprintf (stderr, "can't get color\n");
	  exit (2);
	}
      pixval [i] = col.pixel;
    }
}
#else
char *color_name [MAX_COL] =
{
  "black", 
  "white", 
  "grey63",
  "grey38",
  "grey22",
  "gold",
  "blue",
  "darkred",  /* alas, not nearly dark enough */
  "red"
};

static void init_colors (void)
{
  int i;
  XColor col, exact;
  Colormap cmap;

  cmap = DefaultColormap (mydisplay, myscreen);

  for (i = 0; i < MAX_COL; i++)
    {
      if (! XAllocNamedColor (mydisplay, cmap, color_name [i], &exact, & col))
	{
	  fprintf (stderr, "can't get color\n");
	  exit (2);
	}
      pixval [i] = col.pixel;
    }
}
#endif


typedef struct
{
  XRectangle rect;
  char *label;
  char *flabel;
  int keycode;
  int fg, bg;
} keyinfo;


keyinfo (*keys)[35];

keyinfo keys_hp35 [35] =
{
  { {  24, 120, 30, 24 }, "x^y",   "",     006, WHITE, BLACK },
  { {  72, 120, 30, 24 }, "log",    "",     004, WHITE, BLACK },
  { { 120, 120, 30, 24 }, "ln",   "",    003, WHITE, BLACK },
  { { 168, 120, 30, 24 }, "e^x",   "",     002, WHITE, BLACK },
  { { 216, 120, 30, 24 }, "CLR",      "",        000, WHITE,  BLUE },

  { {  24, 168, 30, 24 }, "sqrt(x)",   "", 056, WHITE, BLACK },
  { {  72, 168, 30, 24 }, "arc",   "",     054, WHITE, MED_GREY },
  { { 120, 168, 30, 24 }, "sin",   "",  053, WHITE, MED_GREY },
  { { 168, 168, 30, 24 }, "cos",   "",  052, WHITE, MED_GREY },
  { { 216, 168, 30, 24 }, "tan",   "",  050, WHITE, MED_GREY },

  { {  24, 216, 30, 24 }, "1/x",  "",      016, WHITE, BLACK },
  { {  72, 216, 30, 24 }, "x<>y",   "",     014, WHITE, BLACK },
  { { 120, 216, 30, 24 }, "RDN",   "",  013, WHITE, BLACK },
  { { 168, 216, 30, 24 }, "STO",   "",  012, WHITE, BLACK },
  { { 216, 216, 30, 24 }, "RCL",     "", 010, WHITE, BLACK },

  { {  24, 264, 78, 24 }, "ENTER^","", 076, WHITE, BLUE },
  { { 120, 264, 30, 24 }, "CHS",   "",     073, WHITE, BLUE },
  { { 168, 264, 30, 24 }, "EEX",   "",     072, WHITE, BLUE },
  { { 216, 264, 30, 24 }, "CLX",   "",   070, WHITE, BLUE },

  { {  24, 312, 24, 24 }, "-",     "",        066, WHITE, BLUE },
  { {  73, 312, 37, 24 }, "7",     "",   064, BLACK, WHITE },
  { { 141, 312, 37, 24 }, "8",     "",   063, BLACK, WHITE },
  { { 209, 312, 37, 24 }, "9",     "", 062, BLACK, WHITE },

  { {  24, 360, 24, 24 }, "+",     "",        026, WHITE, BLUE },
  { {  73, 360, 37, 24 }, "4",     "",        024, BLACK, WHITE },
  { { 141, 360, 37, 24 }, "5",     "",        023, BLACK, WHITE },
  { { 209, 360, 37, 24 }, "6",     "",        022, BLACK, WHITE },

  { {  24, 408, 24, 24 }, "x",     "",        036, WHITE, BLUE },
  { {  73, 408, 37, 24 }, "1",     "",        034, BLACK, WHITE },
  { { 141, 408, 37, 24 }, "2",     "",        033, BLACK, WHITE },
  { { 209, 408, 37, 24 }, "3",     "",        032, BLACK, WHITE },

  { {  24, 456, 24, 24 }, "/",     "",        046, WHITE, BLUE },
  { {  73, 456, 37, 24 }, "0",     "",   044, BLACK, WHITE },
  { { 141, 456, 37, 24 }, ".",     "",      043, BLACK, WHITE },
  { { 209, 456, 37, 24 }, "Pi",  "",    042, BLACK, WHITE },
};

keyinfo keys_hp45 [35] =
{
  { {  24, 120, 30, 24 }, "1/x",   "y^x",     006, WHITE, MED_GREY },
  { {  72, 120, 30, 24 }, "ln",    "log",     004, WHITE, MED_GREY },
  { { 120, 120, 30, 24 }, "e^x",   "10^x",    003, WHITE, MED_GREY },
  { { 168, 120, 30, 24 }, "FIX",   "SCI",     002, WHITE, MED_GREY },
  { { 216, 120, 30, 24 }, "",      "",        000, GOLD,  GOLD },

  { {  24, 168, 30, 24 }, "x^2",   "sqrt(x)", 056, WHITE, MED_GREY },
  { {  72, 168, 30, 24 }, "->P",   "->R",     054, WHITE, BLACK },
  { { 120, 168, 30, 24 }, "SIN",   "SIN^-1",  053, WHITE, BLACK },
  { { 168, 168, 30, 24 }, "COS",   "COS^-1",  052, WHITE, BLACK },
  { { 216, 168, 30, 24 }, "TAN",   "TAN^-1",  050, WHITE, BLACK },

  { {  24, 216, 30, 24 }, "x<>y",  "n!",      016, BLACK, LT_GREY },
  { {  72, 216, 30, 24 }, "RDN",   "x,s",     014, BLACK, LT_GREY },
  { { 120, 216, 30, 24 }, "STO",   "->D.MS",  013, BLACK, LT_GREY },
  { { 168, 216, 30, 24 }, "RCL",   "D.MS->",  012, BLACK, LT_GREY },
  { { 216, 216, 30, 24 }, "%",     "delta %", 010, WHITE, MED_GREY },

#if ENTER_KEY_MOD
  { {  24, 264, 78, 24 }, "ENTER^","        DEG", 074, BLACK, LT_GREY },
#else
  { {  24, 264, 78, 24 }, "ENTER^","        DEG", 075, BLACK, LT_GREY },
#endif /* ENTER_KEY_MOD */
  { { 120, 264, 30, 24 }, "CHS",   "RAD",     073, BLACK, LT_GREY },
  { { 168, 264, 30, 24 }, "EEX",   "GRD",     072, BLACK, LT_GREY },
  { { 216, 264, 30, 24 }, "CLX",   "CLEAR",   070, BLACK, LT_GREY },

  { {  24, 312, 24, 24 }, "-",     "",        066, BLACK, LT_GREY },
  { {  73, 312, 37, 24 }, "7",     "cm/in",   064, BLACK, WHITE },
  { { 141, 312, 37, 24 }, "8",     "kg/lb",   063, BLACK, WHITE },
  { { 209, 312, 37, 24 }, "9",     "ltr/gal", 062, BLACK, WHITE },

  { {  24, 360, 24, 24 }, "+",     "",        026, BLACK, LT_GREY },
  { {  73, 360, 37, 24 }, "4",     "",        024, BLACK, WHITE },
  { { 141, 360, 37, 24 }, "5",     "",        023, BLACK, WHITE },
  { { 209, 360, 37, 24 }, "6",     "",        022, BLACK, WHITE },

  { {  24, 408, 24, 24 }, "x",     "",        036, BLACK, LT_GREY },
  { {  73, 408, 37, 24 }, "1",     "",        034, BLACK, WHITE },
  { { 141, 408, 37, 24 }, "2",     "",        033, BLACK, WHITE },
  { { 209, 408, 37, 24 }, "3",     "",        032, BLACK, WHITE },

  { {  24, 456, 24, 24 }, "/",     "",        046, BLACK, LT_GREY },
  { {  73, 456, 37, 24 }, "0",     "LASTX",   044, BLACK, WHITE },
  { { 141, 456, 37, 24 }, ".",     "Pi",      043, BLACK, WHITE },
  { { 209, 456, 37, 24 }, "SIG+",  "SIG-",    042, BLACK, WHITE },
};

keyinfo keys_hp55 [35] =
{
  { {  24, 120, 30, 24 }, "SIG+",   "SIG-",     006, BLACK, LT_GREY },
  { {  72, 120, 30, 24 }, "y^x",    "sin -1",     004, BLACK, LT_GREY },
  { { 120, 120, 30, 24 }, "1/x",   "cos -1",    003, BLACK, LT_GREY },
  { { 168, 120, 30, 24 }, "%",   "tan -1",     002, BLACK, LT_GREY },
  { { 216, 120, 30, 24 }, "BST",      "",        000, WHITE,  MED_GREY },

  { {  24, 168, 30, 24 }, "y^",   "L.R.", 056, BLACK, LT_GREY },
  { {  72, 168, 30, 24 }, "x<>y",   "ln e^x",     054, BLACK, LT_GREY },
  { { 120, 168, 30, 24 }, "RDN",   "log 10^x",  053, BLACK, LT_GREY },
  { { 168, 168, 30, 24 }, "FIX",   "SCI",  052, BLACK, LT_GREY },
  { { 216, 168, 30, 24 }, "SST",   "",  050, WHITE, MED_GREY },

  { {  24, 216, 30, 24 }, "f",  "",      016, BLACK, GOLD },
  { {  72, 216, 30, 24 }, "g",   "",     014, BLACK, BLUE },
  { { 120, 216, 30, 24 }, "STO",   "x s",  013, BLACK, LT_GREY },
  { { 168, 216, 30, 24 }, "RCL",   "LASTx",  012, BLACK, LT_GREY },
  { { 216, 216, 30, 24 }, "GTO",     "x<=y x=y", 010, WHITE, BLACK },

  { {  24, 264, 78, 24 }, "ENTER^", "H.MS+ -", 075, BLACK, LT_GREY },
  { { 120, 264, 30, 24 }, "CHS",   "sqrt(x) x^2",     073, BLACK, LT_GREY },
  { { 168, 264, 30, 24 }, "EEX",   "n!",     072, BLACK, LT_GREY },
  { { 216, 264, 30, 24 }, "CLX",   "CLR CL.R",   070, BLACK, LT_GREY },

  { {  24, 312, 24, 24 }, "-",     "DEG",        066, BLACK, LT_GREY },
  { {  73, 312, 37, 24 }, "7",     "in mm",   064, BLACK, WHITE },
  { { 141, 312, 37, 24 }, "8",     "ft m",   063, BLACK, WHITE },
  { { 209, 312, 37, 24 }, "9",     "gal l", 062, BLACK, WHITE },

  { {  24, 360, 24, 24 }, "+",     "RAD",        026, BLACK, LT_GREY },
  { {  73, 360, 37, 24 }, "4",     "lbm kg",        024, BLACK, WHITE },
  { { 141, 360, 37, 24 }, "5",     "lbf N",        023, BLACK, WHITE },
  { { 209, 360, 37, 24 }, "6",     "degF degC",        022, BLACK, WHITE },

  { {  24, 408, 24, 24 }, "x",     "GRD",        036, BLACK, LT_GREY },
  { {  73, 408, 37, 24 }, "1",     "H H.MS",        034, BLACK, WHITE },
  { { 141, 408, 37, 24 }, "2",     "D R",        033, BLACK, WHITE },
  { { 209, 408, 37, 24 }, "3",     "Btu J",        032, BLACK, WHITE },

  { {  24, 456, 24, 24 }, "/",     "",        046, BLACK, LT_GREY },
  { {  73, 456, 37, 24 }, "0",     "R P",   044, BLACK, WHITE },
  { { 141, 456, 37, 24 }, ".",     "Pi",      043, BLACK, WHITE },
  { { 209, 456, 37, 24 }, "R/S",  "",    042, WHITE, BLACK },
};


static void draw_string (char *s, XRectangle *rect, int fg, int bg)
{
  XCharStruct cs;
  int dir_hint, ascent, descent;

  XSetForeground (mydisplay, mygc, pixval [bg]);

  XFillRectangles (mydisplay, mywindow, mygc, rect, 1);

  if (s)
    {
      XTextExtents (myfont, s, strlen (s),
		    & dir_hint, & ascent, & descent, & cs);

      XSetForeground (mydisplay, mygc, pixval [fg]);
      XSetBackground (mydisplay, mygc, pixval [bg]);

      XDrawImageString (mydisplay, mywindow, mygc,
		   rect->x + (rect->width - (cs.lbearing + cs.rbearing)) / 2,
		   rect->y + rect->height - (rect->height - (cs.ascent + cs.descent)) / 2,
		   s, strlen (s));
    }
}


static void draw_display (char *str)
{
  draw_string (str, & display_rect, BR_RED, DK_RED);
  XSync (mydisplay, 0);
}


static void draw_calc (void)
{
  int i;
  XRectangle r;

  for (i = 0; i < /*(sizeof (*keys) / sizeof (keyinfo))*/35; i++)
    {
      draw_string ((*keys) [i].label, & (*keys) [i].rect, (*keys) [i].fg, (*keys) [i].bg);
      r = (*keys) [i].rect;
      r.y -= r.height;
      draw_string ((*keys) [i].flabel, & r, GOLD, DK_GREY);
    }
  XSync (mydisplay, 0);
}


static int pt_in_rect (int x, int y, XRectangle *rect)
{
  return ((x >= rect->x) && (x < (rect->x + rect->width)) &&
	  (y >= rect->y) && (y < (rect->y + rect->height)));
}


static int find_key (int x, int y)
{
  int i;

  for (i = 0; i < /*(sizeof ((*keys)) / sizeof (keyinfo))*/35; i++)
    {
      if (pt_in_rect (x, y, & (*keys) [i].rect))
	return (i);
    }
  return (-1);
}





#define LINE_WIDTH 1

static void init_graphics (int argc, char *argv[], char *window_name)
{
  XSizeHints myhint;

  mydisplay = XOpenDisplay ("");
  if (!mydisplay)
    {
      fprintf (stderr, "Can't init X\n");
      exit (1);
    }

  myscreen = DefaultScreen (mydisplay);
  white = WhitePixel (mydisplay, myscreen);
  black = BlackPixel (mydisplay, myscreen);

  init_colors ();

  window_width = WINDOW_WIDTH;
  window_height = WINDOW_HEIGHT;

  myhint.x = 50; myhint.y = 50;
  myhint.width = window_width;
  myhint.height = window_height;
  myhint.flags = PPosition | PSize;

  mywindow = XCreateSimpleWindow
    (mydisplay,
     DefaultRootWindow (mydisplay),
     myhint.x, myhint.y, myhint.width, myhint.height,
     5, 
     black, pixval [DK_GREY]);

  XSetStandardProperties (mydisplay, mywindow, window_name, window_name,
			  None, argv, argc, &myhint);

  mygc = XCreateGC (mydisplay, mywindow, 0, 0);

  myfont = XLoadQueryFont (mydisplay, "6x13");
  XSetFont (mydisplay, mygc, myfont->fid);

  XSetLineAttributes (mydisplay, mygc, LINE_WIDTH, LineSolid, CapButt, JoinMiter);

  XSelectInput (mydisplay, mywindow,
		KeyPressMask | KeyReleaseMask | 
		ButtonPressMask | ButtonReleaseMask |
		ExposureMask );

  XMapRaised (mydisplay, mywindow);
}


static void handle_events (void)
{
  XEvent event;
  int key;

  while (XEventsQueued (mydisplay, QueuedAfterReading) != 0)
    {
      XNextEvent (mydisplay, & event);
      switch (event.type)
	{
	case KeyPress:
	case KeyRelease:
	  if (event.xkey.keycode == escape_code)
	    {
	      exit (0);
	    }
	  else if (keymap [event.xkey.keycode])
	    {
	      *(keymap[event.xkey.keycode]) = (event.type == KeyPress);
	    }
	  break;
	case ButtonPress:
	  key = find_key (event.xbutton.x, event.xbutton.y);
	  if (key >= 0)
	    pressed_key = key;
	  break;
	case ButtonRelease:
	  pressed_key = -1;
	  break;
	case Expose:
	  if (event.xexpose.count == 0)
	    {
	      draw_calc ();
	      draw_display (disp_buf);
	    }
	  break;
	case NoExpose:
	  break;
	default:
	  printf ("unknown event %d\n", event.type);
	  break;
	}
    }
}


void init_display (int argc, char *argv[])
{
  int n;
  disp_buf [0] ='\0';
  keys = &keys_hp45;
  n = strlen(argv[0]);
  if (strcmp(argv[0]+n-4,"hp35")==0)
    keys = &keys_hp35;
  if (strcmp(argv[0]+n-4,"hp55")==0)
    keys = &keys_hp55;
  init_graphics (argc, argv, argv[0]+n-4);
  draw_calc ();
}


void update_display (char *str)
{
  if (strcmp (disp_buf, str) != 0)
    {
      strcpy (disp_buf, str);
      draw_display (str);
    }
}


/* returns -1 if no key pressed */
int check_keyboard (void)
{
  handle_events ();
  if (pressed_key >= 0)
    return ((*keys) [pressed_key].keycode);
  else
    return (-1);
}


