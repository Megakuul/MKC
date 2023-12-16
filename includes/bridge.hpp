#ifndef BRIDGE_H
#define BRIDGE_H

#include <string>
#include <gtkmm.h>
#include <filesystem>

#include "Browser.hpp"
#include "Toolbar.hpp"
#include "PathEntry.hpp"

namespace bridge {


  /**
   * Creates a file
   */
  void wAddFile(Gtk::Window* Parent, Browser* browser);

  /**
   * Creates a directory
   */
  void wAddDir(Gtk::Window* Parent, Browser* browser);

  /**
   * Replaces the specified string in all selected objects with the specified replacement string
   */
  void wRenameObjects(Gtk::Window* Parent, Browser *browser);

  /**
   * Modifies attributes of all selected objects with the specified changes
   */
  void wModifyObjects(Gtk::Window* Parent, Browser *browser);

  /**
   * Cleans all files from the provided list
   */
  void wDeleteObjects(Gtk::Window* Parent, Toolbar* tb, Browser* browser);

  /**
   * Restores all '.mkc' files from the provided list
   */
  void wRestoreObject(Gtk::Window* Parent, Toolbar* tb, Browser* browser);

  /**
   * Copies all selected files from the current-browser to the other browser
   */
  void wDirectCopyObjects(Gtk::Window* Parent, Toolbar* tb, Browser* browser, bool cut);

  /**
   * Open object with xdg-open (detached process)
   */
  void wOpenObjectDefault(std::string object);

  /**
   * Open object with a custom operation specified in a popover run dialog (detached process)
   */
  void wOpenObjectRun(Gtk::Widget* Parent, Gdk::Rectangle* rect, std::string object);

  /**
   * Changes the active/current directory for the provided browser
   */
  void wNavigate(Gtk::Window* Parent, Browser* browser, PathEntry *pathentry, std::filesystem::path objectpath);

  /**
   * Changes the active/current Browser
   */
  void wChangeBrowser(Gtk::Window* mainWindow, Browser* newBrowser);

  /**
   * Handles copying/cutting of files (MIME: "x-special/gnome-copied-files")
   *
   * Fully compatible with Nautilus (Gnome) and Thunar (Xfce)
   */
  void wHandleGnomeCopy(Gtk::Window* Parent, Browser* CurrentBrowser, bool cut);
  
  /**
   * Handles Gnome pastes (MIME: "x-special/gnome-copied-files")
   *
   * Fully compatible with Nautilus (Gnome) and Thunar (Xfce)
   */
  void wHandleGnomePaste(Gtk::Window* Parent, Toolbar* tb, Browser* CurrentBrowser);

  /**
   * Autocompletes the last word on the provided entry based on the content of the currentbrowser
   */
  void wAutoComplete(Gtk::Entry *Entry, Browser *CurrentBrowser);
}
#endif
