#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm.h>
#include <Toolbar.hpp>
#include <Browser.hpp>

class MainWindow : public Gtk::Window
{
public:
  MainWindow();

protected:
  Gtk::Box m_Box;
  Toolbar m_Toolbar;
  Browser m_Browser;
  bool onKeyPress(GdkEventKey* event);
};

#endif