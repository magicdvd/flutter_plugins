#include "md_window.h"

#include "flutter_windows.h"
#include <flutter/plugin_registrar_windows.h>

#include "tchar.h"

#include <iostream>
#include <utility>

#include "include/md_multi_window/md_multi_window_plugin_c_api.h"
#include "md_multi_window_plugin.h"

namespace md_multi_window {

// WindowCreatedCallback _g_window_created_callback = nullptr;

TCHAR kMdWindowClassName[] = _T("MdMultiWindow");

int32_t class_registered_ = 0;

void RegisterWindowClass(WNDPROC wnd_proc) {
  if (class_registered_ == 0) {
    WNDCLASS window_class{};
    window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
    window_class.lpszClassName = kMdWindowClassName;
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = GetModuleHandle(nullptr);
    window_class.hIcon =
        LoadIcon(window_class.hInstance, IDI_APPLICATION);
    window_class.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    window_class.lpszMenuName = nullptr;
    // window_class.lpfnWndProc = wnd_proc;
    RegisterClass(&window_class);
  }
  class_registered_++;
}

void UnregisterWindowClass() {
  class_registered_--;
  if (class_registered_ != 0) {
    return;
  }
  UnregisterClass(kMdWindowClassName, nullptr);
}

// Scale helper to convert logical scaler values to physical using passed in
// scale factor
inline int Scale(int source, double scale_factor) {
  return static_cast<int>(source * scale_factor);
}

using EnableNonClientDpiScaling = BOOL __stdcall(HWND hwnd);

// Dynamically loads the |EnableNonClientDpiScaling| from the User32 module.
// This API is only needed for PerMonitor V1 awareness mode.
void EnableFullDpiSupportIfAvailable(HWND hwnd) {
  HMODULE user32_module = LoadLibraryA("User32.dll");
  if (!user32_module) {
    return;
  }
  auto enable_non_client_dpi_scaling =
      reinterpret_cast<EnableNonClientDpiScaling *>(
          GetProcAddress(user32_module, "EnableNonClientDpiScaling"));
  if (enable_non_client_dpi_scaling != nullptr) {
    enable_non_client_dpi_scaling(hwnd);
    FreeLibrary(user32_module);
  }
}



MdWindow::MdWindow(std::string id, HWND handle,const std::shared_ptr<MdWindowCallback> &callback) : callback_(callback), id_(id), window_handle_(handle), scale_factor_(1) {

}

MdWindow::MdWindow(
    std::string id,
    std::string args,
    const std::shared_ptr<MdWindowCallback> &callback
) : callback_(callback), id_(id), window_handle_(nullptr), scale_factor_(1) {
  // RegisterWindowClass(MdWindow::WndProc);

  // const POINT target_point = {static_cast<LONG>(10),
  //                             static_cast<LONG>(10)};
  // HMONITOR monitor = MonitorFromPoint(target_point, MONITOR_DEFAULTTONEAREST);
  // UINT dpi = FlutterDesktopGetDpiForMonitor(monitor);
  // scale_factor_ = dpi / 96.0;

  // HWND window_handle = CreateWindow(
  //     kMdWindowClassName, L"", WS_OVERLAPPEDWINDOW,
  //     Scale(target_point.x, scale_factor_), Scale(target_point.y, scale_factor_),
  //     Scale(1280, scale_factor_), Scale(720, scale_factor_),
  //     nullptr, nullptr, GetModuleHandle(nullptr), this);

  // RECT frame;
  // GetClientRect(window_handle, &frame);
  // flutter::DartProject project(L"data");
  // project.set_dart_entrypoint_arguments({"md_multi_window", id, std::move(args)});
  // flutter_controller_ = std::make_unique<flutter::FlutterViewController>(
  //     frame.right - frame.left, frame.bottom - frame.top, project);
  // // Ensure that basic setup of the controller was successful.
  // if (!flutter_controller_->engine() || !flutter_controller_->view()) {
  //   std::cerr << "Failed to setup FlutterViewController." << std::endl;
  // }
  // auto view_handle = flutter_controller_->view()->GetNativeWindow();
  // SetParent(view_handle, window_handle);
  // MoveWindow(view_handle, 0, 0, frame.right - frame.left, frame.bottom - frame.top, true);

  // auto registrar = flutter_controller_->engine()->GetRegistrarForPlugin("MdMultiWindowPluginCApi");
  // auto window_registrar = flutter::PluginRegistrarManager::GetInstance()
  //     ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar);
  // MdMultiWindowPlugin::AttachChannel(window_registrar);

  // if (_g_window_created_callback) {
  //   _g_window_created_callback(flutter_controller_.get());
  // }

  // // hide the window when created.
  // ShowWindow(window_handle, SW_HIDE);

}

void MdWindow::Destroy() {
  if (flutter_controller_) {
    flutter_controller_ = nullptr;
  }
  if (window_handle_) {
    DestroyWindow(window_handle_);
    window_handle_ = nullptr;
  }
}

MdWindow::~MdWindow() {
  if (window_handle_) {
    std::cout << "window_handle leak." << std::endl;
  }
  UnregisterWindowClass();
}

}  // namespace md_multi_window

// void MdMultiWindowSetWindowCreatedCallback(WindowCreatedCallback callback) {
//   md_multi_window::_g_window_created_callback = callback;
// }