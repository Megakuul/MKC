// This File contains utility functions to directly manipulate the filesystem

#include <fsutil.hpp>
#include <iostream>
#include <filesystem>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>

using namespace std;
namespace fs = filesystem;

namespace fsutil {
  void AddFile(string directory, string name) {
    fs::path fPath = directory;
    fPath.append(name);

    std::cout<<fs::absolute(fPath)<<endl;
  }

  void AddDir(string directory, string name) {
    fs::path dPath = directory;
    dPath.append(name);


    std::cout<<fs::absolute(dPath)<<endl;
  }

  File GetFileInformation(std::string location) {
    File file_buf{};
    fs::path path_buf(location);
    struct stat stat_buf;
    int res = stat(location.c_str(), &stat_buf);
    if (res != 0) {
      throw fs::filesystem_error("Failed to fetch stats", path_buf, make_error_code(errc::io_error));
    }

    if (S_ISREG(stat_buf.st_mode))
      file_buf.type = "f";
    else if (S_ISDIR(stat_buf.st_mode))
      file_buf.type = "d";
    else if (S_ISLNK(stat_buf.st_mode))
      file_buf.type = "s";
    else
      file_buf.type = "o";

    file_buf.name = path_buf.filename().c_str();
    file_buf.hardlink = stat_buf.st_nlink;
    file_buf.size = stat_buf.st_size;
    {
      int user = (stat_buf.st_mode & S_IRWXU) >> 6;
      int group = (stat_buf.st_mode & S_IRWXG) >> 3;
      int other = stat_buf.st_mode & S_IRWXO;
      file_buf.access = to_string(user * 100 + group * 10 + other);
    }
    file_buf.lastEdited = stat_buf.st_mtime;

    return file_buf;
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
                   function<void(string)> on_moved_away,
                   function<void(string)> on_moved_in
                  ) {
    int length, i = 0;
    int filedescriptor;
    int watchdescriptor;
    char buffer[1024];

    filedescriptor = inotify_init();
    if (filedescriptor < 0) {
      throw runtime_error("Failed to init Filewatcher");
    }

    watchdescriptor = inotify_add_watch(filedescriptor, directory.c_str(), IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);

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
              on_moved_in(event->name);
            else if (event->mask & IN_MOVED_TO)
              on_moved_away(event->name);
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