#ifndef H_KEYHANDLER
#define H_KEYHANDLER

#include <gtkmm.h>
#include "Browser.hpp"
#include "RunShell.hpp"
#include "Toolbar.hpp"

#define ADD_FILE_KEY GDK_KEY_1
#define ADD_FILE_KEY_LB "CTRL + 1"

#define ADD_DIR_KEY GDK_KEY_2
#define ADD_DIR_KEY_LB "CTRL + 2"

#define RENAME_KEY GDK_KEY_g
#define RENAME_KEY_LB "CTRL + G"

#define DELETE_KEY GDK_KEY_BackSpace
#define DELETE_KEY_LB "CTRL + BS"

#define RECOVER_KEY GDK_KEY_b
#define RECOVER_KEY_LB "CTRL + B"

#define DIRECT_COPY_KEY GDK_KEY_k
#define DIRECT_COPY_KEY_LB "CTRL + K"

#define DIRECT_MOVE_KEY GDK_KEY_m
#define DIRECT_MOVE_KEY_LB "CTRL + M"

#define COPY_KEY GDK_KEY_c
#define COPY_KEY_LB "CTRL + C"

#define CUT_KEY GDK_KEY_x
#define CUT_KEY_LB "CTRL + X"

#define PASTE_KEY GDK_KEY_v
#define PASTE_KEY_LB "CTRL + V"

#define PATHENTRY_KEY GDK_KEY_t
#define PATHENTRY_KEY_LB "CTRL + T"

#define BROWSER_1_KEY GDK_KEY_q
#define BROWSER_1_KEY_LB "CTRL + Q"

#define BROWSER_2_KEY GDK_KEY_e
#define BROWSER_2_KEY_LB "CTRL + E"

#define RETURN_KEY GDK_KEY_z
#define RETURN_KEY_LB "CTRL + Z"

#define RUNSHELL_KEY GDK_KEY_r
#define RUNSHELL_KEY_LB "CTRL + R"

#define REFRESH_KEY GDK_KEY_F5

/**
 * Handles keypress on the main-interface
 */
bool HandleKeyPress(
    GdkEventKey* event,
	Gtk::Window* Parent,
	Browser* CurrentBrowser,
	Browser* Browser_1,
	Browser* Browser_2,
	Toolbar* Toolbar,
	Gtk::Entry* Pathentry,
    RunShell* Runentry);

#endif
