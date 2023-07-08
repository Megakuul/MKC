#ifndef FSUTIL_H
#define FSUTIL_H

#include <string>

namespace fsutil {

  void AddFile(std::string location, std::string name);

  void AddDir(std::string location, std::string name);
}

#endif