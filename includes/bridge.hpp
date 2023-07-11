#ifndef BRIDGE_H
#define BRIDGE_H

#include <string>
#include <gtkmm.h>
#include <Browser.hpp>
#include <filesystem>

namespace bridge {

  void wAddFile(Gtk::Window* Parent, std::string location);
  void wAddDir(Gtk::Window* Parent, std::string location);
  void wChangeDir(Browser* browser, Gtk::Entry *pathentry, std::filesystem::path directory);
  void wChangeBrowser(Gtk::Window* mainWindow, Gtk::Entry *pathentry, Browser*& currentBrowser, Browser* newBrowser);
}
#endif