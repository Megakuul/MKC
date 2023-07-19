#include <gtkmm.h>
#include <Browser.hpp>
#include <bridge.hpp>
#include <fsutil.hpp>
#include <string>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

Browser::Browser(Gtk::Window *Parent, string basePath, Browser *&currentBrowser, Gtk::Entry *pathEntry) : 
    CurrentPath(basePath)  {
  
  m_listStore = Gtk::ListStore::create(m_columns);
  set_opacity(0.6);
  set_model(m_listStore);
  get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
  // This function changes the Browser UI on focus
  signal_focus_in_event().connect([this, &currentBrowser, pathEntry](GdkEventFocus* event) {
    currentBrowser->set_opacity(0.6);
    currentBrowser = this;
    currentBrowser->set_opacity(1);
    pathEntry->set_text(CurrentPath.c_str());
    return false;
  });
  // This function is handling the entering of directorys and files
  signal_row_activated().connect([this, pathEntry, Parent](const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column) {
    auto iter = get_model()->get_iter(path);

    string name = iter->get_value(m_columns.name);
    string type = iter->get_value(m_columns.type);
    if (type == "d") {
      fs::path new_path = CurrentPath;
      bridge::wChangeDir(Parent, this, pathEntry, new_path.append(name));
    }
    // TODO: If the value is a file, open it with a text editor
  });

  signal_key_press_event().connect(sigc::mem_fun(
    *this,
    &Browser::on_key_press
  ), false);

  // Name of the element
  auto* nameRenderer = Gtk::manage(new Gtk::CellRendererText());
  auto* name = Gtk::manage(new Gtk::TreeViewColumn(
    "Name",
    *nameRenderer
  ));
  name->add_attribute(nameRenderer->property_text(), m_columns.name);
  nameRenderer->set_padding(3,3);
  name->set_clickable(true);
  name->signal_clicked().connect([this] {
    on_header_clicked(&m_columns.name);
  });
  // Set the max width of the name to 50%;
  signal_size_allocate().connect([name](const Gtk::Allocation& allocation){
    name->set_max_width(allocation.get_width() * 0.5);
  });
    
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
  type->set_clickable(true);
  type->signal_clicked().connect([this] {
    on_header_clicked(&m_columns.type);
  });
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
  hardLinks->set_clickable(true);
  hardLinks->signal_clicked().connect([this] {
    on_header_clicked(&m_columns.hardlinks);
  });
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
  size->set_clickable(true);
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
  size->signal_clicked().connect([this] {
    on_header_clicked(&m_columns.size);
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
  access->set_clickable(true);
  access->signal_clicked().connect([this] {
    on_header_clicked(&m_columns.access);
  });
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
  lastEdited->set_clickable(true);
  // This Custom Cell renderer along the Lambda function for the cell is there to set the date to a local format
  // The set_cell_data_func is always executed if a cell needs to rerender
  lastEdited->set_cell_data_func(*lastEditedRenderer,[this](Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter) {
    const time_t& origin_time = (*iter)[m_columns.lastEdited];

    tm* loc_time = localtime(&origin_time);

    char buffer[30];
    strftime(buffer, sizeof(buffer), "%H:%M %d-%m-%Y", loc_time);

    dynamic_cast<Gtk::CellRendererText*>(renderer)->property_text() = buffer;
  });
  lastEdited->signal_clicked().connect([this] {
    on_header_clicked(&m_columns.lastEdited);
  });
  append_column(*lastEdited);
}

void Browser::AddElement(const fsutil::File &file) {
  auto row = *(m_listStore->append());

  row[m_columns.name] = file.name;
  row[m_columns.type] = file.type;
  row[m_columns.hardlinks] = file.hardlink;
  row[m_columns.size] = file.size;
  row[m_columns.access] = file.access;
  row[m_columns.lastEdited] = file.lastEdited;
}

void Browser::RemoveElement(const string& name) {
  Gtk::TreeModel::Children rows = m_listStore->children();
  for (auto iter = rows.begin(); iter!=rows.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    auto rowname = row[m_columns.name];

    if (rowname == name) {
      m_listStore->erase(iter);
      break;
    }
  }
}

vector<string> Browser::GetSelectedNames() {
  vector<string> names_buf;
  Glib::RefPtr<Gtk::TreeSelection> selection = this->get_selection();

  for (const auto& path : selection->get_selected_rows()) {
    Gtk::TreeModel::iterator iter = this->get_model()->get_iter(path);
    if (iter) {
      Gtk::TreeModel::Row row = *iter;
      Glib::ustring ustr = row[m_columns.name];
      string str(ustr.c_str());
      names_buf.push_back(str);
    }
  }
  return names_buf;
}

void Browser::ClearElements() {
  m_listStore->clear();
}

template<typename T>
void Browser::on_header_clicked(Gtk::TreeModelColumn<T>* column) {
  Gtk::SortType curSortType;
  int curSortColId;
  m_listStore->get_sort_column_id(curSortColId, curSortType);
  if (curSortType != Gtk::SORT_ASCENDING) {
    m_listStore->set_sort_column(*column, Gtk::SORT_ASCENDING);
  } else {
    m_listStore->set_sort_column(*column, Gtk::SORT_DESCENDING);
  }
}

bool Browser::on_key_press(GdkEventKey* event)
{  
  if (event->state & GDK_CONTROL_MASK) {
    switch (event->keyval) {
      case GDK_KEY_v:
        bridge::wHandlePaste(this);
      break;
    }
  }
  return false;
}