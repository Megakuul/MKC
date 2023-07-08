// This File contains wrappers that bridge the direct access to the filesystem (fsutil) and the frontend

#include <gtkmm.h>
#include <bridge.hpp>
#include <fsutil.hpp>
#include <Modal.hpp>
#include <string>
#include <iostream>

using namespace std;

namespace bridge {

  void wAddFile(Gtk::Window* Parent, string location) {
    string filename;

    try {
      unique_ptr<Modal> mod = make_unique<Modal>("Create File", filename, Parent);
    } catch (runtime_error error) {
      // TODO: Implement an error Dialog
      cout<<error.what()<<endl;
    }

    if (filename.empty() || filename==" ") {
      return;
    }

    try {
      fsutil::AddDir(location, filename);
    } catch (runtime_error error) {
      // TODO: Implement an error Dialog
      cout<<error.what()<<endl;
    }
  }

  void wAddDir(Gtk::Window* Parent, string location) {
    string dirname;

    try {
      unique_ptr<Modal> mod = make_unique<Modal>("Create Directory", dirname, Parent);
    } catch (runtime_error error) {
      // TODO: Implement an error Dialog
      cout<<error.what()<<endl;
    }

    if (dirname.empty() || dirname==" ") {
      return;
    }

    try {
      fsutil::AddDir(location, dirname);
    } catch (runtime_error error) {
      // TODO: Implement an error Dialog
      cout<<error.what()<<endl;
    }
  }
}