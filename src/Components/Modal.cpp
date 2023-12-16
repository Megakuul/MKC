#include <cstdlib>
#include <gtkmm.h>

#include "fsutil.hpp"
#include "Modal.hpp"
#include "string"
#include "iostream"

#define FILE_KEY "$"

#define SHELL_PATH "/bin/sh"
#define SHELL_NAME "sh"

using namespace std;

string ShowInputDial(Gtk::Window *Parent, string label) {
  Gtk::MessageDialog dial(*Parent, label, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);

  Gtk::Entry input;
  dial.get_message_area()->pack_start(input);
  input.show();

  input.signal_activate().connect([&]() {
	dial.response(Gtk::RESPONSE_OK);
  });

  if (dial.run() == Gtk::RESPONSE_OK) {
	return input.get_text();
  } else
	return "";
}

fsutil::FileMod ShowModificationDial(Gtk::Window *Parent, string ex_access, string ex_owner) {
  Gtk::MessageDialog dial(*Parent, "Modify Objects", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);

  Gtk::Entry access_entry;
  access_entry.set_alignment(Gtk::Align::ALIGN_CENTER);
  access_entry.set_placeholder_text("Access Number");
  access_entry.set_text(ex_access);
  dial.get_message_area()->pack_start(access_entry);
  access_entry.show();

  Gtk::Entry owner_entry;
  owner_entry.set_alignment(Gtk::Align::ALIGN_CENTER);
  owner_entry.set_placeholder_text("Owner");
  owner_entry.set_text(ex_owner);
  dial.get_message_area()->pack_start(owner_entry);
  owner_entry.show();


  dial.set_default_response(Gtk::RESPONSE_OK);
  if (dial.run() == Gtk::RESPONSE_OK) {
	// Parse owner (format user:group)
	string owner_user;
	string owner_group;
	string owner_str = owner_entry.get_text();
	size_t pos = owner_str.find(':');
	if (pos!=string::npos) {
	  owner_user = owner_str.substr(0, pos);
	  owner_group = owner_str.substr(pos+1);
	}
	// Return FileMod
	return {
	 access_entry.get_text(),
	 owner_user,
	 owner_group
	};
  } else
	// Return empty FileMod this will lead to no modification
	return {};
}

fsutil::OP ShowOperationDial(Gtk::Window *Parent, string files) {
  Gtk::MessageDialog dial(*Parent, "Choose action for conflicting files", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);

  Gtk::RadioButtonGroup selGroup;

  Gtk::RadioButton del_rad("Delete replaced files");
  del_rad.set_group(selGroup);
  del_rad.set_active();
  dial.get_message_area()->pack_start(del_rad);
  del_rad.show();
  
  Gtk::RadioButton trash_rad("Move replaced files to trash");
  trash_rad.set_group(selGroup);
  dial.get_message_area()->pack_start(trash_rad);
  trash_rad.show();
  
  Gtk::RadioButton rename_rad("Rename replaced files");
  rename_rad.set_group(selGroup);
  dial.get_message_area()->pack_start(rename_rad);
  rename_rad.show();

  if (files!="") 
	dial.set_secondary_text("processing:\n"+files);

  dial.set_default_response(Gtk::RESPONSE_OK);
  if (dial.run() == Gtk::RESPONSE_OK) {
	fsutil::OP operation = fsutil::NONE;
	if (del_rad.get_active()) operation = fsutil::DELETE;
	else if (trash_rad.get_active()) operation = fsutil::TRASH;
	else if (rename_rad.get_active()) operation = fsutil::RENAME;
	return operation;
  } else
	return fsutil::NONE;
}


bool ShowConfirmDial(Gtk::Window *Parent, string label) {
  Gtk::MessageDialog dial(*Parent, "Confirmation", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
  dial.set_secondary_text(label);

  if (dial.run() == Gtk::RESPONSE_OK) return true;
  else return false;
}

fsutil::OP ShowDelConfirmDial(Gtk::Window *Parent) {
  Gtk::MessageDialog dial(*Parent, "Confirmation", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
  dial.set_secondary_text("Delete selected object(s) recursively?");
  
  Gtk::CheckButton trash_bx("Move file(s) to trash");
  dial.get_message_area()->pack_start(trash_bx);
  trash_bx.show();

  dial.set_default_response(Gtk::RESPONSE_OK);
  if (dial.run() == Gtk::RESPONSE_OK) {
	if (trash_bx.get_active()) return fsutil::TRASH;
	else return fsutil::DELETE;
  } else
	return fsutil::NONE;
}


void ShowErrDial(Gtk::Window *Parent, string label) {
  Glib::signal_idle().connect([Parent, label]() {
	Gtk::MessageDialog dial(*Parent, "Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);

	dial.set_secondary_text(label);

	dial.run();

	return false;
  });
}

void ShowRunDial(Gtk::Widget &Parent, Gdk::Rectangle rect, string file) {
  Glib::signal_idle().connect([&Parent, rect, file]() {
	auto popover = Gtk::make_managed<Gtk::Popover>();
	Gtk::Entry* entry = Gtk::make_managed<Gtk::Entry>();
	popover->add(*entry);

	popover->set_relative_to(Parent);
	popover->set_pointing_to(rect);
	popover->set_position(Gtk::POS_BOTTOM);
	popover->show_all();
	
	entry->signal_activate().connect([popover, entry, file]() {
	  string prepared_file = '"' + file + '"';
	  string key = FILE_KEY;
	  string cmd = entry->get_text();
	  if (cmd.empty()) {
		popover->hide();
		return;
	  }

	  size_t pos = 0;
	  while ((pos = cmd.find(key, pos)) != string::npos) {
		cmd.replace(pos, key.length(), prepared_file);
		pos += prepared_file.length();
	  }
	  
	  pid_t pid = fork();
	  if (pid == -1) return;
	  if (pid == 0) {
		execl(SHELL_PATH, SHELL_NAME, "-c", cmd.c_str(), (char*)NULL);

		exit(EXIT_FAILURE);
	  }
	  popover->hide();
	  return;
	});
	return false;
  });
}
