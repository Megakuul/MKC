#ifndef MODAL_H
#define MODAL_H

#include <gtkmm.h>
#include <string>

class Modal : public Gtk::Dialog {
public:
    Modal(std::string title, std::string& input);

protected:
    Gtk::Entry entry;
};

#endif