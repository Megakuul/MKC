#include <gtkmm.h>
#include <Modal.hpp>
#include <string>
#include <iostream>

using namespace std;


Modal::Modal(string label, Gtk::Window *Parent, string* input, bool *answer) : entry(), title(label) {
    set_transient_for(*Parent);
    set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
    set_resizable(false);
    set_decorated(false);

    title.set_margin_right(5);
    title.set_margin_left(5);
    title.set_margin_top(10);
    title.show();

    entry.set_margin_right(5);
    entry.set_margin_left(5);
    entry.set_margin_top(10);
    entry.set_margin_bottom(10);
    entry.show();

    entry.signal_activate().connect([this,input] {
        *input = entry.get_text();
        response(Gtk::RESPONSE_APPLY);
    });

    get_content_area()->pack_start(title);
    get_content_area()->pack_start(entry);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);
    add_button("Apply", Gtk::RESPONSE_APPLY);

    auto res = run();
    if (answer && res == Gtk::RESPONSE_APPLY)
        *answer = true;
    else if (answer)
        *answer = false;
    if (res == Gtk::RESPONSE_APPLY)
        *input = entry.get_text();
};