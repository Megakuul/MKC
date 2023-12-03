#include <gtkmm.h>
#include <iostream>
#include <mutex>

#include "Toolbar.hpp"
#include "Modal.hpp"
#include "bridge.hpp"
#include "Browser.hpp"
#include "gtkmm/enums.h"
#include "gtkmm/menuitem.h"
#include "gtkmm/progressbar.h"
#include "keyhandler.hpp"

using namespace std;

Toolbar::Toolbar(Gtk::Window *Parent, Browser *&CurrentBrowser)
  : AFileBtn(), ADirBtn(), RnObjBtn(), DObjBtn(), RObjBtn(), CObjBtn(), MObjBtn(), PMenu(), PMBtn() {
    set_name("toolbar");
    
    AFileBtn.set_stock_id(Gtk::Stock::FILE);
    AFileBtn.set_tooltip_text(ADD_FILE_KEY_LB);

    ADirBtn.set_stock_id(Gtk::Stock::DIRECTORY);
    ADirBtn.set_tooltip_text(ADD_DIR_KEY_LB);

	RnObjBtn.set_stock_id(Gtk::Stock::CONVERT);
	RnObjBtn.set_tooltip_text(RENAME_KEY_LB);

    DObjBtn.set_stock_id(Gtk::Stock::DELETE);
    DObjBtn.set_tooltip_text(DELETE_KEY_LB);

    RObjBtn.set_stock_id(Gtk::Stock::UNDELETE);
    RObjBtn.set_tooltip_text(RECOVER_KEY_LB);

    CObjBtn.set_stock_id(Gtk::Stock::COPY);
    CObjBtn.set_tooltip_text(COPY_KEY_LB);
	
	MObjBtn.set_stock_id(Gtk::Stock::CUT);
    MObjBtn.set_tooltip_text(MOVE_KEY_LB);

	PMBtn.set_popup(PMenu);
	
	PMenu.show_all();
    
    add(AFileBtn);
    add(ADirBtn);
	add(RnObjBtn);
    add(DObjBtn);
    add(RObjBtn);
	add(CObjBtn);
	add(MObjBtn);
	add(PMBtn);

    AFileBtn.signal_clicked().connect([Parent,&CurrentBrowser] {
	  bridge::wAddFile(Parent, CurrentBrowser->CurrentPath);
    });
    ADirBtn.signal_clicked().connect([Parent,&CurrentBrowser] {
	  bridge::wAddDir(Parent, CurrentBrowser->CurrentPath);
    });
	RnObjBtn.signal_clicked().connect([Parent,&CurrentBrowser] {
	  bridge::wRenameObjects(Parent, CurrentBrowser, CurrentBrowser->CurrentPath);
	});
    DObjBtn.signal_clicked().connect([Parent,&CurrentBrowser] {
	  bridge::wDeleteObjects(Parent, CurrentBrowser->CurrentPath, CurrentBrowser->GetSelectedNames());
    });
    RObjBtn.signal_clicked().connect([Parent,&CurrentBrowser] {
	  bridge::wRestoreObject(Parent, CurrentBrowser->CurrentPath, CurrentBrowser->GetSelectedNames());
    });
	CObjBtn.signal_clicked().connect([Parent,&CurrentBrowser] {
	  bridge::wDirectCopyObjects(
	    Parent,
		CurrentBrowser->CurrentPath,
		CurrentBrowser->RemoteBrowser->CurrentPath,
		CurrentBrowser->GetSelectedNames(),
		false
	  );
	});
	MObjBtn.signal_clicked().connect([Parent,&CurrentBrowser] {
	  bridge::wDirectCopyObjects(
	    Parent,
		CurrentBrowser->CurrentPath,
		CurrentBrowser->RemoteBrowser->CurrentPath,
		CurrentBrowser->GetSelectedNames(),
		true
	  );
	});
}

size_t Toolbar::init_process() {
  lock_guard<mutex> guard(mut);
  process_ticker++;
  init_queue.push(process_ticker);
  return process_ticker;
}

void Toolbar::update_process(size_t id, double progress) {
  lock_guard<mutex> guard(mut);
  update_queue.push(make_pair(id, progress));
}

void Toolbar::on_process_map_update() {
  // TODO: Make this thread safe (mlock it, for initialization and then delock it when adding menuentries)
  while (!init_queue.empty()) {
	// TODO: Init MenuItems and then add it to the process_map
	Gtk::MenuItem *menu_item = Gtk::manage(new Gtk::MenuItem());
	Gtk::Box *box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
	
	Gtk::Label *label = Gtk::manage(new Gtk::Label(to_string(process_ticker)));
	Gtk::ProgressBar *pb = Gtk::manage(new Gtk::ProgressBar());
  }

  while (!update_queue.empty()) {
	// TODO: Update progress and delete items (also in process_map) if progress is 100 or above
  }

  // TODO: Unlock lockguard and add elements
}

