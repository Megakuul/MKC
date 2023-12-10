#include <gtkmm.h>

#include "fsutil.hpp"
#include "Modal.hpp"
#include "gtkmm/dialog.h"
#include "string"
#include "iostream"

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
