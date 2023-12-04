#ifndef MODAL_H
#define MODAL_H

#include <gtkmm.h>
#include <string>
#include <fsutil.hpp>

/**
 * Creates a dialog for getting an input
 * @param Parent window to draw the dialog
 * @param label text to display
 * @returns The input text or "" if it was canceled
 */
std::string ShowInputDial(Gtk::Window *Parent, std::string label);

/**
 * Creates a dialog for selecting the operation
 * @param Parent window to draw the dialog
 * @param files that are processed by the operation
 * @returns The operation selected or OP::NONE if it was canceled
 */
fsutil::OP ShowOperationDial(Gtk::Window *Parent, std::string files="");

/**
 * Creates a dialog to confirm an action
 * @param Parent window to draw the dialog
 * @param label text to display
 * @returns true if the operation is confirmed
 */
bool ShowConfirmDial(Gtk::Window *Parent, std::string label);

/**
 * Creates a dialog to confirm deletion and ask if deleted files should be
 * trashed
 * @param Parent window to draw the dialog
 * @returns The operation selected (DELETE | TRASH) or OP::NONE if it was
 * canceled
 */
fsutil::OP ShowDelConfirmDial(Gtk::Window *Parent);

/**
 * Creates a dialog to display an error
 *
 * This dialog is thread safe and can be called from threads different to the Main-Thread
 * @param Parent window to draw the dialog
 * @param label text to display
 */
void ShowErrDial(Gtk::Window *Parent, std::string label);

#endif
