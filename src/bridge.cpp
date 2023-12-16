// This File contains wrappers that bridge the direct access to the filesystem (fsutil) and the frontend

#include <exception>
#include <gtkmm.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <thread>
#include <utility>
#include <vector>
#include <istream>

#include "bridge.hpp"
#include "PathEntry.hpp"
#include "fsutil.hpp"
#include "Modal.hpp"
#include "Browser.hpp"
#include "uritopath.h"

using namespace std;
namespace fs = filesystem;

#define PROCESS_FRACTION_PRECISION 10000

namespace bridge {
  
  void wAddFile(Gtk::Window* Parent, Browser *browser) {
	string location = browser->CurrentPath;
	
    string filename = ShowInputDial(Parent, "Create File");
	if (filename.empty() || filename==" ") {
	  return;
	}

    try {
	  if (fs::exists(fs::path(location) / filename)) {
		if (!ShowConfirmDial(Parent, "Overwrite existing file?")) {
		  return;
		}
	  }
      fsutil::AddFile(location, filename);
    } catch (const exception &e) {
	  ShowErrDial(Parent, e.what());
    }
  }

  void wAddDir(Gtk::Window* Parent, Browser *browser) {
	string location = browser->CurrentPath;
	
	string dirname = ShowInputDial(Parent, "Create Directory");
	if (dirname.empty() || dirname==" ") {
	  return;
	}

    try {
      fsutil::AddDir(browser->CurrentPath, dirname);
    } catch (exception e) {
	  ShowErrDial(Parent, e.what());
    }
  }

  void wRenameObjects(Gtk::Window* Parent, Browser *browser) {
	string location = browser->CurrentPath;
	vector<string> objectnames = browser->GetSelectedNames();
	
	try {
	  string srcstr = ShowInputDial(Parent, "Source String");
	  if (srcstr.empty()) {
		return;
	  }

	  string deststr = ShowInputDial(Parent, "Replacement String");
	  if (deststr.empty()) {
		return;
	  }

	  fs::path origin_path(location);

	  for (string name : objectnames) {
		size_t pos = name.find(srcstr);
		if (pos == string::npos) continue;

		fs::path src_path = origin_path / name;
		
		name.replace(pos, srcstr.size(), deststr);

		fs::path dest_path = origin_path / name;
		
		fsutil::MoveObject(src_path, dest_path, fsutil::SKIP);
	  }
	} catch (exception &e) {
	  ShowErrDial(Parent, e.what());
    }
  }

  void wModifyObjects(Gtk::Window* Parent, Browser *browser) {
	string source = browser->CurrentPath;
	vector<string> objectnames = browser->GetSelectedNames();
	if (objectnames.empty()) return;

	fsutil::FileMod mods;
	try {
	  // Get data from first file as "example" data for the dialog
	  fsutil::File initfile;
	  string owner;
	  
	  fsutil::GetFileInformation(fs::path(source) / objectnames[0], initfile);
	  fsutil::GetFileOwner(initfile, owner);

	  // Launch user dialog to get modifications
	  mods = ShowModificationDial(Parent, initfile.access, owner);

	  	
	  // Execute modification for all selected files
	  for (string name : objectnames) {
		string cur_src = fs::path(source) / name;
		fsutil::ModifyObject(cur_src, mods);
	  }
	} catch (exception &e) {
	  ShowErrDial(Parent, e.what());
	}
  }

  void wDeleteObjects(Gtk::Window* Parent, Toolbar* tb, Browser *browser) {
	string source = browser->CurrentPath;
	vector<string> objectnames = browser->GetSelectedNames();
	
	fsutil::OP operation = ShowDelConfirmDial(Parent);

    // Do not delete current dir 
    objectnames.erase(
      remove(objectnames.begin(), objectnames.end(), "."), objectnames.end()
    );
    // Do not delete the last dir 
    objectnames.erase(
      remove(objectnames.begin(), objectnames.end(), ".."), objectnames.end()
    );

	size_t tb_process = tb->init_process();
	double fraction =
	  ceil(1.0 / objectnames.size() * PROCESS_FRACTION_PRECISION) / PROCESS_FRACTION_PRECISION;

	for (const string &object : objectnames) {
	  thread([Parent, tb, tb_process, fraction, object, source, operation]() {
		try {
		  CleanObject(fs::path(source) / object, operation);
		} catch (const exception &e) {
		  ShowErrDial(Parent, e.what());
		}
		tb->update_process(tb_process, fraction);
	  }).detach();
	}
  }

  void wRestoreObject(Gtk::Window* Parent, Toolbar* tb, Browser* browser) {
	string source = browser->CurrentPath;
	vector<string> objectnames = browser->GetSelectedNames();
	
	fsutil::OP operation = ShowOperationDial(Parent);
	size_t tb_process = tb->init_process();
	double fraction =
	  ceil(1.0 / objectnames.size() * PROCESS_FRACTION_PRECISION) / PROCESS_FRACTION_PRECISION;
	
	for (const string& object : objectnames) {
	  thread([Parent, tb, tb_process, fraction, object, source, operation]() {
		if (fs::path(object).extension() == ".mkc") {
		  try {
			fsutil::RecoverObject(fs::path(source) / object, operation);
		  } catch (exception &e) {
			ShowErrDial(Parent, e.what());
		  }
		}
		tb->update_process(tb_process, fraction);
	  }).detach();
	}
  }

  void wDirectCopyObjects(Gtk::Window* Parent, Toolbar* tb, Browser* browser, bool cut) {
	string source = browser->CurrentPath;
	string destination = browser->RemoteBrowser->CurrentPath;
	vector<string> objectnames = browser->GetSelectedNames();
	
    // Do not delete current dir 
    objectnames.erase(
      remove(objectnames.begin(), objectnames.end(), "."), objectnames.end()
    );
    // Do not delete the last dir 
    objectnames.erase(
      remove(objectnames.begin(), objectnames.end(), ".."), objectnames.end()
    );

	size_t tb_process = tb->init_process();
	double fraction =
	  ceil(1.0 / objectnames.size() * PROCESS_FRACTION_PRECISION) / PROCESS_FRACTION_PRECISION;

	for (const string& object : objectnames) {
	  thread([Parent, tb, tb_process, fraction, object, source, destination, cut]() {
		try {
		  fs::path src_buf(fs::path(source) / object);
		  fs::path dest_buf(fs::path(destination) / object);
		
		  if (cut) fsutil::MoveObject(src_buf, dest_buf, fsutil::SKIP);
		  else fsutil::CopyObject(src_buf, dest_buf, fsutil::SKIP); 
		} catch (const exception &e) {
		  ShowErrDial(Parent, e.what());
		}
		tb->update_process(tb_process, fraction);
	  }).detach();
	}
  }
  
  void wOpenObjectDefault(string object) {
	pid_t pid = fork();
	if (pid == 0) {
	  setsid();

	  execl("/usr/bin/xdg-open", "xdg-open", object.c_str(), (char*) NULL);

	  std::exit(EXIT_FAILURE);
	}
  }

  void wOpenObjectRun(Gtk::Widget* Parent, Gdk::Rectangle* rect, std::string object) {
	ShowRunDial(*Parent, *rect, object);
  }
  
  void wChangeBrowser(Gtk::Window* mainWindow, Browser* newBrowser) {
    mainWindow->set_focus(*newBrowser);
  }

  void  wNavigate(Gtk::Window* Parent, Browser* browser, PathEntry *pathentry, fs::path objectpath) {
    try {
	  fsutil::File objectstat;
	  fsutil::GetFileInformation(objectpath, objectstat);
	  if (objectstat.type != "d") {
		wOpenObjectDefault(objectpath);
		return;
	  }
	  
      vector<fsutil::File> new_content;
      fsutil::GetFilesFromDirectory(objectpath.c_str(), new_content);

      browser->ClearElements();
      browser->CurrentPath = objectpath;
      pathentry->set_text(browser->CurrentPath.c_str());
	  pathentry->set_position(-1);

	  // Add path to the completions of the pathentry
	  pathentry->AddCompletion(browser->CurrentPath);

      // Disable Sorting before adding elements (otherwise the performance of the list suffers)
      browser->DisableSorting();
      for (const fsutil::File& file : new_content) {
        browser->AddElement(file);
      }
      // Enable Sorting after adding elements
	  browser->DefaultSorting();

      // Initialize Filewatcher
      fsutil::DeallocateWatcher(browser->WatchRef.WatcherState, browser->WatchRef.WatcherMutex, browser->WatchRef.WatcherCv);
      // Initialize Filewatcher
      thread t([objectpath, browser] {
        fsutil::InitWatcher(objectpath, browser->WatchRef.WatcherState, browser->WatchRef.WatcherMutex, browser->WatchRef.WatcherCv,
          [browser](string filename){
            // On create
            fsutil::File file;
            fsutil::GetFileInformation((browser->CurrentPath / filename).c_str(), file);

			browser->AddElement(file);
          }, [browser](string filename){
            // on delete
			fsutil::File file;
            fsutil::GetFileInformation((browser->CurrentPath / filename).c_str(), file);

			browser->RemoveElement(file);
          }, [browser](string filename){
            // on moved in
			fsutil::File file;
            fsutil::GetFileInformation((browser->CurrentPath / filename).c_str(), file);

            browser->AddElement(file);
          }, [browser](string filename){
            // on moved away
			fsutil::File file;
            fsutil::GetFileInformation((browser->CurrentPath / filename).c_str(), file);

			browser->RemoveElement(file);
          }, [browser](string filename){
            // on changed
			fsutil::File file;
            fsutil::GetFileInformation((browser->CurrentPath / filename).c_str(), file);

			browser->UpdateElement(file);
          }
        );
      });
      t.detach();
    } catch (const exception &e) {
	  ShowErrDial(Parent, e.what());
    }
  }


  void wHandleGnomeCopy(Gtk::Window* Parent, Browser* CurrentBrowser, bool cut) {
	try {
	  string data_buf = cut ? "cut" : "copy";
		
	  for (auto name : CurrentBrowser->GetSelectedNames()) {
		if (name == "." || name == "..") continue;
		
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
	  
	  auto clipboard = Gtk::Clipboard::get();
	  clipboard->set({Gtk::TargetEntry("x-special/gnome-copied-files")},
					 [data_buf](Gtk::SelectionData& data, const unsigned int /* target index */) {
		data.set(data.get_target(), data_buf);
	  }, [](){});

	  clipboard->store();
	} catch (exception &e) {
	  ShowErrDial(Parent, e.what());
	}
  }

  void wHandleGnomePaste(Gtk::Window* Parent, Toolbar *tb, Browser* CurrentBrowser) {
	try {
	  auto clipboard = Gtk::Clipboard::get();
	  clipboard->request_contents("x-special/gnome-copied-files", [CurrentBrowser, tb, Parent](const Gtk::SelectionData& data) {
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
        string cur_uri;

		// List for all operations
		vector<pair<pair<fs::path, fs::path>, fsutil::OP>> operations;

		// Read all operations to the operations vector
        while (getline(stream, cur_uri)) {
          // Decode URI, the clipboard does return the filepath as html uri (e.g. file:///dir/file)
          char* cur_path = dyn_decuri(cur_uri.c_str());
          if (!cur_path) {
            // If the clipboard contained an invalid path (not in URI format)
            throw fs::filesystem_error("Invalid filepattern", fs::path(cur_uri), error_code());
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

		  // Set default operation to SKIP
		  fsutil::OP cur_op = fsutil::SKIP;
		  
          // Check if the element already exists
          if (fs::exists(dest_path)) {
			cur_op = ShowOperationDial(Parent);
          }

		  // Add other elements
		  operations.push_back(make_pair(make_pair(src_path, dest_path), cur_op));
        }

		// Process Operations

		size_t tb_process = tb->init_process();
		double fraction =
		  ceil(1.0 / operations.size() * PROCESS_FRACTION_PRECISION) / PROCESS_FRACTION_PRECISION;

		for (const auto& operation : operations) {
		  thread([Parent, tb, tb_process, fraction, operation, op]() {
			try {
			  // Run the operation
			  if (op == "cut") {
				fsutil::MoveObject(operation.first.first, operation.first.second, operation.second);
			  } else if (op == "copy") {
				fsutil::CopyObject(operation.first.first, operation.first.second, operation.second);
			  }
			} catch (const exception &e) {
			  ShowErrDial(Parent, e.what());
			}
			tb->update_process(tb_process, fraction);
		  }).detach();
		}
	  });  
	} catch (exception &e) {
	  ShowErrDial(Parent, e.what());
	}
  }

  void wAutoComplete(Gtk::Entry *Entry, Browser *CurrentBrowser) {
	string cur_text = Entry->get_text();

	// Take last space delimiter
	size_t l_space_del = cur_text.rfind(' ');
	// Take last path delimiter ('/')
	size_t l_path_del = cur_text.rfind('/');
	// Take last path key delimiter
	size_t l_pkey_del = cur_text.rfind('%');

	// Get the last delimiter pos or -1 if no delimiter is found
	int l_del = max({
		l_space_del==string::npos?-1:static_cast<int>(l_space_del),
		l_path_del==string::npos?-1:static_cast<int>(l_path_del),
		l_pkey_del==string::npos?-1:static_cast<int>(l_pkey_del)
    });

	// Split of the last word
	string cur_attr
	  = l_del==-1 ? cur_text : cur_text.substr(l_del + 1);

	// Don't apply if last char is space
	if (cur_attr.empty()) return;

	// Buffer with completions
	vector<string> completions = vector<string>();

	// Extract all values from the column and directly apply the first completor iteration
	for (const string& val : CurrentBrowser->GetAllNames()) {
	  if (cur_attr[0] == val[0]) completions.push_back(val);
	}

	size_t i = 0;
	// Iterate over the cur_attr to extract the matching completions
	while (1) {
	  if (completions.size() == 0) {
		// No completion found
	    return;
	  } else if (completions.size() == 1) {
		// One completion found, insert new text
		cur_text.resize(cur_text.size()-cur_attr.size());
		cur_text += completions[0];
		Entry->set_text(cur_text);
		// Set cursor to the end
		Entry->set_position(-1);
		return;
	  }

	  // Iterate further if the index is <= cur_attr
	  if (i <= cur_attr.length()) i++;
	  else break;

	  // If cur_attr==i the str is out of bound, this case is catched here
	  // Check if content of cur_attr is read and increment counter ifn
	  if (i == cur_attr.length()) {
		// Multiple completions found

		// Find shortest completion
		string shortest_completion = completions[0];
		for (size_t j = 1; j < completions.size(); ++j) {
		  if (completions[j].length() < shortest_completion.length()) {
			shortest_completion = completions[j];
		  }
		}
		// Clear the completion list
		completions.clear();
		// Add the shortest completion
		completions.push_back(shortest_completion);
		// Reiterate to apply the completion
		i++; continue;
	  }

	  // Buffer of completions buffer
	  vector<string> completions_buf = vector<string>();
	  // Retrieve all completions that contain the next char
	  for (string comp : completions) {
		if (cur_attr[i] == comp[i])
		  completions_buf.push_back(comp);
	  }
	  completions = completions_buf;
	}
  }
}
