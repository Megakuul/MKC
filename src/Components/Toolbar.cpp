#include <gtkmm.h>
#include <Toolbar.hpp>
#include <Modal.hpp>
#include <bridge.hpp>
#include <iostream>
#include <string>

using namespace std;

Toolbar::Toolbar(Gtk::Window *Parent) : AFileBtn(), ADirBtn(), ToggleBtn() {
    set_name("toolbar");
    
    AFileBtn.set_stock_id(Gtk::Stock::FILE);
    AFileBtn.set_tooltip_text("CTRL + 1");

    ADirBtn.set_stock_id(Gtk::Stock::DIRECTORY);
    ADirBtn.set_tooltip_text("CTRL + 2");
    
    add(AFileBtn);
    add(ADirBtn);

    AFileBtn.signal_clicked().connect(
        sigc::bind(sigc::mem_fun(*this, &Toolbar::AFileBtn_clicked),
        Parent
    ));
    ADirBtn.signal_clicked().connect(
        sigc::bind(sigc::mem_fun(*this, &Toolbar::ADirBtn_clicked),
        Parent
    ));
}

void Toolbar::AFileBtn_clicked(Gtk::Window *Parent) {
    bridge::wAddFile(Parent, "/");
}

void Toolbar::ADirBtn_clicked(Gtk::Window *Parent) {
    bridge::wAddDir(Parent, "/");
}

Toolbar::~Toolbar() {

}