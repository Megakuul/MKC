#include <gtkmm.h>
#include <Toolbar.hpp>
#include <Modal.hpp>
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
    string filename;

    unique_ptr<Modal> mod = make_unique<Modal>("Enter File Name", filename);
        
    std::cout<<filename<<std::endl;
}

void Toolbar::ADirBtn_clicked() {
    string dirname;

    unique_ptr<Modal> mod = make_unique<Modal>("Enter Dir Name", dirname);

    std::cout<<dirname<<std::endl;
}

Toolbar::~Toolbar() {

}