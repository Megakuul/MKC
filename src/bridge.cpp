// This File contains wrappers that bridge the direct access to the filesystem (fsutil) and the frontend

#include <gtkmm.h>
#include <bridge.hpp>
#include <fsutil.hpp>
#include <Modal.hpp>
#include <Browser.hpp>
#include <uritopath.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <thread>
#include <vector>
#include <istream>


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
    Gtk::MessageDialog dial(*Parent, "Confirmation", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
    dial.set_secondary_text("Delete selected objects recursively?");

    Gtk::CheckButton trashbtn("Move replaced files to trash");
    dial.get_message_area()->pack_start(trashbtn);
    trashbtn.show();

    if (dial.run() != Gtk::RESPONSE_OK) {
      return;
    }

    // Do not delete current dir 
    objectnames.erase(
      remove(objectnames.begin(), objectnames.end(), "."), objectnames.end()
    );
    // Do not delete the last dir 
    objectnames.erase(
      remove(objectnames.begin(), objectnames.end(), ".."), objectnames.end()
    );

    try {
      const auto operation = trashbtn.get_active() ? fsutil::TRASH : fsutil::DELETE; 
      fsutil::DeleteObjects(location, objectnames, operation); 
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

  void wRestoreObject(Gtk::Window* Parent, string location, vector<string> objectnames) {
    for (const auto& object : objectnames) {
      if (fs::path(object).extension() == ".mkc") {
        try {
          fsutil::RecoverObject(fs::path(location) / object);
        } catch (fs::filesystem_error fserror) {
          Gtk::MessageDialog dial(*Parent, "Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
          dial.set_secondary_text(fserror.what());
          dial.run();
        } catch (exception error) {
          Gtk::MessageDialog dial(*Parent, "Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
          dial.set_secondary_text(error.what());
          dial.run();
        }
      }
    }
  }

  void wHandlePaste(Gtk::Window* Parent, Browser* CurrentBrowser) {
    // TODO: The clipboard is currently only compatible with GNOME
    auto clipboard = Gtk::Clipboard::get();
    clipboard->request_contents("x-special/gnome-copied-files", [CurrentBrowser, Parent](const Gtk::SelectionData& data) {
      try {
        auto text = data.get_data_as_string();
        if (text.empty()) {
          return;
        }
        istringstream stream(text);
        string op;

        // Eat operation
        getline(stream, op);
        if (op != "cut" && op != "copy") {
          return;
        }

        // Eat the files
        string curUri;
        /**
         * List of files that must be overwritten
         * First attr is the src_path the second the dest_path
        */
        vector<tuple<fs::path, fs::path>> overwrites;
        while (getline(stream, curUri)) {
          // Decode URI, the clipboard does return the filepath as html uri (e.g. file:///dir/file)
          char* cur_path = dyn_decuri(curUri.c_str());
          if (!cur_path) {
            // If the clipboard contained an invalid path (not in URI format)
            throw fs::filesystem_error("Invalid filepattern", fs::path(curUri), error_code());
          }

          // Check source path
          fs::path src_path(cur_path);
          if (!fs::exists(src_path)) {
            // If the source path does not exist
            throw fs::filesystem_error("Invalid source path", fs::path(src_path), error_code());
          }

          // Build destination path
          fs::path dest_path(CurrentBrowser->CurrentPath);
          dest_path.append(src_path.filename().c_str());

          // Check if the element already exists
          if (fs::exists(dest_path)) {
            overwrites.push_back(tuple(src_path, dest_path));
            continue;
          }

          // Run the operation
          if (op == "cut") {
            fsutil::MoveObject(src_path.c_str(), dest_path.c_str());
          } else if (op == "copy") {
            fsutil::CopyObject(src_path.c_str(), dest_path.c_str());
          }
        }

        // Handle duplicated elements
        if (!overwrites.empty()) {
          string files;
          for (const auto& file : overwrites) {
            files.append("\n");
            files.append(get<0>(file).filename());
          }
          Gtk::MessageDialog dial(*Parent, "Confirmation", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);

          Gtk::CheckButton trashbtn("Move replaced files to trash");
          dial.get_message_area()->pack_start(trashbtn);
          trashbtn.show();

          dial.set_secondary_text("Overwrite these:\n"+files);

          if (dial.run() == Gtk::RESPONSE_OK) {
            const auto operation = trashbtn.get_active() ? fsutil::TRASH : fsutil::DELETE;
            for (const auto& file : overwrites) {
              if (op == "cut") {
                fsutil::MoveObject(get<0>(file).c_str(), get<1>(file).c_str(), operation);
              } else if (op == "copy") {
                fsutil::CopyObject(get<0>(file).c_str(), get<1>(file).c_str(), operation);
              }
            }
          }
        }
      } catch (fs::filesystem_error fserror) {
        Gtk::MessageDialog dial(*Parent, "Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
        dial.set_secondary_text(fserror.what());
        dial.run();
      }
    });
  }
}