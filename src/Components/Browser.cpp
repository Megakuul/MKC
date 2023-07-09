#include <gtkmm.h>
#include <Browser.hpp>
#include <string>
#include <iostream>
#include <ctime>
#include <iomanip>

using namespace std;

Browser::Browser() {
  set_name("Filebrowser");

  m_listStore = Gtk::ListStore::create(m_columns);

  set_model(m_listStore);

  // ID of the element
  auto* idRenderer = Gtk::manage(new Gtk::CellRendererText());
  auto* id = Gtk::manage(new Gtk::TreeViewColumn(
    "ID",
    *idRenderer
  ));
  id->add_attribute(idRenderer->property_text(), m_columns.id);
  id->set_max_width(50);
  append_column(*id);
  
  // Type of the element (f/d)
  auto* typeRenderer = Gtk::manage(new Gtk::CellRendererText());
  auto* type = Gtk::manage(new Gtk::TreeViewColumn(
    "T",
    *typeRenderer
  ));
  type->add_attribute(typeRenderer->property_text(), m_columns.type);
  type->set_max_width(50);
  append_column(*type);

  // Name of the element
  auto* nameRenderer = Gtk::manage(new Gtk::CellRendererText());
  auto* name = Gtk::manage(new Gtk::TreeViewColumn(
    "Name",
    *nameRenderer
  ));
  name->add_attribute(nameRenderer->property_text(), m_columns.name);
  name->set_max_width(200);
  append_column(*name);

  // Accessrights of the element
  auto* accessRenderer = Gtk::manage(new Gtk::CellRendererText());
  auto* access = Gtk::manage(new Gtk::TreeViewColumn(
    "Access",
    *accessRenderer
  ));
  access->add_attribute(accessRenderer->property_text(), m_columns.access);
  access->set_max_width(100);
  append_column(*access);

  // Owner of the element
  auto* ownerRenderer = Gtk::manage(new Gtk::CellRendererText());
  auto* owner = Gtk::manage(new Gtk::TreeViewColumn(
    "Owner",
    *ownerRenderer
  ));
  owner->add_attribute(ownerRenderer->property_text(), m_columns.owner);
  owner->set_max_width(100);
  append_column(*owner);

  // LastModified attribute of the element
  auto* lastEditedRenderer = Gtk::manage(new Gtk::CellRendererText());
  auto* lastEdited = Gtk::manage(new Gtk::TreeViewColumn(
    "Last Modified",
    *lastEditedRenderer
  ));
  lastEdited->add_attribute(lastEditedRenderer->property_text(), m_columns.lastEdited);
  lastEdited->set_max_width(120);
  // This Custom Cell renderer along the Lambda function for the cell is there to set the date to a local format
  // The set_cell_data_func is always executed if a cell needs to rerender
  lastEdited->set_cell_data_func(*lastEditedRenderer,[this](Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter) {
    // Get column 'lastEdited' from the current row (edited row)
    const std::time_t& origin_time = (*iter)[m_columns.lastEdited];

    std::tm* loc_time = std::localtime(&origin_time);

    std::stringstream ss;
    ss << std::put_time(loc_time, "%H:%M %Y-%m-%d");

    dynamic_cast<Gtk::CellRendererText*>(renderer)->property_text() = ss.str();
  });
  append_column(*lastEdited);

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
  row[m_columns.lastEdited] = lastEdited;
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