#ifndef BRIDGE_H
#define BRIDGE_H

#include <string>
#include <gtkmm.h>
#include <Browser.hpp>

namespace bridge {

  void wAddFile(Gtk::Window* Parent, std::string location);
  void wAddDir(Gtk::Window* Parent, std::string location);
  void wChangeDir(Browser* browser, std::string directory);
}
#endif