#ifndef BROWSER_H
#define BROWSER_H

#include <gtkmm.h>
#include <string>

class Browser : public Gtk::TreeView {

public:
  Browser();
  void AddElement(const std::string id,
                  const std::string name,
                  const std::string type, 
                  const std::string access, 
                  const std::string owner, 
                  const std::time_t lastEdited);
  void RemoveElement(const std::string& id);

  class ModelColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    ModelColumns() {
      add(id);
      add(name);
      add(type);
      add(lastEdited);
      add(access);
      add(owner);
    }
    Gtk::TreeModelColumn<Glib::ustring> id;
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::ustring> type;
    Gtk::TreeModelColumn<Glib::DateTime> lastEdited;
    Gtk::TreeModelColumn<Glib::ustring> access;
    Gtk::TreeModelColumn<Glib::ustring> owner;
  };

private:
  Glib::RefPtr<Gtk::ListStore> m_listStore;
  ModelColumns m_columns;
};

#endif