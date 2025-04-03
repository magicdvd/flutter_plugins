#ifndef MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_MANAGER_H_
#define MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_MANAGER_H_

#include <cstdint>
#include <string>
#include <map>
#include "md_window.h"


namespace md_multi_window {

class MdWindowManager : public std::enable_shared_from_this<MdWindowManager>, public MdWindowCallback{

 public:
  // singleton
  static MdWindowManager *Instance();
  MdWindowManager();

  // create new window
  std::string Create(std::string id, std::string args);

  std::string CreateWithMain(std::string id, HWND window_handle);

  flutter::EncodableList GetAllWindowIDs();

  // callback used
  void OnWindowClose(std::string id) override;
  void OnWindowDestroy(std::string id) override;
 private:
  void AddWindowAndNotifyAll(std::string id, std::unique_ptr<MdWindow> window);
  std::map<std::string, std::unique_ptr<MdWindow>> windows_;

};

}  // namespace md_multi_window

#endif //MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_MANAGER_H_