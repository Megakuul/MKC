#ifndef FSUTIL_H
#define FSUTIL_H

#include <string>
#include <filesystem>

namespace fsutil {

  std::filesystem::directory_iterator GetDirectoryContent(std::string location);

  void AddFile(std::string location, std::string name);

  void AddDir(std::string location, std::string name);
}

#endif