#include <gtkmm.h>
#include <Modal.hpp>
#include <string>
#include <iostream>

using namespace std;


string ShowInputDial(Gtk::Window *Parent, string label) {
  Gtk::MessageDialog dial(*Parent, label, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);

  Gtk::Entry input;
  dial.get_message_area()->pack_start(input);
  input.show();

  if (dial.run() == Gtk::RESPONSE_OK) {
	return input.get_text();
  } else
	return "";
}

fsutil::OP ShowOperationDial(Gtk::Window *Parent, string files) {
  Gtk::MessageDialog dial(*Parent, "Choose action for conflicting files", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);

  Gtk::RadioButtonGroup selGroup;

  Gtk::RadioButton delbtn("Delete replaced files");
  delbtn.set_group(selGroup);
  delbtn.set_active();
  dial.get_message_area()->pack_start(delbtn);
  delbtn.show();
  Gtk::RadioButton trashbtn("Move replaced files to trash");
  trashbtn.set_group(selGroup);
  dial.get_message_area()->pack_start(trashbtn);
  trashbtn.show();
  Gtk::RadioButton renamebtn("Rename replaced files");
  renamebtn.set_group(selGroup);
  dial.get_message_area()->pack_start(renamebtn);
  renamebtn.show();

  if (files!="") 
	dial.set_secondary_text("processing:\n"+files);

  if (dial.run() == Gtk::RESPONSE_OK) {
	fsutil::OP operation = fsutil::ERROR;
	if (delbtn.get_active()) operation = fsutil::DELETE;
	else if (trashbtn.get_active()) operation = fsutil::TRASH;
	else if (renamebtn.get_active()) operation = fsutil::RENAME;
	return operation;
  } else {
	return fsutil::ERROR;
  }
}
