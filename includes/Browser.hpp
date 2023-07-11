#ifndef BROWSER_H
#define BROWSER_H

#include <gtkmm.h>
#include <string>
#include <filesystem>

class Browser : public Gtk::TreeView {

public:
  Browser(std::string basePath);
  std::filesystem::path CurrentPath; 

  void AddElement(const std::string name,
                  const std::string type,
                  const int hardlinks, 
                  const int size, 
                  const std::string access, 
                  const std::time_t lastEdited);

  void RemoveElement(const std::string& id);

  void ClearElements();

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

private:
  Glib::RefPtr<Gtk::ListStore> m_listStore;
  ModelColumns m_columns;
  template<typename T>
  void on_header_clicked(Gtk::TreeModelColumn<T>* column);
  void on_type_header_clicked();
  void on_hardlinks_header_clicked();
  void on_size_header_clicked();
  void on_access_header_clicked();
  void on_lastEdited_header_clicked();

  void on_row_activated();
};

#endif