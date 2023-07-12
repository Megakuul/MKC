// This File contains utility functions to directly manipulate the filesystem

#include <fsutil.hpp>
#include <iostream>
#include <filesystem>
#include <sys/inotify.h>
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

  // TODO: DeallocateWatcher is not returning if no Watcher is running
  void DeallocateWatcher(atomic<bool> &state, mutex &state_mutex, condition_variable &state_cv) {
    unique_lock<mutex> lock(state_mutex);
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
      length = read(filedescriptor, buffer, 1024);
      while (i < length && !state) {
        struct inotify_event *event = (struct inotify_event *) &buffer[i];
        if (event->len) {
          if (event->mask & IN_CREATE) {
            on_create(event->name);
            cout << "The file " << event->name << " was created." << endl;
          } else if (event->mask & IN_DELETE) {
            on_delete(event->name);
            cout << "The file " << event->name << " was deleted." << endl;
          } else if (event->mask & IN_MOVED_FROM) {
            on_moved_in(event->name);
            cout << "The file " << event->name << " was moved from this directory." << endl;
          } else if (event->mask & IN_MOVED_TO) {
            on_moved_away(event->name);
            cout << "The file " << event->name << " was moved to this directory." << endl;
          }
        }
        i += sizeof(struct inotify_event) + event->len;
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

