#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <gtkmm.h>
#include <Browser.hpp>
#include <queue>

class Toolbar : public Gtk::Toolbar {
public:
  Toolbar(Gtk::Window *Parent, Browser*& CurrentBrowser);

  size_t init_process();
  void update_process(size_t id, double progress);

protected:
  Gtk::ToolButton AFileBtn;
  Gtk::ToolButton ADirBtn;
  Gtk::ToolButton RnObjBtn;
  Gtk::ToolButton DObjBtn;
  Gtk::ToolButton RObjBtn;
  Gtk::ToolButton CObjBtn;
  Gtk::ToolButton MObjBtn;

  Gtk::MenuButton PMBtn;
  Gtk::Menu PMenu;

private:
  /**
   * Function that is called to update the process_map ui (PMBtn/PMenu)
   */
  void on_process_map_update();

  /**
   * Dispatcher used to mutate subwidgets of the Toolbar
   */
  Glib::Dispatcher dispatcher;
  /**
   * Ticker for process ids, that every process gets it's unique id
   */
  size_t process_ticker;
  /**
   * Mutex to lock when mutating internal data on this object
   */
  std::mutex mut;
  /**
   * Queues processes that are about to be created
   */
  std::queue<size_t> init_queue;

  /**
   * Queues updates to the progress of a process
   */
  std::queue<std::pair<size_t, double>> update_queue;
  /**
   * Maps processes to menuitems
   */
  std::map<size_t, Gtk::MenuItem*> process_map;
};

#endif
