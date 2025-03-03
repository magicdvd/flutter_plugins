//
// Created by yangbin on 2022/1/11.
//

#ifndef MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_MANAGER_H_
#define MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_MANAGER_H_

#include <cstdint>
#include <string>
#include <map>
#include "md_window.h"

class MdWindowManager : public std::enable_shared_from_this<MdWindowManager> {

 public:
  static MdWindowManager *Instance();

  MdWindowManager();

  std:string CreateWindow(std::string id, std::string args);

  void AddWindowAndNotifyAll(std::string id, HWND window_handle);

  flutter::EncodableList GetAllWindowIDs();

 private:

  std::map<int64_t, std::unique_ptr<MdWindow>> windows_;

};

#endif //MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_MANAGER_H_
