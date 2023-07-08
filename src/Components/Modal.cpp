#include <gtkmm.h>
#include <Modal.hpp>
#include <string>
#include <iostream>

using namespace std;


Modal::Modal(string label, string& input, Gtk::Window *Parent) : entry(), title(label) {
    set_transient_for(*Parent);
    set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
    set_resizable(false);
    set_decorated(false);

    entry.set_margin_right(5);
    entry.set_margin_left(5);
    entry.set_margin_top(10);
    entry.set_margin_bottom(10);
    entry.show();

    title.set_margin_right(5);
    title.set_margin_left(5);
    title.set_margin_top(10);
    title.show();

    get_content_area()->pack_start(title);
    get_content_area()->pack_start(entry);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);
    add_button("Apply", Gtk::RESPONSE_APPLY);

    entry.signal_activate().connect(
        sigc::bind(sigc::mem_fun(*this, &Modal::on_activate),
        input
    ));

    if (run() == Gtk::RESPONSE_APPLY)
        input = entry.get_text();
};

// Apply and exit if the Input is submitted (Enter)
void Modal::on_activate(string &input) {
    input = entry.get_text();
    response(Gtk::RESPONSE_APPLY);
}