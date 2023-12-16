#include <gtkmm.h>

#include "PathEntry.hpp"

using namespace std;

PathEntry::PathEntry() {

  completion_column = Gtk::TreeModelColumn<Glib::ustring>();
  completion_record.add(completion_column);

  completion_store = Gtk::ListStore::create(completion_record);
  completion = Gtk::EntryCompletion::create();
  completion->set_model(completion_store);
  completion->set_text_column(completion_column);
  set_completion(completion);
}


void PathEntry::AddCompletion(string completion) {
  auto completions = completion_store->children();
  for (auto iter = completions.begin(); iter != completions.end(); ++iter) {
	if ((*iter)[completion_column] == completion) return;
  }
  auto row = completion_store->append();
  (*row)[completion_column] = completion;
}
