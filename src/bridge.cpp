// This File contains wrappers that bridge the direct access to the filesystem (fsutil) and the frontend

#include <gtkmm.h>
#include <bridge.hpp>
#include <fsutil.hpp>
#include <Modal.hpp>
#include <Browser.hpp>
#include <string>
#include <iostream>
#include <filesystem>
#include <thread>

using namespace std;
namespace fs = filesystem;

namespace bridge {

  void wChangeBrowser(Gtk::Window* mainWindow, Browser* newBrowser) {
    mainWindow->set_focus(*newBrowser);
  }

  void wChangeDir(Gtk::Window* Parent, Browser* browser, Gtk::Entry *pathentry, fs::path directory) {
    try {
      auto new_contents = fs::directory_iterator(directory);

      browser->ClearElements();
      browser->CurrentPath = directory;
      pathentry->set_text(browser->CurrentPath.c_str());
      
      for (const auto& entry : new_contents) {
        browser->AddElement(entry);
      }

      // Initialize Filewatcher
      fsutil::DeallocateWatcher(browser->watcher_state, browser->watcher_mutex, browser->watcher_cv);
      // Initialize Filewatcher
      thread t([directory, browser] {
        fsutil::InitWatcher(directory, browser->watcher_state, browser->watcher_mutex, browser->watcher_cv,
          [browser](string filename){
            // On create
            fs::path cur_path = browser->CurrentPath;
            cur_path.append(filename);
            Glib::signal_idle().connect_once([browser, cur_path] {
              browser->AddElement(cur_path);
            });
          }, [browser](string filename){
            // on delete
            Glib::signal_idle().connect_once([browser, filename] {
              browser->RemoveElement(filename);
            });
          }, [browser](string filename){
            // on moved away
            Glib::signal_idle().connect_once([browser, filename] {
              browser->RemoveElement(filename);
            });
          }, [browser](string filename){
            // on moved in
            fs::path cur_path = browser->CurrentPath;
            cur_path.append(filename);
            Glib::signal_idle().connect_once([browser, cur_path] {
              browser->AddElement(cur_path);
            });
          }
        );
      });
      t.detach();
    } catch (const fs::filesystem_error error) {
      Gtk::MessageDialog dial(*Parent, "Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
      dial.set_secondary_text(error.what());
      dial.run();
    }
  }

  void wAddFile(Gtk::Window* Parent, string location) {
    string filename;

    try {
      Modal mod("Create File", filename, Parent);
    } catch (const runtime_error error) {
      // TODO: Implement an error Dialog
      cout<<error.what()<<endl;
    }

    if (filename.empty() || filename==" ") {
      return;
    }

    try {
      fsutil::AddDir(location, filename);
    } catch (const fs::filesystem_error error) {
      Gtk::MessageDialog dial(*Parent, "Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
      dial.set_secondary_text(error.what());
      dial.run();
    }
  }

  void wAddDir(Gtk::Window* Parent, string location) {
    string dirname;

    try {
      Modal mod("Create Directory", dirname, Parent);
    } catch (runtime_error error) {
      // TODO: Implement an error Dialog
      cout<<error.what()<<endl;
    }

    if (dirname.empty() || dirname==" ") {
      return;
    }

    try {
      fsutil::AddDir(location, dirname);
    } catch (fs::filesystem_error error) {
      Gtk::MessageDialog dial(*Parent, "Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
      dial.set_secondary_text(error.what());
      dial.run();
    }
  }
}