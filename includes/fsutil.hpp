#ifndef FSUTIL_H
#define FSUTIL_H

#include <string>
#include <filesystem>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>

/**
 * Functions that directly interact with the filesystem or other low level APIs
 * 
 * 
 * For tasks that do not require a lot of power the crossplatform filesystems API is used.
 * The heavy lifting is done by linux sys librarys.
*/
namespace fsutil {

  /**
   * Represents a File with the properties shown in the UI
  */
  struct File {
    std::string name;
    std::string type;
    int hardlink;
    int size;
    std::string access;
    time_t lastEdited;
  };

  /**
   * Operation to do
   * ERROR = Function will just throw an error if multiple files conflict
   * DELETE = Duplicated files are deleted
   * TRASH = Duplicated files are moved to the trash
  */
  enum OP {
    ERROR = 0,
    DELETE = 1,
    TRASH = 2,
	RENAME = 3,
  };

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
   * Puts an object into the mkc trash (and compresses it)
   * @param source Object path
  */
  void TrashObject(const std::string source);

  /**
   * Puts a compressed object back into its original form
   * @param source Object path
  */
  void RecoverObject(const std::string source);

  /**
   * Recursively deletes every file or directory in the names vector contained in the directory path
   * @param directory Directory Path
   * @param names Names of the Objects
  */
  void DeleteObjects(std::string directory, std::vector<std::string> names, OP operation);

  /**
   * Removes a file from the current location with the specified operation
  */
  void CleanObject(std::string file, OP operation);


  /**
   * Copies an object
   * @param source source path
   * @param destination destination path
  */
  void CopyObject(std::string source, std::string destination, OP operation=OP::ERROR);


  /**
   * Moves an object
   * @param source source path
   * @param destination destination path
  */
  void MoveObject(std::string source, std::string destination, OP operation=OP::ERROR);
  
  void GetFilesFromDirectory(const std::string &location, std::vector<File> &files);

  /**
   * Constructs a File struct out of a file
   * 
   * 
   * This function uses the sys/stats library to directly access the files without overhead,
   * it is only working on Linux
   * @param location Path of the file
  */
  void GetFileInformation(const std::string &location, File &file);

  /**
   * Initializes a Filewatcher
   * 
   * 
   * This function uses the sys/inotify library for initializing a file watcher in the kernel,
   * it is only working on Linux
   * 
   * 
   * The function should be executed asynchron
   * 
   * To deallocate it, use the DeallocateWatcher with the associated state
   * @param directory Directory Path to watch
   * @param state The State of the Watcher that needs to be allocated
   * @param state_mutex Mutex associated with the provided State
   * @param state_cv Conditional Variable associated with the provided State
   * @param on_create Function executed on create of a file (uses the filename as argument)
   * @param on_delete Function executed on deletion of a file (uses the filename as argument)
   * @param on_moved_away Function executed when a file gets moved_away (uses the filename as argument)
   * @param on_moved_in Function executed when a file gets moved_in (uses the filename as argument)
  */
  void InitWatcher(std::filesystem::path directory,
                   std::atomic<bool> &state,
                   std::mutex &state_mutex,
                   std::condition_variable &state_cv,
                   std::function<void(std::string)> on_create,
                   std::function<void(std::string)> on_delete,
                   std::function<void(std::string)> on_moved_in,
                   std::function<void(std::string)> on_moved_away);
  
  /**
   * Deallocates all Filewatchers that are associated with the state
   * 
   * 
   * This function uses the sys/inotify library for initializing a file watcher in the kernel,
   * it is only working on Linux
   * 
   * 
   * The function will return if all watchers are deallocated, dont execute it asynchron
   * @param state The State of the Watcher that needs to be deallocated
   * @param state_mutex Mutex associated with the provided State
   * @param state_cv Conditional Variable associated with the provided State
  */
  void DeallocateWatcher(std::atomic<bool> &state, std::mutex &state_mutex, std::condition_variable &state_cv);
}

#endif
