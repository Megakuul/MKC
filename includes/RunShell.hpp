#ifndef H_RUNSHELL
#define H_RUNSHELL

#include <gtkmm.h>
#include <mutex>

#include "Browser.hpp"

class RunShell : public Gtk::Entry {
public:
  RunShell(Browser *&CurrentBrowser, std::vector<Glib::ustring> completion_items);

protected:
  Glib::RefPtr<Gtk::EntryCompletion> completion;
  Glib::RefPtr<Gtk::ListStore> completion_store;
  Gtk::TreeModel::ColumnRecord completion_record;
  Gtk::TreeModelColumn<Glib::ustring> completion_column;
  
private:
  void on_shell_activate();
  void on_shell_update();
  bool on_key_press(GdkEventKey* event);
  void insert_path();
  
  std::mutex mut;
  pid_t process = 0;
  Browser *&current_browser;
  Glib::Dispatcher dispatcher;
};

#endif
