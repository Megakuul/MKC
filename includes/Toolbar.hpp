#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <gtkmm.h>

class Toolbar : public Gtk::Toolbar {
public:
    Toolbar(Gtk::Window *Parent);
    virtual ~Toolbar();

protected:
    void AFileBtn_clicked(Gtk::Window *Parent);
    void ADirBtn_clicked(Gtk::Window *Parent);

    Gtk::ToolButton AFileBtn;
    Gtk::ToolButton ADirBtn;
    Gtk::ToggleButton ToggleBtn;
};

#endif