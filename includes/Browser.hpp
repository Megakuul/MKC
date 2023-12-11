#ifndef BROWSER_H
#define BROWSER_H

#include <gtkmm.h>
#include <string>
#include <filesystem>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>

#include "fsutil.hpp"

class Browser : public Gtk::TreeView {

public:
  Browser(Gtk::Window *Parent, std::string basePath, Browser *&currentBrowser, Browser *remoteBrowser, Gtk::Entry *pathEntry);

  ~Browser();

  /**
   * TreeModel for the Browser
  */
  class ModelColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    ModelColumns() {
      add(name);
      add(type);
      add(hardlinks);
      add(size);
      add(access);
      add(lastEdited);
    }
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::ustring> type;
    Gtk::TreeModelColumn<int> hardlinks;
    Gtk::TreeModelColumn<off_t> size;
    Gtk::TreeModelColumn<Glib::ustring> access;
    Gtk::TreeModelColumn<std::time_t> lastEdited;
  };

  struct WatcherReference {
	/**
	 * Current state of the filewatcher
	 * 
	 * 
	 * One of the variables to handle the Watcher State
	 * 
	 * 
	 * If the watcher_state is false it means that the watcher is running,
	 * true will stop the watcher.
	 */
	std::atomic<bool> WatcherState{true};
	/**
	 * Mutex for the state of the current state of the filewatcher
	 * 
	 * 
	 * One of the variables to handle the Watcher State
	 */
	std::mutex WatcherMutex;
	/**
	 * Conditional variable to check when the current state of the filewatcher got changed
	 * 
	 * 
	 * One of the variables to handle the Watcher State
	 */
	std::condition_variable WatcherCv;
  };

  /**
   * Watcher Reference that binds to the current filewatcher
  */
  WatcherReference WatchRef;

  /**
   * Current Path of the Filebrowser
  */
  std::filesystem::path CurrentPath;

  /**
   * Pointer to the "other" Browser
   */
  Browser *RemoteBrowser;

  /**
   * Schedules an element appendation to the el_dispatcher
  */
  void AddElement(const fsutil::File &file);

  /**
   * Schedules an element deletion to the el_dispatcher
  */
  void RemoveElement(const fsutil::File &file);

  /**
   * Schedules an element update to the el_dispatcher
  */
  void UpdateElement(const fsutil::File &file);

  /**
   * Schedules an full browser clear to the el_dispatcher
  */
  void ClearElements();

  /**
   * Gets the Names of all elements in the Browser
  */
  std::vector<std::string> GetAllNames();

  /**
   * Gets the Names of the selected elements in the Browser
  */
  std::vector<std::string> GetSelectedNames();

  /**
   * Disables the Sorting column (and by this disables the overhead of sorting always when inserting rows)
  */
  void DisableSorting();

  /**
   * Sets the Sorting column of the Browser to the default (name)
  */
  void DefaultSorting();

protected:
  /**
   * RefPtr to the ListStore
  */
  Glib::RefPtr<Gtk::ListStore> m_listStore;

  /**
   * ModelColumns (like a blueprint or template for the columns)
  */
  ModelColumns m_columns;

  /**
   * Parent Window reference
  */
  Gtk::Window *m_parent;
  
private:
  /**
   * Operations that can be performed by the el_dispatcher
  */
  enum EL_DISPATCHER_OP {
    APPEND = 0,
	DELETE = 1,
	UPDATE = 2,
	CLEAR = 3,
  };

  /**
   * Mutex to lock el_dispatcher_map
  */
  std::mutex el_dispatcher_mut;

  /**
   * Determines if the dispatcher is running, if set to false, the dispatcher is stopped
  */
  std::atomic<bool> el_dispatcher_running;

  /**
   * Thread that runs the dispatcher
  */
  std::thread el_dispatcher_thread;
  
  /**
   * Queue holding incomming element operations for element dispatcher
  */
  std::vector<std::pair<std::string, std::pair<fsutil::File, EL_DISPATCHER_OP>>> el_dispatcher_queue;

  /**
   * Threadsafe inserts incomming element operations to the dispatcher queue
   *
   * This function will also optimize insertion, by not inputting twice the same operation
   * if an operation is inserted that is already present, it just omits insertion and by that keeps the first operation present
  */
  void insert_el_dispatcher_queue(const std::pair<std::string, std::pair<fsutil::File, EL_DISPATCHER_OP>> &el_pair);

  /**
   * Starts a cron dispatcher that runs in an interval and updates elements
  */
  void start_el_dispatcher(int intervalms);

  /**
   * Directly appends an element to the Browser
  */
  void append_el(const fsutil::File &file);

  /**
   * Directly deletes an element to the Browser
  */
  void delete_el(const fsutil::File &file);

  /**
   * Directly updates an element to the Browser
  */
  void update_el(const fsutil::File &file);

  /**
   * Directly clears all elements from the Browser
  */
  void clear_el();
  
  /**
   * Function that is executed when a header of a column is clicked
   * 
   * @param column Pointer to the column that got clicked
  */
  template<typename T>
  void on_header_clicked(Gtk::TreeModelColumn<T>* column);

  /**
   * Function that is executed when a row is activated (double click or enter)
  */
  void on_row_activated();
};

  
#endif
