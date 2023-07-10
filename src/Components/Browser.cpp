#include <gtkmm.h>
#include <Browser.hpp>
#include <string>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

Browser::Browser() : CurrentPath("/home/linus") {
  set_name("Filebrowser");

  m_listStore = Gtk::ListStore::create(m_columns);

  set_model(m_listStore);

  // Name of the element
  auto* nameRenderer = Gtk::manage(new Gtk::CellRendererText());
  auto* name = Gtk::manage(new Gtk::TreeViewColumn(
    "Name",
    *nameRenderer
  ));
  name->add_attribute(nameRenderer->property_text(), m_columns.name);
  nameRenderer->set_padding(3,3);
  name->set_expand(true);
  name->set_max_width(500);
  append_column(*name);

  // Type of the element (f/d)
  auto* typeRenderer = Gtk::manage(new Gtk::CellRendererText());
  auto* type = Gtk::manage(new Gtk::TreeViewColumn(
    "T",
    *typeRenderer
  ));
  type->add_attribute(typeRenderer->property_text(), m_columns.type);
  typeRenderer->set_alignment(0.5, 0.5);
  typeRenderer->set_padding(3,3);
  type->set_max_width(50);
  append_column(*type);

  // Hardlinks of the element
  auto* hardLinksRenderer = Gtk::manage(new Gtk::CellRendererText());
  auto* hardLinks = Gtk::manage(new Gtk::TreeViewColumn(
    "HL",
    *hardLinksRenderer
  ));
  hardLinks->add_attribute(hardLinksRenderer->property_text(), m_columns.hardlinks);
  hardLinksRenderer->set_alignment(0.5, 0.5);
  hardLinksRenderer->set_padding(3,3);
  hardLinks->set_max_width(50);
  append_column(*hardLinks);

  // Size of the element
  auto* sizeRenderer = Gtk::manage(new Gtk::CellRendererText());
  auto* size = Gtk::manage(new Gtk::TreeViewColumn(
    "Size",
    *sizeRenderer
  ));
  size->add_attribute(sizeRenderer->property_text(), m_columns.size);
  sizeRenderer->set_padding(3,3);
  size->set_max_width(100);
  // This Custom Cell renderer along the Lambda function for the cell is there to convert the size to according standard
  // The set_cell_data_func is always executed if a cell needs to rerender
  size->set_cell_data_func(*sizeRenderer,[this](Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter) {
    const int& origin_num = (*iter)[m_columns.size];
    
    double shorten_num = origin_num;
    size_t index;
    ostringstream ss;
    string suffix;

    for (index = 0; index < 5; ++index) {
      double next_num = shorten_num / 1000;
      if (next_num < 1) {
        break;
      }
      shorten_num = next_num;
    }
    switch (index) {
      case 0: suffix = "B"; break;
      case 1: suffix = "KB"; break;
      case 2: suffix = "MB"; break;
      case 3: suffix = "GB"; break;
      case 4: suffix = "TB"; break;
    }
 
    ss << fixed << setprecision(2) << shorten_num << suffix;
    dynamic_cast<Gtk::CellRendererText*>(renderer)->property_text() = ss.str();
  });
  append_column(*size);

  // Accessrights of the element
  auto* accessRenderer = Gtk::manage(new Gtk::CellRendererText());
  auto* access = Gtk::manage(new Gtk::TreeViewColumn(
    "Access",
    *accessRenderer
  ));
  access->add_attribute(accessRenderer->property_text(), m_columns.access);
  accessRenderer->set_alignment(0.5, 0.5);
  accessRenderer->set_padding(3,3);
  access->set_max_width(100);
  append_column(*access);

  // LastModified attribute of the element
  auto* lastEditedRenderer = Gtk::manage(new Gtk::CellRendererText());
  auto* lastEdited = Gtk::manage(new Gtk::TreeViewColumn(
    "Last Modified",
    *lastEditedRenderer
  ));
  lastEdited->add_attribute(lastEditedRenderer->property_text(), m_columns.lastEdited);
  lastEdited->set_max_width(150);
  lastEditedRenderer->set_padding(3,3);
  // This Custom Cell renderer along the Lambda function for the cell is there to set the date to a local format
  // The set_cell_data_func is always executed if a cell needs to rerender
  lastEdited->set_cell_data_func(*lastEditedRenderer,[this](Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter) {
    const std::time_t& origin_time = (*iter)[m_columns.lastEdited];

    std::tm* loc_time = std::localtime(&origin_time);

    std::stringstream ss;
    ss << std::put_time(loc_time, "%H:%M %m-%d");

    dynamic_cast<Gtk::CellRendererText*>(renderer)->property_text() = ss.str();
  });
  append_column(*lastEdited);

  m_listStore->set_sort_column(m_columns.lastEdited, Gtk::SORT_ASCENDING);
}

void Browser::AddElement(const string name,
                         const string type,
                         const string hardlinks,
                         const int size,
                         const string access, 
                         const time_t lastEdited) {

  auto row = *(m_listStore->append());
  row[m_columns.name] = name;
  row[m_columns.type] = type;
  row[m_columns.hardlinks] = hardlinks;
  row[m_columns.size] = size;
  row[m_columns.access] = access;
  row[m_columns.lastEdited] = lastEdited;
}

void Browser::RemoveElement(const string& name) {
  Gtk::TreeModel::Children rows = m_listStore->children();
  for (auto iter = rows.begin(); iter!=rows.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    auto rowname = row[m_columns.name];

    cout << rowname << endl;
    if (rowname == name) {
      m_listStore->erase(iter);
      break;
    }
  }
}

void Browser::ClearElements() {
  Gtk::TreeModel::Children rows = m_listStore->children();
  for (auto iter = rows.begin(); iter!=rows.end(); ++iter) {
    m_listStore->erase(iter);
  }
}