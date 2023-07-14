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
#include <vector>

using namespace std;
namespace fs = filesystem;

namespace bridge {

  void wChangeBrowser(Gtk::Window* mainWindow, Browser* newBrowser) {
    mainWindow->set_focus(*newBrowser);
  }

  void wChangeDir(Gtk::Window* Parent, Browser* browser, Gtk::Entry *pathentry, fs::path directory) {
    try {
      vector<fsutil::File> new_content;
      fsutil::GetFilesFromDirectory(directory.c_str(), new_content);

      browser->ClearElements();
      browser->CurrentPath = directory;
      pathentry->set_text(browser->CurrentPath.c_str());

      // Disable Sorting before adding elements (otherwise the performance of the list suffers)
      browser->m_listStore->set_sort_column(Gtk::TreeSortable::DEFAULT_UNSORTED_COLUMN_ID, Gtk::SORT_ASCENDING);
      for (const fsutil::File& file : new_content) {
        browser->AddElement(file);
      }
      // Enable Sorting after adding elements
      browser->m_listStore->set_sort_column(browser->m_columns.name, Gtk::SORT_ASCENDING);

      // Initialize Filewatcher
      fsutil::DeallocateWatcher(browser->watcher_state, browser->watcher_mutex, browser->watcher_cv);
      // Initialize Filewatcher
      thread t([directory, browser] {
        fsutil::InitWatcher(directory, browser->watcher_state, browser->watcher_mutex, browser->watcher_cv,
          [browser](string filename){
            // On create
            fs::path cur_path = browser->CurrentPath;
            cur_path.append(filename);
            fsutil::File file;
            fsutil::GetFileInformation(cur_path.c_str(), file);
            Glib::signal_idle().connect_once([browser, file] {
              browser->AddElement(file);
            });
          }, [browser](string filename){
            // on delete
            Glib::signal_idle().connect_once([browser, filename] {
              browser->RemoveElement(filename);
            });
          }, [browser](string filename){
            // on moved in
            fs::path cur_path = browser->CurrentPath;
            cur_path.append(filename);
            fsutil::File file;
            fsutil::GetFileInformation(cur_path.c_str(), file);
            Glib::signal_idle().connect_once([browser, file] {
              browser->AddElement(file);
            });
          }, [browser](string filename){
            // on moved away
            Glib::signal_idle().connect_once([browser, filename] {
              browser->RemoveElement(filename);
            });
          }
        );
      });
      t.detach();
    } catch (const runtime_error error) {
      Gtk::MessageDialog dial(*Parent, "Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
      dial.set_secondary_text(error.what());
      dial.run();
    }
  }

  void wDeleteObjects(Gtk::Window* Parent, string location, vector<string> objectnames) {
    bool answer;
    Modal mod("Delete selected objects recursively?", Parent, nullptr, &answer);

    try {
      fsutil::DeleteObjects(location, objectnames);
    } catch (const fs::filesystem_error error) {
      Gtk::MessageDialog dial(*Parent, "Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
      dial.set_secondary_text(error.what());
      dial.run();
    }
  }

  void wAddFile(Gtk::Window* Parent, string location) {
    string filename;

    Modal mod("Create File", Parent, &filename);

    if (filename.empty() || filename==" ") {
      return;
    }

    try {
      fsutil::AddFile(location, filename);
    } catch (const fs::filesystem_error error) {
      Gtk::MessageDialog dial(*Parent, "Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
      dial.set_secondary_text(error.what());
      dial.run();
    }
  }

  void wAddDir(Gtk::Window* Parent, string location) {
    string dirname;

    Modal mod("Create Directory", Parent, &dirname);

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