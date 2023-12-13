#ifndef FSUTIL_H
#define FSUTIL_H

#include <string>
#include <filesystem>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <sys/types.h>

// Location of the trash relative to $HOME
#define TRASH_PATH_REL ".mkc/trash"

/**
 * Functions that directly interact with the filesystem or other low level APIs
 * 
 * 
 * For tasks that do not require a lot of power the crossplatform filesystems API is used.
 * The heavy lifting is done by linux sys librarys.
 *
 * fsutil functions / operations do not catch anything and throw runtime_errors() on errors
*/
namespace fsutil {

  /**
   * Represents a File with the properties shown in the UI
  */
  struct File {
    std::string name;
    std::string type;
    int hardlink;
    off_t size;
    std::string access;
	uid_t owner_uid;
	gid_t owner_gid;
    time_t lastEdited;
  };

  /**
   * Represents a File with the properties that can be manipulated by the user
  */
  struct FileMod {
	std::string access;
	std::string owner_user;
	std::string owner_group;
  };

  /**
   * Operation to do
   * ERROR = Function will just throw an error if multiple files conflict
   * DELETE = Duplicated files are deleted
   * TRASH = Duplicated files are moved to the trash
  */
  enum OP {
    NONE = 0,
	SKIP = 1,
    DELETE = 2,
    TRASH = 3,
	RENAME = 4,
	ERROR = 5
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
   * Modifies attributes of a file specified in the FileMod
   *
   * If modification fields are left "", they are not modified
   * @param source Object source path
   * @param modifications Mods to apply to the object
  */
  void ModifyObject(std::string source, FileMod modifications);

  /**
   * Moves a object to the trash and generates a meta file to recover it
   * @param source Object source path
  */
  void TrashObject(const std::string source);

  /**
   * Moves a object from the trash to its original position (and generates the directories if necessary)
   * @param source Object path
   * @param operation Operation executed when files conflict
  */
  void RecoverObject(const std::string source, OP operation=OP::ERROR);

  /**
   * Removes a file or directory (recursively) from the current location with the specified operation
   * @param source Object source path
   * @param operation Operation executed when files conflict
  */
  void CleanObject(std::string source, OP operation);


  /**
   * Copies an object
   * @param source source path
   * @param destination destination path
   * @param operation Operation executed when files conflict
  */
  void CopyObject(std::string source, std::string destination, OP operation=OP::ERROR);


  /**
   * Moves an object
   * @param source source path
   * @param destination destination path
   * @param operation Operation executed when files conflict
  */
  void MoveObject(std::string source, std::string destination, OP operation=OP::ERROR);

  /**
   * Writes all files from a directory to the files vector
   * @param location Directory to list
   * @param files Reference vector that will be filled up with File structs
  */
  void GetFilesFromDirectory(const std::string &location, std::vector<File> &files);

  /**
   * Constructs a File struct out of a file
   * 
   * 
   * This function uses the sys/stats library to directly access the files without overhead,
   * it is only working on Linux
   * @param location Path of the file
   * @param files Reference to File struct that will be filled up
  */
  void GetFileInformation(const std::string &location, File &file);

  /**
   * Fetches the username and groupname from uid/gid provided in the file
   * and parses it in the common format 'username:groupname'
   *
   * @param file File struct to read the owner from
   * @param owner Reference to string that will be filled up
  */
  void GetFileOwner(File &file, std::string &owner);

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
                   std::function<void(std::string)> on_moved_away,
				   std::function<void(std::string)> on_changed);
  
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
