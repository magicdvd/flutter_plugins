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

  static WindowCreatedCallback window_callback;

  MdWindow(std::string id, std::string args, const std::shared_ptr<MdWindowCallback> &callback);
  MdWindow(std::string id, HWND handle, const std::shared_ptr<MdWindowCallback> &callback);
  ~MdWindow();

 protected:

  HWND GetWindowHandle() { return window_handle_; };

 private:

  std::weak_ptr<MdWindowCallback> callback_;

  std::string id_;

  HWND window_handle_;

  // The Flutter instance hosted by this window.
  std::unique_ptr<flutter::FlutterViewController> flutter_controller_;

  double scale_factor_;

  bool destroyed_ = false;

  //static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

  //static MdWindow *GetThisFromHandle(HWND window) noexcept;

  //LRESULT MessageHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

  void Destroy();
};

}  // namespace md_multi_window

#endif //MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_H_