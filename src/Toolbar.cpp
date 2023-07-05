#include <gtkmm.h>
#include <Toolbar.hpp>
#include <iostream>
#include <string>

using namespace std;

Toolbar::Toolbar() : AFileBtn(), ADirBtn("+D"), ToggleBtn() {
    auto css = Gtk::CssProvider::create();
    css->load_from_path("../style/Toolbar.css");
    get_style_context()->add_provider(css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    AFileBtn.set_stock_id(Gtk::Stock::FILE);
    ADirBtn.set_stock_id(Gtk::Stock::DIRECTORY);
    add(AFileBtn);
    add(ADirBtn);

    AFileBtn.signal_clicked().connect(sigc::mem_fun(*this, &Toolbar::AFileBtn_clicked));
    ADirBtn.signal_clicked().connect(sigc::mem_fun(*this, &Toolbar::ADirBtn_clicked));
}

void Toolbar::AFileBtn_clicked() {
    Gtk::Dialog dial("Enter File Name");
    Gtk::Entry entry;
    entry.show();
    dial.get_content_area()->pack_start(entry);
    dial.add_button("Cancel", Gtk::RESPONSE_CANCEL);
    dial.add_button("Apply", Gtk::RESPONSE_APPLY);

    string filename;
    int result = dial.run();
    if (result == Gtk::RESPONSE_APPLY)
        filename = entry.get_text();
        
    std::cout<<filename<<std::endl;
}

void Toolbar::ADirBtn_clicked() {
    std::cout<<"Create Dir Here"<<std::endl;
}

Toolbar::~Toolbar() {

}