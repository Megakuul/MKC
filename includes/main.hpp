#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm.h>
#include <Toolbar.hpp>
#include <Browser.hpp>

class MainWindow : public Gtk::Window
{
public:
  MainWindow();
  Browser *CurrentBrowser;

protected:
  Gtk::Box m_MainBox;
  Toolbar m_Toolbar;
  Gtk::Box m_PathentryBox;
  Gtk::ToolButton m_Returnbtn;
  Gtk::Entry m_Pathentry;
  Gtk::Box m_SplitBox;
  Browser m_Browser_1;
  Browser m_Browser_2;
  Gtk::ScrolledWindow m_Scrollable_1;
  Gtk::ScrolledWindow m_Scrollable_2;
  bool on_key_press(GdkEventKey* event);
  void on_pathentry_activate();
  void on_returnbtn_clicked();
};

#endif