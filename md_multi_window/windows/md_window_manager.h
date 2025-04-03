#ifndef MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_MANAGER_H_
#define MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_MANAGER_H_

#include <cstdint>
#include <string>
#include <map>
#include "md_window.h"


namespace md_multi_window {

class MdWindowManager : public std::enable_shared_from_this<MdWindowManager>, public MdWindowCallback{

 public:
  static MdWindowManager *Instance();

  MdWindowManager();

  std::string Create(std::string id, std::string args);
  void AddWindowAndNotifyAll(std::string id, HWND window_handle);

  flutter::EncodableList GetAllWindowIDs();

  // flutter::EncodableList GetAllWindowIDs();
  void OnWindowClose(std::string id) override;
  void OnWindowDestroy(std::string id) override;
 private:

  std::map<std::string, std::unique_ptr<MdWindow>> windows_;

};

}  // namespace md_multi_window

#endif //MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_MANAGER_H_