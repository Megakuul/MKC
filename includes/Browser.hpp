#ifndef BROWSER_H
#define BROWSER_H

#include <gtkmm.h>
#include <string>
#include <filesystem>
#include <fsutil.hpp>
#include <atomic>
#include <mutex>
#include <condition_variable>

class Browser : public Gtk::TreeView {

public:
  Browser(Gtk::Window *Parent, std::string basePath, Browser *&currentBrowser, Browser *remoteBrowser, Gtk::Entry *pathEntry);

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
   * Adds an element to the Browser
  */
  void AddElement(const fsutil::File &file);

  /**
   * Removes an element to the Browser
  */
  void RemoveElement(const std::string& name);

  /**
   * Updates attributes of an element
   */
  void UpdateElement(const fsutil::File &file);

  /**
   * Clears all the elements from the Browser
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
  };
  
  /**
   * Map that holds elements to be dispatched
  */
  std::map<Glib::ustring, std::pair<fsutil::File*, EL_DISPATCHER_OP>> el_dispatcher_map;
  
  /**
   * Starts a cron dispatcher that runs in an interval and updates elements
  */
  void start_el_dispatcher(int intervalms);
  
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

  /**
   * Function that is executed when a key is pressed
  */
  bool on_key_press(GdkEventKey* event);
};

  
#endif
