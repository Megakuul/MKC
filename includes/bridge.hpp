#ifndef BRIDGE_H
#define BRIDGE_H

#include <string>
#include <gtkmm.h>
#include <filesystem>

#include "Browser.hpp"

namespace bridge {


  /**
   * Creates a file
   */
  void wAddFile(Gtk::Window* Parent, std::string location);

  /**
   * Creates a directory
   */
  void wAddDir(Gtk::Window* Parent, std::string location);

  /**
   * Replaces the specified string in all selected objects with the specified replacement string
   */
  void wRenameObjects(Gtk::Window* Parent, Browser *browser, std::string location);

  /**
   * Cleans all files from the provided list
   */
  void wDeleteObjects(Gtk::Window* Parent, std::string source, std::vector<std::string> objectnames);

  /**
   * Restores all '.mkc' files from the provided list
   */
  void wRestoreObject(Gtk::Window* Parent, std::string source, std::vector<std::string> objectnames);

  /**
   * Copies all selected files from the current-browser to the other browser
   */
  void wDirectCopyObjects(Gtk::Window* Parent, std::string source, std::string destination, std::vector<std::string> objectname, bool cut);

  /**
   * Open element with default application in a detached process
   */
  void wOpenObject(std::string filepath);

  /**
   * Changes the active/current directory for the provided browser
   */
  void wNavigate(Gtk::Window* Parent, Browser* browser, Gtk::Entry *pathentry, std::filesystem::path objectpath);

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
  void wHandleGnomePaste(Gtk::Window* Parent, Browser* CurrentBrowser);

  /**
   * Autocompletes the last word on the provided entry based on the content of the currentbrowser
   */
  void wAutoComplete(Gtk::Entry *Entry, Browser *CurrentBrowser);
}
#endif
