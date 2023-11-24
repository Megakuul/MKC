#ifndef H_RUNSHELL
#define H_RUNSHELL

#include <gtkmm.h>
#include <mutex>

class RunShell : public Gtk::Entry {
public:
  RunShell(Gtk::Entry *Pathentry);

private:
  void on_shell_activate();
  void on_shell_update();
  std::mutex mut;
  pid_t process = 0;

  Gtk::Entry *pathentry;

  Glib::Dispatcher dispatcher;
};

#endif
