#ifndef BRIDGE_H
#define BRIDGE_H

#include <string>
#include <gtkmm.h>

namespace bridge {

  void wAddFile(Gtk::Window* Parent, std::string location);
  void wAddDir(Gtk::Window* Parent, std::string location);

}
#endif