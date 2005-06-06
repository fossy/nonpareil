/*
$Id$
Copyright 1995, 2004, 2005 Eric L. Smith <eric@brouhaha.com>

Nonpareil is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.  Note that I am not
granting permission to redistribute or modify Nonpareil under the
terms of any later version of the General Public License.

Nonpareil is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program (in the file "COPYING"); if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111, USA.
*/

#include <ctype.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "util.h"
#include "display.h"
#include "kml.h"
#include "display_gtk.h"
#include "proc.h"
#include "keyboard.h"
#include "slide_switch.h"
#include "arch.h"
#include "platform.h"
#include "model.h"
#include "state_io.h"
#include "about.h"
#include "sound.h"

#ifdef HAS_DEBUGGER_GUI
  #include "debugger_gui.h"
#endif

#ifdef HAS_DEBUGGER_CLI
  #include "debugger_cli.h"
#endif

#ifndef SHAPE_DEFAULT
#define SHAPE_DEFAULT true
#endif


char *default_path = MAKESTR(DEFAULT_PATH);


typedef struct
{
  gboolean scancode_debug;
  kml_t *kml;
  sim_t *sim;
  GtkWidget *main_window;
  GtkWidget *menubar;  // actually a popup menu in transparency/shape mode
  gui_display_t *gui_display;
  char state_fn [255];
} csim_t;


void usage (FILE *f)
{
  fprintf (f, "%s:  Microcode-level calculator simulator\n",
	   nonpareil_release);
  fprintf (f, "Copyright 1995, 2003, 2004, 2005 Eric L. Smith\n");
  fprintf (f, "http://nonpareil.brouhaha.com/\n");
  fprintf (f, "\n");
  fprintf (f, "usage: %s [options...] kmlfile\n", progname);
  fprintf (f, "options:\n");
  fprintf (f, "   --shape");
  if (SHAPE_DEFAULT)
    fprintf (f, " (default)");
  fprintf (f, "\n");
  fprintf (f, "   --noshape");
  if (! (SHAPE_DEFAULT))
    fprintf (f, " (default)");
  fprintf (f, "\n");
  fprintf (f, "   --kmldebug\n");
  fprintf (f, "   --kmldump\n");
  fprintf (f, "   --scancodedebug\n");
#ifdef HAS_DEBUGGER
  fprintf (f, "   --stop\n");
#endif
}


void process_commands (csim_t *csim,
		       kml_command_list_t *commands,
		       int scancode,
		       int pressed);


void process_command (csim_t *csim,
		      kml_command_list_t *command,
		      int scancode,
		      int pressed)
{
  switch (command->cmd)
    {
    case KML_CMD_MAP:
      if (scancode != command->arg1)
	{
	  fprintf (stderr, "scancode %d has map command for scancode %d\n",
		   scancode, command->arg1);
	  return;
	}
      if (! set_key_state (command->arg2, pressed))
	{
	  fprintf (stderr, "scancode %d has map command for nonexistent key %d\n",
		   scancode, command->arg2);
	  return;
	}
      break;
    case KML_CMD_IFPRESSED:
      if (pressed)
	process_commands (csim, command->then_part, scancode, pressed);
      else if (command->else_part)
	process_commands (csim, command->else_part, scancode, pressed);
      break;
    default:
      fprintf (stderr, "unimplemented command %d\n", command->cmd);
    }
}

void process_commands (csim_t *csim,
		       kml_command_list_t *commands,
		       int scancode,
		       int pressed)
{
  while (commands)
    {
      process_command (csim, commands, scancode, pressed);
      commands = commands->next;
    }
}


gboolean key_event_callback (GtkWidget *widget,
			     GdkEventKey *event,
			     gpointer data)
{
  csim_t *csim = data;
  kml_scancode_t *scancode;

  if ((event->type != GDK_KEY_PRESS) && 
      (event->type != GDK_KEY_RELEASE))
    return (FALSE);  /* why are we here? */

  for (scancode = csim->kml->first_scancode; scancode; scancode = scancode->next)
    {
      if (event->keyval == scancode->scancode)
	{
	  process_commands (csim,
			    scancode->commands,
			    event->keyval,
			    event->type == GDK_KEY_PRESS);
	  return (TRUE);
	}
    }
  if (csim->scancode_debug)
    fprintf (stderr, "unrecognized scancode %d\n", event->keyval);
  return (FALSE);
}


static void main_window_destroy_callback (GtkWidget *widget, gpointer data)
{
  csim_t *csim = data;

  if (csim->state_fn [0])
    state_write_xml (csim->sim, csim->state_fn);
  gtk_main_quit ();
}


static void quit_callback (gpointer callback_data,
			   guint    callback_action,
			   GtkWidget *widget)
{
  csim_t *csim = callback_data;

  if (csim->state_fn [0])
    state_write_xml (csim->sim, csim->state_fn);
  gtk_main_quit ();
}


static void file_open (gpointer callback_data,
		       guint    callback_action,
		       GtkWidget *widget)
{
  csim_t *csim = callback_data;
  GtkWidget *dialog;

  dialog = gtk_file_chooser_dialog_new ("Load Calculator State",
					GTK_WINDOW (csim->main_window),
					GTK_FILE_CHOOSER_ACTION_OPEN,
					GTK_STOCK_CANCEL,
					GTK_RESPONSE_CANCEL,
					GTK_STOCK_OPEN,
					GTK_RESPONSE_ACCEPT,
					NULL);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      char *fn = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      strlcpy (csim->state_fn, fn, sizeof (csim->state_fn));
      g_free (fn);
      state_read_xml (csim->sim, csim->state_fn);
    }

  gtk_widget_destroy (dialog);
}


// handles save (action==1) and save as (action==2)
static void file_save (gpointer callback_data,
		       guint    callback_action,
		       GtkWidget *widget)
{
  csim_t *csim = callback_data;
  GtkWidget *dialog;

  if ((callback_action == 1) && (csim->state_fn [0]))
    {
      state_write_xml (csim->sim, csim->state_fn);
      return;
    }

  dialog = gtk_file_chooser_dialog_new ("Save Calculator State",
					GTK_WINDOW (csim->main_window),
					GTK_FILE_CHOOSER_ACTION_SAVE,
					GTK_STOCK_CANCEL,
					GTK_RESPONSE_CANCEL,
					GTK_STOCK_SAVE,
					GTK_RESPONSE_ACCEPT,
					NULL);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      char *fn = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      strlcpy (csim->state_fn, fn, sizeof (csim->state_fn));
      g_free (fn);
      state_write_xml (csim->sim, csim->state_fn);
    }

  gtk_widget_destroy (dialog);
}


static void edit_copy (gpointer callback_data,
		       guint    callback_action,
		       GtkWidget *widget)
{
  // csim_t *csim = callback_data;
  // $$$ not yet implemented
}


static void edit_paste (gpointer callback_data,
			guint    callback_action,
			GtkWidget *widget)
{
  // csim_t *csim = callback_data;
  // $$$ not yet implemented
}


static void help_about (gpointer callback_data,
			guint    callback_action,
			GtkWidget *widget)
{
  csim_t *csim = callback_data;

  about_dialog (csim->main_window, csim->kml);
}


static GtkItemFactoryEntry menu_items [] =
  {
    { "/_File",         NULL,         NULL,          0, "<Branch>" },
    { "/File/_Open",    "<control>O", file_open,     1, "<StockItem>", GTK_STOCK_OPEN },
    { "/File/_Save",    "<control>S", file_save,     1, "<StockItem>", GTK_STOCK_SAVE },
    { "/File/Save _As", NULL,         file_save,     2, "<Item>" },
    { "/File/sep1",     NULL,         NULL,          0, "<Separator>" },
    { "/File/_Quit",    "<CTRL>Q",    quit_callback, 1, "<StockItem>", GTK_STOCK_QUIT },
    { "/_Edit",         NULL,         NULL,          0, "<Branch>" },
    { "/Edit/_Copy",    "<control>C", edit_copy,     1, "<StockItem>", GTK_STOCK_COPY },
    { "/Edit/_Paste",   "<control>V", edit_paste,    1, "<StockItem>", GTK_STOCK_PASTE },
#ifdef HAS_DEBUGGER
    { "/_Debug",        NULL,         NULL,          0, "<Branch>" },
#endif
#ifdef HAS_DEBUGGER_GUI
    { "/Debug/Show Reg", NULL,        debug_show_reg, 1, "<Item>" },
    { "/Debug/Show RAM", NULL,        debug_show_ram, 1, "<Item>" },
    { "/Debug/Run",     NULL,         debug_run,     1, "<Item>" },
    { "/Debug/Step",    NULL,         debug_step,    1, "<Item>" },
    { "/Debug/Trace",   NULL,         debug_trace,   1, "<ToggleItem>" },
    { "/Debug/Key Trace", NULL,     debug_key_trace, 1, "<ToggleItem>" },
    { "/Debug/RAM Trace", NULL,     debug_ram_trace, 1, "<ToggleItem>" },
#endif // HAS_DEBUGGER
#ifdef HAS_DEBUGGER_CLI
    { "/Debug/Command Window", NULL,     debug_cmd_win, 1, "<ToggleItem>" },
#endif // HAS_DEBUGGER_CLI
    { "/_Help",         NULL,         NULL,          0, "<Branch>" },
    { "/_Help/About",   NULL,         help_about,    1, "<Item>" }
  };

static gint nmenu_items = sizeof (menu_items) / sizeof (GtkItemFactoryEntry);


static GtkWidget *create_menus (csim_t *csim,
				GtkType container_type)
{
  GtkAccelGroup *accel_group;
  GtkItemFactory *item_factory;

  accel_group = gtk_accel_group_new ();
  item_factory = gtk_item_factory_new (container_type,
				       "<main>",
				       accel_group);
  gtk_item_factory_create_items (item_factory, nmenu_items, menu_items, csim);
  gtk_window_add_accel_group (GTK_WINDOW (csim->main_window), accel_group);
  return (gtk_item_factory_get_widget (item_factory, "<main>"));
}


gboolean move_window_callback (GtkWidget *widget,
			       GdkEventButton *event,
			       gpointer data)
{
  csim_t *csim = data;

  if (event->type == GDK_BUTTON_PRESS)
    {
      switch (event->button)
	{
	case 1:  /* left button */
	  gtk_window_begin_move_drag (GTK_WINDOW (csim->main_window),
				      event->button,
				      event->x_root,
				      event->y_root,
				      event->time);
	  break;
	case 3:  /* right button */
	  gtk_menu_popup (GTK_MENU (csim->menubar),
			  NULL,  /* parent_menu_shell */
			  NULL,  /* parent_menu_item */
			  NULL,  /* func */
			  NULL,  /* data */
			  event->button,
			  event->time);
	  break;
	}
    }
  return (FALSE);
}


void set_default_state_path (csim_t *csim)
{
  const char *p;
  model_info_t *model_info;

  model_info = get_model_info (sim_get_model (csim->sim));

  p = g_get_home_dir ();
  strcpy (csim->state_fn, p);
  // $$$ not sure whether we're supposed to g_free() the home dir string

  max_strncat (csim->state_fn, "/.nonpareil", sizeof (csim->state_fn));
  if (! dir_exists (csim->state_fn))
    {
      if (! create_dir (csim->state_fn))
	warning ("can't create directory '%s'\n", csim->state_fn);
    }

  max_strncat (csim->state_fn, "/", sizeof (csim->state_fn));
  max_strncat (csim->state_fn, model_info->name, sizeof (csim->state_fn));
  max_strncat (csim->state_fn, ".nst", sizeof (csim->state_fn));
}


int main (int argc, char *argv[])
{
  csim_t *csim;
  char *kml_name = NULL;
  char *kml_fn, *image_fn, *rom_fn;
#ifdef HAS_DEBUGGER
  char *listing_fn;
#endif

  csim = alloc (sizeof (csim_t));

  gboolean shape = SHAPE_DEFAULT;
  gboolean kml_dump = FALSE;
  gboolean run = TRUE;

  int model;
  model_info_t *model_info;

  GtkWidget *event_box;

  GtkWidget *vbox;
  GtkWidget *fixed;

  GdkPixbuf *file_pixbuf;  /* the entire image loaded from the file */

  GdkPixbuf *background_pixbuf;  /* window background (subset of file_pixbuf) */
  GError *error = NULL;
  GtkWidget *image;

  GdkBitmap *image_mask_bitmap = NULL;

  progname = newstr (argv [0]);

  g_thread_init (NULL);

  gtk_init (& argc, & argv);

  init_sound ();

  while (--argc)
    {
      argv++;
      if (*argv [0] == '-')
	{
	  if (strcasecmp (argv [0], "--shape") == 0)
	    shape = true;
	  else if (strcasecmp (argv [0], "--noshape") == 0)
	    shape = false;
	  else if (strcasecmp (argv [0], "--kmldump") == 0)
	    kml_dump = 1;
	  else if (strcasecmp (argv [0], "--scancodedebug") == 0)
	    csim->scancode_debug = 1;
#ifdef HAS_DEBUGGER
	  else if (strcasecmp (argv [0], "--stop") == 0)
	    run = FALSE;
#endif
	  else
	    fatal (1, "unrecognized option '%s'\n", argv [0]);
	}
      else if (kml_name)
	fatal (1, "only one KML file may be specified\n");
      else
	kml_name = argv [0];
    }

  if (kml_name)
    {
      kml_fn = find_file_in_path_list (kml_name, ".kml", default_path);
      if (! kml_fn)
	fatal (2, "can't find KML file '%s'\n", kml_name);
    }
  else
    {
      char *p = strrchr (progname, '/');
      if (p)
	p++;
      else
	p = progname;
      kml_name = newstrcat (p, ".kml");
      kml_fn = find_file_in_path_list (kml_name, NULL, default_path);
      if (! kml_fn)
	fatal (1, "can't find KML file '%s'\n", kml_name);
    }


  csim->kml = read_kml_file (kml_fn);
  if (! csim->kml)
    fatal (2, "can't read KML file '%s'\n", kml_fn);

  if (kml_dump)
    {
      print_kml (stdout, csim->kml);
      exit (0);
    }

  if (! csim->kml->image)
    fatal (2, "No image file spsecified in KML\n");

  if (! csim->kml->rom)
    fatal (2, "No ROM file specified in KML\n");

  if (! csim->kml->model)
    fatal (2, "No model specified in KML\n");

  model = find_model_by_name (csim->kml->model);
  if (model == MODEL_UNKNOWN)
    fatal (2, "Unrecognized model specified in KML\n");

  model_info = get_model_info (model);

  image_fn = find_file_in_path_list (csim->kml->image, NULL, default_path);
  if (! image_fn)
    fatal (2, "can't find image file '%s'\n", csim->kml->image);

  file_pixbuf = gdk_pixbuf_new_from_file (image_fn, & error);
  if (! file_pixbuf)
    fatal (2, "can't load image '%s'\n", image_fn);

  if (! csim->kml->has_background_size)
    {
      csim->kml->background_size.width = gdk_pixbuf_get_width (file_pixbuf) - csim->kml->background_offset.x;
      csim->kml->background_size.height = gdk_pixbuf_get_height (file_pixbuf) - csim->kml->background_offset.y;
    }

  background_pixbuf = gdk_pixbuf_new_subpixbuf (file_pixbuf,
						csim->kml->background_offset.x,
						csim->kml->background_offset.y,
						csim->kml->background_size.width,
						csim->kml->background_size.height);

  csim->main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  if (csim->kml->has_transparency && shape)
    {
      image_mask_bitmap = (GdkBitmap *) gdk_pixmap_new (NULL,
							csim->kml->background_size.width,
							csim->kml->background_size.height,
							1);
      gdk_pixbuf_render_threshold_alpha (file_pixbuf,
					 image_mask_bitmap,
					 csim->kml->background_offset.x,  /* src_x */
					 csim->kml->background_offset.y,  /* src_y */
					 0, 0,  /* dest_x, _y */
					 csim->kml->background_size.width,
					 csim->kml->background_size.height,
					 csim->kml->transparency_threshold);
    }

  gtk_window_set_resizable (GTK_WINDOW (csim->main_window), FALSE);

  gtk_window_set_title (GTK_WINDOW (csim->main_window),
			csim->kml->title ? csim->kml->title : "Nonpareil");

  event_box = gtk_event_box_new ();
  gtk_container_add (GTK_CONTAINER (csim->main_window), event_box);

  vbox = gtk_vbox_new (FALSE, 1);
  gtk_container_add (GTK_CONTAINER (event_box), vbox);

  if (image_mask_bitmap)
    {
      csim->menubar = create_menus (csim, GTK_TYPE_MENU);
    }
  else
    {
      csim->menubar = create_menus (csim, GTK_TYPE_MENU_BAR);
      gtk_box_pack_start (GTK_BOX (vbox), csim->menubar, FALSE, TRUE, 0);
    }

  fixed = gtk_fixed_new ();
  gtk_widget_set_size_request (fixed,
			       csim->kml->background_size.width,
			       csim->kml->background_size.height);
  gtk_box_pack_end (GTK_BOX (vbox), fixed, FALSE, TRUE, 0);

  if (background_pixbuf != NULL)
    {
      image = gtk_image_new_from_pixbuf (background_pixbuf);
      gtk_fixed_put (GTK_FIXED (fixed), image, 0, 0);
    }

  // Have to show everything here, or gui_display_init() can't construct the
  // GCs for the annunciators.
  gtk_widget_show_all (csim->main_window);

  csim->gui_display = gui_display_init (csim->kml,
					csim->main_window,
					event_box,
					fixed,
					file_pixbuf);
  if (! csim->gui_display)
    fatal (2, "can't initialize display\n");

  csim->sim = sim_init (model,
			model_info->clock_frequency,
			model_info->ram_size,
			csim->kml->character_segment_map,
			(display_update_callback_fn_t *) gui_display_update,
			csim->gui_display);

#ifdef HAS_DEBUGGER_GUI
  init_debugger_gui (csim->sim);
#endif

  add_slide_switches (csim->sim, csim->kml, background_pixbuf, fixed);
  add_keys (csim->sim, csim->kml, background_pixbuf, fixed);

  if (image_mask_bitmap)
    {
      gtk_widget_shape_combine_mask (csim->main_window,
				     image_mask_bitmap,
				     0,
				     0);

      gtk_window_set_decorated (GTK_WINDOW (csim->main_window), FALSE);
    }

  // Have to show everything again, now that we've done gui_display_init()
  // and combined the shape mask.
  gtk_widget_show_all (csim->main_window);

  g_signal_connect (G_OBJECT (csim->main_window),
		    "key_press_event",
		    G_CALLBACK (key_event_callback),
		    csim);

  g_signal_connect (G_OBJECT (csim->main_window),
		    "key_release_event",
		    G_CALLBACK (key_event_callback),
		    csim);

  if (image_mask_bitmap)
    {
      g_signal_connect (G_OBJECT (csim->main_window),
			"button_press_event",
			G_CALLBACK (move_window_callback),
			csim);
    }

  g_signal_connect (G_OBJECT (csim->main_window),
		    "destroy",
		    GTK_SIGNAL_FUNC (main_window_destroy_callback),
		    csim);

  rom_fn = find_file_in_path_list (csim->kml->rom, NULL, default_path);
  if (! rom_fn)
    fatal (2, "can't find ROM file '%s'\n", csim->kml->rom);

  if (! sim_read_object_file (csim->sim, rom_fn))
    fatal (2, "can't read object file '%s'\n", rom_fn);

#ifdef HAS_DEBUGGER
  if (csim->kml->rom_listing)
    {
      listing_fn = find_file_in_path_list (csim->kml->rom_listing, NULL, default_path);
      if (! listing_fn)
	warning ("can't find ROM listing file '%s'\n", csim->kml->rom_listing);
      else if (! sim_read_listing_file (csim->sim, listing_fn))
	warning ("can't read ROM listing file '%s'\n", listing_fn);
    }
#endif

  sim_reset (csim->sim);

  init_slide_switches ();

  set_default_state_path (csim);

  if ((csim->state_fn [0]) && file_exists (csim->state_fn))
    state_read_xml (csim->sim, csim->state_fn);

  if (run)
    sim_start (csim->sim);

  gtk_main ();

  exit (0);
}
