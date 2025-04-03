#ifndef MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_MANAGER_H_
#define MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_MANAGER_H_

#include <cstdint>
#include <string>
#include <map>
#include "include/md_multi_window/md_multi_window_plugin_c_api.h"
#include "md_window.h"


namespace md_multi_window {

class MdWindowManager : public std::enable_shared_from_this<MdWindowManager>, public MdWindowCallback{

 public:
  static MdMultiWindowPluginCreateWindowCallback g_create_window_callback;
  // singleton
  static MdWindowManager *Instance();
  MdWindowManager();

  // create new window
  std::string CreateWindowAndRegister(std::string id, std::string args);

  std::string RegisterMainWindow(std::string id, std::shared_ptr<FlutterWindow> window);

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