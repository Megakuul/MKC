#include <gtkmm.h>
#include <Browser.hpp>
#include <string>
#include <iostream>
#include <ctime>

using namespace std;

Browser::Browser() {
  set_name("Filebrowser");

  m_listStore = Gtk::ListStore::create(m_columns);

  set_model(m_listStore);

  append_column("ID", m_columns.id);
  get_column(0)->set_max_width(50);

  append_column("Type", m_columns.type);
  get_column(1)->set_max_width(50);

  append_column("Name", m_columns.name);
  get_column(2)->set_max_width(200);

  append_column("Access", m_columns.access);
  get_column(3)->set_max_width(100);

  append_column("Owner", m_columns.owner);
  get_column(4)->set_max_width(100);

  append_column("Date", m_columns.lastEdited);
  get_column(5)->set_max_width(120);

  m_listStore->set_sort_column(m_columns.lastEdited, Gtk::SORT_ASCENDING);
}

void Browser::AddElement(const string id,
                         const string name,
                         const string type, 
                         const string access, 
                         const string owner, 
                         const time_t lastEdited) {

  auto row = *(m_listStore->append());
  row[m_columns.id] = id;
  row[m_columns.name] = name;
  row[m_columns.type] = type;
  row[m_columns.access] = access;
  row[m_columns.owner] = owner;

  // TODO: This is not working right now
  Glib::DateTime dateTime = Glib::DateTime::create_now_local(lastEdited);

  row[m_columns.lastEdited] = dateTime;
}

void Browser::RemoveElement(const string& id) {
  Gtk::TreeModel::Children rows = m_listStore->children();
  for (auto iter = rows.begin(); iter!=rows.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    auto rowid = row[m_columns.id];

    cout << rowid << endl;
    if (rowid == id) {
      m_listStore->erase(iter);
      break;
    }
  }
}