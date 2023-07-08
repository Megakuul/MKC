#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm.h>
#include <Toolbar.hpp>

class MainWindow : public Gtk::Window
{
public:
  MainWindow();

protected:
  Gtk::Box m_Box;
  Toolbar m_Toolbar;
  bool onKeyPress(GdkEventKey* event);
};

#endif