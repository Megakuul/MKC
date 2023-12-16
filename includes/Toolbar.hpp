#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <gtkmm.h>
#include <queue>

#include "Browser.hpp"
#include "PathEntry.hpp"

class Toolbar : public Gtk::Toolbar {
public:
  Toolbar(Gtk::Window *Parent, PathEntry *Pathentry, Browser*& CurrentBrowser);

  size_t init_process();
  void update_process(size_t id, double progress);

protected:
  // Left Items
  Gtk::ToolButton AddFileBtn;
  Gtk::ToolButton AddDirBtn;
  Gtk::ToolButton RenameObjBtn;
  Gtk::ToolButton ModifyObjBtn;
  Gtk::ToolButton CopyObjBtn;
  Gtk::ToolButton MoveObjBtn;
  Gtk::ToolButton DeleteObjBtn;
  Gtk::ToolButton RecoverObjBtn;

  // Splits the section in half (right and left items) by full expanding
  Gtk::ToolItem SplitItem;

  // Right Items
  Gtk::ToolButton PMenuBtn;
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
  size_t process_ticker = 0;
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
   * Maps processes to <MenuItem, ProgressBar>
   */
  std::map<size_t, std::pair<Gtk::MenuItem*, Gtk::ProgressBar*>> process_map;
};

#endif
