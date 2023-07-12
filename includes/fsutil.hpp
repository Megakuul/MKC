#ifndef FSUTIL_H
#define FSUTIL_H

#include <string>
#include <filesystem>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace fsutil {

  /**
   * Adds a File to the directory
   * @param directory Directory Path
   * @param name Filename
  */
  void AddFile(std::string directory, std::string name);

  /**
   * Adds a Directory to the directory
   * @param directory Directory Path
   * @param name Directory name
  */
  void AddDir(std::string directory, std::string name);

  /**
   * Initializes a Filewatcher
   * 
   * 
   * The function should be executed asynchron
   * 
   * To deallocate it, use the DeallocateWatcher with the associated state
   * @param directory Directory Path to watch
   * @param state State of the Watcher
   * @param state_mutex Mutex of the State Watcher
   * @param state_cv Conditional Variable of the State Watcher
   * @param on_create Function executed on create of a file (uses the filename as argument)
   * @param on_delete Function executed on deletion of a file (uses the filename as argument)
   * @param on_moved_away Function executed when a file gets moved_away (uses the filename as argument)
   * @param on_moved_in Function executed when a file gets moved_in (uses the filename as argument)
  */
  void InitWatcher(std::filesystem::path directory,
                   std::atomic<bool> &state,
                   std::mutex &state_mutex,
                   std::condition_variable &state_cv,
                   function<void(string)> on_create,
                   function<void(string)> on_delete,
                   function<void(string)> on_moved_away,
                   function<void(string)> on_moved_in);
  
  /**
   * Deallocates all Filewatchers that are associated with the state
   * 
   * 
   * The function will return if all watchers are deallocated, dont execute it asynchron
  */
  void DeallocateWatcher(std::atomic<bool> &state, std::mutex &state_mutex, std::condition_variable &state_cv);
}

#endif