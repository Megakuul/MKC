#include <gtkmm.h>
#include <Modal.hpp>
#include <string>

using namespace std;

// TODO: Currently the Modal is not deallocated, it is just hidden

Modal::Modal(string title, string& input) : entry() {
    set_title(title);

    entry.set_margin_right(5);
    entry.set_margin_left(5);
    entry.set_margin_top(10);
    entry.set_margin_bottom(10);
    entry.show();

    get_content_area()->pack_start(entry);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);
    add_button("Apply", Gtk::RESPONSE_APPLY);

    if (run() == Gtk::RESPONSE_APPLY)
        input = entry.get_text();
};