#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <gtkmm.h>

class Toolbar : public Gtk::Toolbar {
public:
    Toolbar();
    virtual ~Toolbar();

protected:
    void AFileBtn_clicked();
    void ADirBtn_clicked();

    Gtk::ToolButton AFileBtn;
    Gtk::ToolButton ADirBtn;
    Gtk::ToggleButton ToggleBtn;
};

#endif