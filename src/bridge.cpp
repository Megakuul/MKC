// This File contains wrappers that bridge the direct access to the filesystem (fsutil) and the frontend

#include <gtkmm.h>
#include <bridge.hpp>
#include <fsutil.hpp>
#include <Modal.hpp>
#include <Browser.hpp>
#include <string>
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

int perm_to_int(fs::perms p);
time_t get_time_from_filetime(const fs::file_time_type filetime);

namespace bridge {
  void wChangeDir(Browser* browser, string directory) {

    try {
      auto new_contents = fsutil::GetDirectoryContent(directory);

      browser->ClearElements();
      browser->CurrentPath = directory;
      
      for (const auto& entry : new_contents) {
        const auto& status = fs::status(entry);

        if (fs::is_regular_file(status)) {
          browser->AddElement(
            entry.path().filename(),
            "f",
            to_string(fs::hard_link_count(entry)),
            fs::file_size(entry),
            to_string(perm_to_int(status.permissions())),
            get_time_from_filetime(fs::last_write_time(entry))
          );
        } else if (fs::is_directory(entry)) {
          browser->AddElement(
            entry.path().filename(),
            "d",
            to_string(fs::hard_link_count(entry)),
            0,
            to_string(perm_to_int(status.permissions())),
            get_time_from_filetime(fs::last_write_time(entry))
          );
        } else {
          // TODO: Add the element but as invalid element or something
          cout << "Failed to read: " << entry.path() << endl;
        }
      }
    } catch (const fs::filesystem_error error) {
      // TODO: Implement an error Dialog
      cout<<error.what()<<endl;
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
      // TODO: Implement an error Dialog
      cout<<error.what()<<endl;
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
      // TODO: Implement an error Dialog
      cout<<error.what()<<endl;
    }
  }
}

int perm_to_int(fs::perms p) {
  int res = 0;
  // Get Owner access
  res += ((p & fs::perms::owner_read) != fs::perms::none)   ? 400 : 0;
  res += ((p & fs::perms::owner_write) != fs::perms::none)  ? 200 : 0;
  res += ((p & fs::perms::owner_exec) != fs::perms::none)   ? 100 : 0;
  // Get Group access
  res += ((p & fs::perms::group_read) != fs::perms::none)   ? 040 : 0;
  res += ((p & fs::perms::group_write) != fs::perms::none)  ? 020 : 0;
  res += ((p & fs::perms::group_exec) != fs::perms::none)   ? 010 : 0;
  // Get Others access
  res += ((p & fs::perms::others_read) != fs::perms::none)  ? 004 : 0;
  res += ((p & fs::perms::others_write) != fs::perms::none) ? 002 : 0;
  res += ((p & fs::perms::others_exec) != fs::perms::none)  ? 001 : 0;
  
  return res;
}

time_t get_time_from_filetime(const fs::file_time_type filetime) {
// This only works with C++ 20
#ifdef __GNUC__
  return chrono::system_clock::to_time_t(chrono::file_clock::to_sys(filetime));
#else
  return chrono::system_clock::to_time_t(
    chrono::file_clock::to_sys(chrono::file_clock::to_utc(filetime))
  );
#endif
}