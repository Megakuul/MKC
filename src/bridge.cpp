// This File contains wrappers that bridge the direct access to the filesystem (fsutil) and the frontend

#include "gdkmm/types.h"
#include "gtkmm/clipboard.h"
#include "gtkmm/radiobutton.h"
#include "gtkmm/radiobuttongroup.h"
#include "gtkmm/selectiondata.h"
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
  
  void wAddFile(Gtk::Window* Parent, string location) {
	
    string filename = ShowInputDial(Parent, "Create File");
	if (filename.empty() || filename==" ") {
	  return;
	}

    try {
	  if (fs::exists(fs::path(location) / filename)) {
		Gtk::MessageDialog dial(*Parent, "Confirmation", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
		dial.set_secondary_text("Overwrite existing file?");
		
		if (dial.run() != Gtk::RESPONSE_OK) {
		  return;
		}
	  }
      fsutil::AddFile(location, filename);
    } catch (const fs::filesystem_error error) {
      Gtk::MessageDialog dial(*Parent, "Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
      dial.set_secondary_text(error.what());
      dial.run();
    }
  }

  void wAddDir(Gtk::Window* Parent, string location) {

	string dirname = ShowInputDial(Parent, "Create Directory");
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

  void wDeleteObjects(Gtk::Window* Parent, string source, vector<string> objectnames) {
    Gtk::MessageDialog dial(*Parent, "Confirmation", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
    dial.set_secondary_text("Delete selected objects recursively?");

    Gtk::CheckButton trashbtn("Move files to trash");
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
      fsutil::DeleteObjects(source, objectnames, operation); 
    } catch (const fs::filesystem_error error) {
      Gtk::MessageDialog dial(*Parent, "Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
      dial.set_secondary_text(error.what());
      dial.run();
    }
  }

  void wRestoreObject(Gtk::Window* Parent, string source, vector<string> objectnames) {
	fsutil::OP operation = ShowOperationDial(Parent);
	if (operation==fsutil::ERROR) {
	  return;
	}
	for (const auto& object : objectnames) {
	  if (fs::path(object).extension() == ".mkc") {
		try {
		  fsutil::RecoverObject(fs::path(source) / object, operation);
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

  void wDirectCopyObjects(Gtk::Window* Parent, string source, string destination, vector<string> objectnames, bool cut) {
    // Do not delete current dir 
    objectnames.erase(
      remove(objectnames.begin(), objectnames.end(), "."), objectnames.end()
    );
    // Do not delete the last dir 
    objectnames.erase(
      remove(objectnames.begin(), objectnames.end(), ".."), objectnames.end()
    );

    try {
	  for (string object : objectnames) {
		fs::path src_buf(fs::path(source) / object);
		fs::path dest_buf(fs::path(destination) / object);
		
		if (cut) fsutil::MoveObject(src_buf, dest_buf, fsutil::SKIP);
		else fsutil::CopyObject(src_buf, dest_buf, fsutil::SKIP); 
	  }
    } catch (const fs::filesystem_error error) {
      Gtk::MessageDialog dial(*Parent, "Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
      dial.set_secondary_text(error.what());
      dial.run();
    }
  }
  
  void wDefaultOpenObject(string path) {
	pid_t pid = fork();
	if (pid == 0) {
	  setsid();

	  execl("/usr/bin/xdg-open", "xdg-open", path.c_str(), (char*) NULL);

	  std::exit(EXIT_FAILURE);
	}
  }

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
			  browser->RemoveElement(file.name);
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
			  browser->RemoveElement(file.name);
              browser->AddElement(file);
            });
          }, [browser](string filename){
            // on moved away
            Glib::signal_idle().connect_once([browser, filename] {
              browser->RemoveElement(filename);
            });
          }, [browser](string filename){
            // on changed
            fs::path cur_path = browser->CurrentPath;
            cur_path.append(filename);
            fsutil::File file;
            fsutil::GetFileInformation(cur_path.c_str(), file);
            Glib::signal_idle().connect_once([browser, file] {
			  browser->RemoveElement(file.name);
              browser->AddElement(file);
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


  void wHandleGnomeCopy(Gtk::Window* Parent, Browser* CurrentBrowser, bool cut) {
	try {
	  auto clipboard = Gtk::Clipboard::get();
	  clipboard->set({Gtk::TargetEntry("x-special/gnome-copied-files")}, [CurrentBrowser, cut](Gtk::SelectionData& data, const unsigned int /* target index */) {
		string data_buf = cut ? "cut" : "copy";
		
		for (auto name : CurrentBrowser->GetSelectedNames()) {
		  fs::path cur_path(CurrentBrowser->CurrentPath);
		  cur_path.append(name);
		  char* cur_uri = dyn_encpath(cur_path.c_str());
		  if (!cur_uri) {
			// If the selection contained an invalid path (not in FS format)
			throw fs::filesystem_error("Invalid filepattern", fs::path(cur_uri), error_code());
		  }

		  data_buf += "\n";
		  data_buf += cur_uri;
		}

		data.set(data.get_target(), data_buf);
	  }, [](){});

	  clipboard->store();
	} catch (fs::filesystem_error fserror) {
	  Gtk::MessageDialog dial(*Parent, "Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
	  dial.set_secondary_text(fserror.what());
	  dial.run();
	}
  }

  void wHandleGnomePaste(Gtk::Window* Parent, Browser* CurrentBrowser) {
	try {
	  auto clipboard = Gtk::Clipboard::get();
	  clipboard->request_contents("x-special/gnome-copied-files", [CurrentBrowser, Parent](const Gtk::SelectionData& data) {
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
            files.append("\n- ");
            files.append(get<0>(file).filename());
          }

		  fsutil::OP operation = ShowOperationDial(Parent, files);
		  if (operation!=fsutil::ERROR) {
			for (const auto& file : overwrites) {
              if (op == "cut") {
                fsutil::MoveObject(get<0>(file).c_str(), get<1>(file).c_str(), operation);
              } else if (op == "copy") {
                fsutil::CopyObject(get<0>(file).c_str(), get<1>(file).c_str(), operation);
              }
            }
		  }
        }
	  });  
	} catch (fs::filesystem_error fserror) {
	  Gtk::MessageDialog dial(*Parent, "Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
	  dial.set_secondary_text(fserror.what());
	  dial.run();
	}
  }
}
