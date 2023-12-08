#include <gtkmm.h>
#include <iostream>
#include <mutex>
#include <utility>

#include "Toolbar.hpp"
#include "Modal.hpp"
#include "bridge.hpp"
#include "Browser.hpp"
#include "gdkmm/event.h"
#include "keyhandler.hpp"

using namespace std;

Toolbar::Toolbar(Gtk::Window *Parent, Browser *&CurrentBrowser)
  : AFileBtn(), ADirBtn(), RnObjBtn(), DObjBtn(), RObjBtn(), CObjBtn(), MObjBtn(), SplitItem(), PMenuBtn() {
    set_name("toolbar");

	dispatcher.connect(sigc::mem_fun(*this, &Toolbar::on_process_map_update));
    
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

	MObjBtn.set_stock_id(Gtk::Stock::CUT);
    MObjBtn.set_tooltip_text(MOVE_KEY_LB);

	PMenuBtn.set_stock_id(Gtk::Stock::GOTO_BOTTOM);
	PMenuBtn.set_tooltip_text("Running Operations");

	SplitItem.set_expand(true);
    
    add(AFileBtn);
    add(ADirBtn);
	add(RnObjBtn);
    add(DObjBtn);
    add(RObjBtn);
	add(CObjBtn);
	add(MObjBtn);

	add(SplitItem);
	
	add(PMenuBtn);

    AFileBtn.signal_clicked().connect([Parent, &CurrentBrowser] {
	  bridge::wAddFile(Parent, CurrentBrowser->CurrentPath);
    });
    ADirBtn.signal_clicked().connect([Parent, &CurrentBrowser] {
	  bridge::wAddDir(Parent, CurrentBrowser->CurrentPath);
    });
	RnObjBtn.signal_clicked().connect([Parent, &CurrentBrowser] {
	  bridge::wRenameObjects(Parent, CurrentBrowser, CurrentBrowser->CurrentPath);
	});
    DObjBtn.signal_clicked().connect([Parent, this, &CurrentBrowser] {
	  bridge::wDeleteObjects(Parent, this, CurrentBrowser->CurrentPath, CurrentBrowser->GetSelectedNames());
    });
    RObjBtn.signal_clicked().connect([Parent, this, &CurrentBrowser] {
	  bridge::wRestoreObject(Parent, this, CurrentBrowser->CurrentPath, CurrentBrowser->GetSelectedNames());
    });
	CObjBtn.signal_clicked().connect([Parent, this, &CurrentBrowser] {
	  bridge::wDirectCopyObjects(
	    Parent,
		this,
		CurrentBrowser->CurrentPath,
		CurrentBrowser->RemoteBrowser->CurrentPath,
		CurrentBrowser->GetSelectedNames(),
		false
	  );
	});
	MObjBtn.signal_clicked().connect([Parent, this, &CurrentBrowser] {
	  bridge::wDirectCopyObjects(
	    Parent,
		this,
		CurrentBrowser->CurrentPath,
		CurrentBrowser->RemoteBrowser->CurrentPath,
		CurrentBrowser->GetSelectedNames(),
		true
	  );
	});
	PMenuBtn.signal_clicked().connect([this] {
	  PMenu.popup_at_widget(&PMenuBtn, Gdk::GRAVITY_SOUTH_EAST, Gdk::GRAVITY_NORTH_EAST, nullptr);
	});
}

size_t Toolbar::init_process() {
  lock_guard<mutex> guard(mut);
  process_ticker++;
  init_queue.push(process_ticker);
  dispatcher.emit();
  return process_ticker;
}

void Toolbar::update_process(size_t id, double progress) {
  lock_guard<mutex> guard(mut);
  update_queue.push(make_pair(id, progress));
  dispatcher.emit();
}

void Toolbar::on_process_map_update() {
  while (true) {
	size_t id;
	{
	  lock_guard<mutex> guard(mut);
	  if (init_queue.empty()) break;
	  id = init_queue.front();
	  init_queue.pop();
	}
	  
	Gtk::MenuItem *menu_item = Gtk::manage(new Gtk::MenuItem());
    
	Gtk::ProgressBar *pb = Gtk::manage(new Gtk::ProgressBar());
	pb->set_fraction(0);

	menu_item->add(*pb);

	{
	  lock_guard<mutex> guard(mut);
	  process_map.insert(make_pair(id, make_pair(menu_item, pb)));
	}

	PMenu.append(*menu_item);
	PMenu.hide();
	PMenu.show_all();
	// TODO: This is generating a Warning, due to the required event
	PMenu.popup_at_widget(&PMenuBtn, Gdk::GRAVITY_SOUTH_EAST, Gdk::GRAVITY_NORTH_EAST, nullptr);
  }

  while (true) {
	size_t id;
	double progress;
	string operation;
	Gtk::MenuItem *item; 
	Gtk::ProgressBar *pb;
	{
	  lock_guard<mutex> guard(mut);
	  if (update_queue.empty()) break;
	  pair front = update_queue.front();
	  id = front.first;
	  progress = front.second;
	  update_queue.pop();
		
	  auto process = process_map.find(id);
	  if (process == process_map.end()) continue;
		
	  item = process->second.first;
	  pb = process->second.second;
	}
	  
	double new_progress = pb->get_fraction() + progress;
	if (new_progress >= 1.0) {
	  {
		lock_guard<mutex> guard(mut);
		process_map.erase(id);
	  }
	  PMenu.remove(*item);
	  PMenu.hide();
	} else {
	  pb->set_fraction(new_progress);
	}
  }
}

