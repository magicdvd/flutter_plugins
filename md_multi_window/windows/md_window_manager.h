#ifndef MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_MANAGER_H_
#define MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_MANAGER_H_

#include <cstdint>
#include <string>
#include <map>
#include "include/md_multi_window/md_multi_window_plugin_c_api.h"
#include "md_window.h"


namespace md_multi_window {

std::wstring String2WString(const std::string &string);

class MdWindowManager{

 public:
  static MdMultiWindowPluginCreateWindowCallback g_create_window_callback;
  // singleton
  static MdWindowManager *Instance();
  MdWindowManager();

  // create new window
  std::string CreateWindowAndRegister(const std::string& args);

  std::string RegisterWindow(
    const std::string& id,
    HWND window_handle,
    std::shared_ptr<FlutterWindow> window,
    std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel
  );

  flutter::EncodableList GetAllWindowIDs();

  std::vector<MdWindow*> GetAllWindows();

  MdWindow* GetWindow(const std::string& id);

  bool HandleMesssage(const std::string& id, UINT message,  WPARAM const wparam,
    LPARAM const lparam);

 private:
  void AddWindowAndNotifyAll(const std::string& id, std::unique_ptr<MdWindow> window);
  std::map<std::string, std::unique_ptr<MdWindow>> windows_;

};

}  // namespace md_multi_window

#endif //MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_MANAGER_H_