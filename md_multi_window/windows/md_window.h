#ifndef MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_H_
#define MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_H_

#include <windows.h>

#include <flutter/flutter_view_controller.h>
#include "include/md_multi_window/md_multi_window_plugin_c_api.h"
#include <cstdint>
#include <memory>
#include <string>

#include "flutter/encodable_value.h"

namespace md_multi_window {

class MdWindowCallback {

  public:
    virtual void OnWindowClose(std::string id) = 0;
  
    virtual void OnWindowDestroy(std::string id) = 0;
  
  };

class MdWindow {

 public:
  MdWindow(std::string id, std::shared_ptr<FlutterWindow>, const std::shared_ptr<MdWindowCallback> &callback);
  ~MdWindow();

 private:

  std::shared_ptr<FlutterWindow> window_;

  std::weak_ptr<MdWindowCallback> callback_;

  std::string id_;

  void Destroy();
};

}  // namespace md_multi_window

#endif //MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_H_