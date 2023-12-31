// This File contains utility functions to directly manipulate the filesystem

#include <cstdlib>
#include <exception>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <sys/inotify.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "fsutil.hpp"

using namespace std;
namespace fs = filesystem;

namespace fsutil {
  void AddFile(string directory, string name) {
    fs::path path_buf = directory;
    path_buf.append(name);

    ofstream out(path_buf.c_str());
	
	out.close();
  }

  void AddDir(string directory, string name) {
    fs::path path_buf = directory;
    path_buf.append(name);

    fs::create_directories(path_buf);
  }

  void ModifyObject(string source, FileMod modifications) {
	if (!fs::exists(source)) {
	  throw runtime_error("Failed to modify object: Object not found!");
	}
	
	if (!modifications.access.empty()) {
	  size_t octal_permission = stoi(modifications.access, nullptr, 8);
	  mode_t mode = static_cast<mode_t>(octal_permission);

	  if (chmod(source.c_str(), mode) == -1) {
		throw runtime_error("Failed to modify object: Cannot modify access!");
	  }
	}

	if (!modifications.owner_user.empty() || !modifications.owner_group.empty()) {
	   
	  struct passwd *pw = getpwnam(modifications.owner_user.c_str());
	  if (!pw) {
		throw runtime_error("Failed to modify object: User not found!");
	  }
	  struct group *gr = getgrnam(modifications.owner_group.c_str());
	  if (!gr) {
		throw runtime_error("Failed to modify object: Group not found!");
	  }

	  if (chown(source.c_str(), pw->pw_uid, gr->gr_gid) == -1) {
		throw runtime_error("Failed to modify object: Cannot modify owner!");
	  }
	}
  }
  
  void CleanObject(string source, OP operation) {
	if (operation==OP::NONE||operation==OP::SKIP||operation==OP::ERROR) {
	  return;
	}
	if (fs::exists(source)) {
	  if (operation == OP::RENAME) {
		size_t version = 1;
		fs::path path(source);

		string path_base = path.parent_path() / path.stem();
		string path_ext = path.extension();

		fs::path path_buf;

		do {
		  ++version;
		  path_buf = path_base + ("_" + to_string(version));
		  path_buf.replace_extension(path_ext);
		} while (fs::exists(path_buf));

		fs::rename(path, path_buf);
	  }
	  if (operation == OP::TRASH) {
		TrashObject(source);
	  } else {
		fs::remove_all(source);
	  }
	}
  }

  void TrashObject(const string file) {
    // Get and create the trash_path if it does not exist already
    fs::path trash_path = fs::path(getenv("HOME")) / TRASH_PATH_REL;
    if (!fs::exists(trash_path)) {
      fs::create_directories(trash_path);
    }
    // Capture the source_path
    fs::path src_path(file);

    // Define the new filename in a format like {trash_path}{filename}{currenttime}
    trash_path.append(src_path.filename().string());
    time_t now = time(nullptr);
    trash_path.concat(to_string(now) + ".mkc");

    // Set metafile that contains the source_path
    ofstream out_meta(trash_path.string() + ".meta");
    out_meta << src_path << '\n';

	fs::rename(src_path, trash_path);
  }

  void RecoverObject(const string source, OP operation) {
    // Generating required paths
    fs::path src_path(source);
    fs::path dest_path;
    fs::path meta_path(source);
    // Check if the meta file exists
    meta_path.concat(".meta");
    if (fs::exists(meta_path)) {
      /*
        If a meta file is in place,
        the file should contain the src as first line
      */
      ifstream in_meta(meta_path);
      string dest_str;
      getline(in_meta, dest_str);

      // This will remove the '"' ticks if they are present
      if (dest_str.size() >= 2 && dest_str.front() == '"' && dest_str.back() == '"') {
        dest_str = dest_str.substr(1, dest_str.size() - 2);
      }
      dest_path = fs::path(dest_str);
    } else throw runtime_error("Failed to recover object: Corresponding .meta file not found!");

	// Recreate path if it is not existing
	fs::create_directories(dest_path.parent_path());

	MoveObject(src_path, dest_path, operation);
	
	CleanObject(meta_path, OP::DELETE);
  }

  void CopyObject(string source, string destination, OP operation) {
	if (operation==fsutil::NONE) return;
	if (source==destination) return;
    CleanObject(destination, operation);
	if (operation==fsutil::SKIP && (!fs::exists(source) || fs::exists(destination))) {
	  return;
	}
    fs::copy(source, destination);
  }
 
  void MoveObject(string source, string destination, OP operation) {
	if (operation==fsutil::NONE) return;
	if (source==destination) return;
	CleanObject(destination, operation);
	if (operation==fsutil::SKIP && (!fs::exists(source) || fs::exists(destination))) {
	  return;
	}
    fs::rename(source, destination);
  }

  void GetFilesFromDirectory(const string &location, vector<File> &files) {
    DIR* dir = opendir(location.c_str());
    if (dir == nullptr) {
      throw runtime_error("Failed to list files: Cannot read content of\n - " + location);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
      struct File file_buf;

	  fs::path path_buf(location);
	  path_buf.append(entry->d_name);

	  GetFileInformation(path_buf, file_buf);

      files.push_back(file_buf);
    }
  }

  void GetFileInformation(const string &location, File &file) {
    fs::path path_buf(location);
    struct stat stat_buf;
    int res = stat(location.c_str(), &stat_buf);
    if (res != 0) {
      file = {
        path_buf.filename().c_str(), "e", 0, 0, "", 0
      };
      return; 
    }
    
    file.name = path_buf.filename().c_str();

    if (S_ISREG(stat_buf.st_mode))
      file.type = "f";
    else if (S_ISDIR(stat_buf.st_mode))
      file.type = "d";
    else if (S_ISLNK(stat_buf.st_mode))
      file.type = "s";
    else
      file.type = "o";
      
    file.hardlink = stat_buf.st_nlink;
    file.size = stat_buf.st_size;
    {
      int user = (stat_buf.st_mode & S_IRWXU) >> 6;
      int group = (stat_buf.st_mode & S_IRWXG) >> 3;
      int other = stat_buf.st_mode & S_IRWXO;
      file.access = to_string(user * 100 + group * 10 + other);
    }
	file.owner_uid = stat_buf.st_uid;
	file.owner_gid = stat_buf.st_gid;
    file.lastEdited = stat_buf.st_mtime;
  }

  void GetFileOwner(File &file, string &owner) {
	struct passwd *pw = getpwuid(file.owner_uid);
	if (!pw) {
	  throw runtime_error("Failed to get owner: UID not found!");
	}
	
	struct group *gr = getgrgid(file.owner_gid);
	if (!gr) {
	  throw runtime_error("Failed to get owner: GID not found!");
	}

	owner = string(pw->pw_name) + ":" + string(gr->gr_name);
  }

  void DeallocateWatcher(atomic<bool> &state, mutex &state_mutex, condition_variable &state_cv) {
    // if the State is set to true (blocked) it will deblock it
    // this is the case if something fails or if the deallocation happens when nothing is allocated
    if (state) {
      {
        lock_guard<mutex> lock(state_mutex);
        state = false;
      }
      return;
    }
    unique_lock<mutex> lock(state_mutex);
    // Setting the state to true this will block all loops
    state = true;
    state_cv.wait(lock, [&state]{ 
      return state == false; 
    });
  }

  void InitWatcher(fs::path directory,
                   atomic<bool> &state,
                   mutex &state_mutex,
                   condition_variable &state_cv,
                   function<void(string)> on_create,
                   function<void(string)> on_delete,
                   function<void(string)> on_moved_in,
                   function<void(string)> on_moved_away,
				   function<void(string)> on_changed) {
    int length, i = 0;
    int filedescriptor;
    int watchdescriptor;
    char buffer[1024];

    filedescriptor = inotify_init();
    if (filedescriptor < 0) {
      throw runtime_error("Failed to init Filewatcher");
    }

    watchdescriptor =
	  inotify_add_watch(filedescriptor, directory.c_str(),
						IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO | IN_MODIFY | IN_ATTRIB);

    while (!state) {
      i = 0;

      fd_set fds;
      FD_ZERO(&fds);
      FD_SET(filedescriptor, &fds);
      struct timeval timeout = {0,10};
      int ret = select(filedescriptor + 1, &fds, nullptr, nullptr, &timeout);

      if (ret > 0 && FD_ISSET(filedescriptor, &fds)) {
        length = read(filedescriptor, buffer, 1024);
        while (i < length && !state) {
          struct inotify_event *event = (struct inotify_event *) &buffer[i];
          if (event->len) {
            if (event->mask & IN_CREATE)
              on_create(event->name);
            else if (event->mask & IN_DELETE)
              on_delete(event->name);
            else if (event->mask & IN_MOVED_FROM)
              on_moved_away(event->name);
            else if (event->mask & IN_MOVED_TO)
              on_moved_in(event->name);
			else if (event->mask & IN_ALL_EVENTS)
			  on_changed(event->name);
          }
          i += sizeof(struct inotify_event) + event->len;
        }
      }
    }

    inotify_rm_watch(filedescriptor, watchdescriptor);
    close(filedescriptor);
    {
      lock_guard<mutex> lock(state_mutex);
      state = false;
    }
    state_cv.notify_one();
  }
}
