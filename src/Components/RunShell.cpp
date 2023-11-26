#include <gtkmm.h>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <thread>
#include <mutex>

#include "RunShell.hpp"
#include "bridge.hpp"
#include "gdk/gdkkeysyms.h"
#include "gtkmm/treemodel.h"
#include "gtkmm/treeviewcolumn.h"

#define READY_COLOR "rgba(154, 222, 123, 0.3)"
#define WAIT_COLOR "rgba(243, 182, 100, 0.3)"
#define READY_TOOLTIP "Shell ready, enter a command"
#define WAIT_TOOLTIP "Process still running, kill it by again pressing 'Enter'"

#define PATH_KEY '%'

using namespace std;

RunShell::RunShell(Browser *&CurrentBrowser, vector<Glib::ustring> completion_items) : current_browser(CurrentBrowser) {
  // Custom UI options
  override_background_color(Gdk::RGBA(READY_COLOR));
  set_margin_right(10);
  set_margin_left(10);
  set_margin_top(5);
  set_margin_bottom(10);
  set_tooltip_text(READY_TOOLTIP);

  // Signal Initialization
  dispatcher.connect(sigc::mem_fun(*this, &RunShell::on_shell_update));
  signal_activate().connect(sigc::mem_fun(*this, &RunShell::on_shell_activate));
  signal_key_press_event().connect(sigc::mem_fun(*this, &RunShell::on_key_press));

  // Completion Initialization
  completion_column = Gtk::TreeModelColumn<Glib::ustring>();
  completion_record.add(completion_column);
  
  completion_store = Gtk::ListStore::create(completion_record);

  completion = Gtk::EntryCompletion::create();
  completion->set_model(completion_store);
  completion->set_text_column(completion_column);
  set_completion(completion);
	
  for (const auto& item : completion_items) {
	auto row = *(completion_store->append());
	row[completion_column] = item;
  }
}

void RunShell::on_shell_activate() {
  {
	lock_guard<mutex> guard(mut);
	if (process!=0) {
	  kill(process, SIGKILL);
	  process = 0;
	  dispatcher.emit();
	  return;
	}
  }
  
  pid_t pid = fork();
  if (pid == -1) return;
  if (pid == 0) {
	chdir(current_browser->CurrentPath.c_str());
	execl("/bin/sh", "sh", "-c", get_text().c_str(), (char*)NULL);

	exit(EXIT_FAILURE);
  } else {
	{
	  lock_guard<mutex> guard(mut);
	  process = pid;
	}
	dispatcher.emit();
	thread([this, pid]() {
	  int status;
	  waitpid(pid, &status, 0);
	  {
		lock_guard<mutex> guard(mut);
		process = 0;
	  }
	  dispatcher.emit();
	}).detach();
  }
}

void RunShell::on_shell_update() {
  set_text("");
  lock_guard<mutex> guard(mut);
  if (process!=0) {
	set_tooltip_text(WAIT_TOOLTIP);
	override_background_color(Gdk::RGBA(WAIT_COLOR));
  } else {
	set_tooltip_text(READY_TOOLTIP);
	override_background_color(Gdk::RGBA(READY_COLOR));
  }
}

bool RunShell::on_key_press(GdkEventKey* event) {
  switch (event->keyval) {
  case GDK_KEY_Tab:
	bridge::wAutoComplete(this, current_browser);
	insert_path();
	return true;
  default:
	return false;
  }
}

void RunShell::insert_path() {
  string cur_text = get_text();
  
  // Read keyword from Hardcoded Macro
  string path_keyword(1, PATH_KEY);

  // Get current path
  string path = current_browser->CurrentPath;
  if (path.back() != '/') path += '/';

  // Iterate over every keyword and replace it with the current path
  size_t start_pos = 0;
  while ((start_pos = cur_text.find(path_keyword, start_pos)) != string::npos) {
	cur_text.replace(start_pos, path_keyword.length(), path);
	start_pos += path.length();
  }
  set_text(cur_text);
  set_position(-1);
}

