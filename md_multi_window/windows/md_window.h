#ifndef MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_H_
#define MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_H_

#include <windows.h>

#include <flutter/flutter_view_controller.h>
#include "include/md_multi_window/md_multi_window_plugin_c_api.h"
#include <cstdint>
#include <memory>
#include <string>
#include "flutter/encodable_value.h"
#include "flutter/method_channel.h"

namespace md_multi_window {

class MdWindowCallback {

  public:
    virtual void OnWindowClose(const std::string& id) = 0;
    virtual void OnWindowDestroy(const std::string& id) = 0;
  };

class MdWindow {

 public:
 MdWindow::MdWindow(
  const std::string& id,
  std::shared_ptr<FlutterWindow> window,
  HWND window_handle,
  std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel,
  const std::shared_ptr<MdWindowCallback> &callback
  );
  ~MdWindow();

 private:

  std::shared_ptr<FlutterWindow> window_;

  std::weak_ptr<MdWindowCallback> callback_;

  std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel_;

  HWND window_handle_;

  std::string id_;

  void Destroy();
};

}  // namespace md_multi_window

#endif //MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_H_