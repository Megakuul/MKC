#ifndef MODAL_H
#define MODAL_H

#include <gtkmm.h>
#include <string>

class Modal : public Gtk::Dialog {
public:
    /**
     * Creates a Modal Window
     * @param label Label of the Modal Box
     * @param Parent Parent Window that hosts the Modal
     * @param input input for the Modal Box
     * @param answer answer of the Modal Box (Apply=true, Cancel=false)
    */
    Modal(std::string label, Gtk::Window *Parent, std::string* input, bool *answer=nullptr);

protected:
    Gtk::Entry entry;
    Gtk::Label title;
};

#endif