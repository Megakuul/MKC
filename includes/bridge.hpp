#ifndef BRIDGE_H
#define BRIDGE_H

#include <string>
#include <gtkmm.h>
#include <Browser.hpp>
#include <filesystem>

namespace bridge {

  void wAddFile(Gtk::Window* Parent, std::string location);
  void wAddDir(Gtk::Window* Parent, std::string location);
  void wDeleteObjects(Gtk::Window* Parent, std::string location, std::vector<std::string> objectnames);
  void wChangeDir(Gtk::Window* Parent, Browser* browser, Gtk::Entry *pathentry, std::filesystem::path directory);
  void wChangeBrowser(Gtk::Window* mainWindow, Browser* newBrowser);
}
#endif