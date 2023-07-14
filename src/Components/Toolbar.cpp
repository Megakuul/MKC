#include <gtkmm.h>
#include <Toolbar.hpp>
#include <Modal.hpp>
#include <bridge.hpp>
#include <Browser.hpp>

using namespace std;

Toolbar::Toolbar(Gtk::Window *Parent, Browser *CurrentBrowser) : AFileBtn(), ADirBtn(), DObjBtn() {
    set_name("toolbar");
    
    AFileBtn.set_stock_id(Gtk::Stock::FILE);
    AFileBtn.set_tooltip_text("CTRL + 1");

    ADirBtn.set_stock_id(Gtk::Stock::DIRECTORY);
    ADirBtn.set_tooltip_text("CTRL + 2");

    DObjBtn.set_stock_id(Gtk::Stock::DELETE);
    DObjBtn.set_tooltip_text("CTRL + 3");
    
    add(AFileBtn);
    add(ADirBtn);
    add(DObjBtn);

    AFileBtn.signal_clicked().connect([Parent,CurrentBrowser] {
        bridge::wAddFile(Parent, CurrentBrowser->CurrentPath);
    });
    ADirBtn.signal_clicked().connect([Parent,CurrentBrowser] {
        bridge::wAddDir(Parent, CurrentBrowser->CurrentPath);
    });
    DObjBtn.signal_clicked().connect([Parent,CurrentBrowser] {
        Glib::RefPtr<Gtk::TreeSelection> elements = CurrentBrowser->get_selection();

        bridge::wDeleteObjects(Parent, CurrentBrowser->CurrentPath, CurrentBrowser->GetSelectedNames());
    });
}