#ifndef MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_H_
#define MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_H_

#include <Windows.h>

#include <flutter/flutter_view_controller.h>

#include <cstdint>
#include <memory>
#include <string>

class MdWindow {

 public:

  MdWindow(std::string id, std::string args);
  MdWindow(std::string id, HWND handle);
  ~MdWindow() override;

 protected:

  HWND GetWindowHandle() override { return window_handle_; }

 private:

  HWND window_handle_;

  std:string id_;

  // The Flutter instance hosted by this window.
  std::unique_ptr<flutter::FlutterViewController> flutter_controller_;

  double scale_factor_;

  bool destroyed_ = false;

  static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

  static MdWindow *GetThisFromHandle(HWND window) noexcept;

  LRESULT MessageHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

  void Destroy();
};

#endif //MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_H_
