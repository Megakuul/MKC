#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <gtkmm.h>
#include <Browser.hpp>

class Toolbar : public Gtk::Toolbar {
public:
    Toolbar(Gtk::Window *Parent, Browser*& CurrentBrowser);

protected:
  Gtk::ToolButton AFileBtn;
  Gtk::ToolButton ADirBtn;
  Gtk::ToolButton DObjBtn;
  Gtk::ToolButton RObjBtn;
  Gtk::ToolButton CObjBtn;
  Gtk::ToolButton MObjBtn;
};

#endif
