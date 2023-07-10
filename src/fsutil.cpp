// This File contains utility functions to directly manipulate the filesystem

#include <fsutil.hpp>
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

namespace fsutil {

  fs::directory_iterator GetDirectoryContent(string location) {
    fs::path dirpath(location);

    return fs::directory_iterator(dirpath);
  }

  void AddFile(string location, string name) {
    fs::path fPath = location;
    fPath.append(name);

    std::cout<<fs::absolute(fPath)<<endl;
  }

  void AddDir(string location, string name) {
    fs::path dPath = location;
    dPath.append(name);


    std::cout<<fs::absolute(dPath)<<endl;
  }
}

