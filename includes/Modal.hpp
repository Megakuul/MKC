#ifndef MODAL_H
#define MODAL_H

#include <gtkmm.h>
#include <string>

class Modal : public Gtk::Dialog {
public:
    Modal(std::string label, std::string& input, Gtk::Window *Parent);

protected:
    Gtk::Entry entry;
    Gtk::Label title;
    void on_activate(std::string &input);
};

#endif