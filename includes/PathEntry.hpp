#ifndef PATHENTRY_H
#define PATHENTRY_H

#include <gtkmm.h>

class PathEntry : public Gtk::Entry {
public:
  PathEntry();
  void AddCompletion(std::string completion);

protected:
  Glib::RefPtr<Gtk::EntryCompletion> completion;
  Glib::RefPtr<Gtk::ListStore> completion_store;
  Gtk::TreeModel::ColumnRecord completion_record;
  Gtk::TreeModelColumn<Glib::ustring> completion_column;
};

#endif
