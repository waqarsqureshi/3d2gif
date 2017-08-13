#include <iostream>

class filePath {
public:
  std::string path, name, name_without_ext, ext;
  filePath(std::string input_file){
    std::size_t found = input_file.find_last_of("/\\");
    path = input_file.substr(0,found+1);
    name = input_file.substr(found+1);
    std::size_t found_ext = name.find_last_of(".");
    name_without_ext = name.substr(0,found_ext);
    ext = name.substr(found_ext+1,name.length());
  }
};
