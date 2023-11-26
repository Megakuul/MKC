#ifndef H_KEYHANDLER
#define H_KEYHANDLER

#include <gtkmm.h>
#include "Browser.hpp"
#include "RunShell.hpp"

#define ADD_FILE_KEY GDK_KEY_1
#define ADD_FILE_KEY_LB "CTRL + 1"

#define ADD_DIR_KEY GDK_KEY_2
#define ADD_DIR_KEY_LB "CTRL + 2"

#define DELETE_KEY GDK_KEY_3
#define DELETE_KEY_LB "CTRL + 3"

#define RECOVER_KEY GDK_KEY_4
#define OPEN_RECOVER_KEY GDK_KEY_b
#define RECOVER_KEY_LB "CTRL + 4\nCTRL + b (Open trash)"

#define COPY_KEY GDK_KEY_k
#define COPY_KEY_LB "CTRL + k"

#define MOVE_KEY GDK_KEY_m
#define MOVE_KEY_LB "CTRL + m"

#define PATHENTRY_KEY GDK_KEY_t
#define PATHENTRY_KEY_LB "CTRL + t"

#define BROWSER_1_KEY GDK_KEY_q
#define BROWSER_1_KEY_LB "CTRL + q"

#define BROWSER_2_KEY GDK_KEY_e
#define BROWSER_2_KEY_LB "CTRL + e"

#define RETURN_KEY GDK_KEY_z
#define RETURN_KEY_LB "CTRL + z"

#define RUNSHELL_KEY GDK_KEY_r
#define RUNSHELL_KEY_LB "CTRL + r"

/**
 * Handles keypress on the main-interface
 */
bool HandleKeyPress(
    GdkEventKey* event,
	Gtk::Window* Parent,
	Browser* CurrentBrowser,
	Browser* Browser_1,
	Browser* Browser_2,
	Gtk::Entry* Pathentry,
    RunShell* Runentry);

#endif
