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
  Browser(Gtk::Window *Parent, std::string basePath, Browser *&currentBrowser, Gtk::Entry *pathEntry);

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
  std::atomic<bool> watcher_state{true};
  /**
   * Mutex for the state of the current state of the filewatcher
   * 
   * 
   * One of the variables to handle the Watcher State
  */
  std::mutex watcher_mutex;
  /**
   * Conditional variable to check when the current state of the filewatcher got changed
   * 
   * 
   * One of the variables to handle the Watcher State
  */
  std::condition_variable watcher_cv;
  /**
   * Current Path of the Filebrowser
  */
  std::filesystem::path CurrentPath;

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
    Gtk::TreeModelColumn<int> size;
    Gtk::TreeModelColumn<Glib::ustring> access;
    Gtk::TreeModelColumn<std::time_t> lastEdited;
  };

  /**
   * RefPtr to the ListStore
  */
  Glib::RefPtr<Gtk::ListStore> m_listStore;

  /**
   * ModelColumns (like a blueprint or template for the columns)
  */
  ModelColumns m_columns;

  /**
   * Adds an element to the Browser
  */
  void AddElement(const fsutil::File &file);

  /**
   * Removes an element to the Browser
  */
  void RemoveElement(const std::string& name);

  /**
   * Clears all the elements from the Browser
  */
  void ClearElements();

private:
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