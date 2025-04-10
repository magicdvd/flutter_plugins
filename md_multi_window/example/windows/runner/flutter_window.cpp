#include "flutter_window.h"

#include <optional>
#include <iostream>

#include "flutter/generated_plugin_registrant.h"
#include "md_multi_window/md_multi_window_plugin_c_api.h"

FlutterWindow::FlutterWindow(const flutter::DartProject& project)
    : project_(project) {}

FlutterWindow::~FlutterWindow() {}

void FlutterWindow::SetWindowID(const std::string& id) {
  id_ = id;
}

bool FlutterWindow::OnCreate() {
  if (!Win32Window::OnCreate()) {
    return false;
  }
  std::cout << "fluterWindow OnCreate called" << std::endl;
  RECT frame = GetClientArea();

  // The size here must match the window dimensions to avoid unnecessary surface
  // creation / destruction in the startup path.
  flutter_controller_ = std::make_unique<flutter::FlutterViewController>(
      frame.right - frame.left, frame.bottom - frame.top, project_);
  // Ensure that basic setup of the controller was successful.
  if (!flutter_controller_->engine() || !flutter_controller_->view()) {
    return false;
  }
  RegisterPlugins(flutter_controller_->engine());
  SetChildContent(flutter_controller_->view()->GetNativeWindow());

  MdMultiWindowPluginCApiRegister(id_,this->GetHandle(),flutter_controller_->engine(), shared_from_this());

  flutter_controller_->engine()->SetNextFrameCallback([&]() {
    this->Show();
  });

  // Flutter can complete the first frame before the "show window" callback is
  // registered. The following call ensures a frame is pending to ensure the
  // window is shown. It is a no-op if the first frame hasn't completed yet.
  flutter_controller_->ForceRedraw();

  return true;
}

void FlutterWindow::OnDestroy() {
  if (flutter_controller_) {
    flutter_controller_ = nullptr;
  }
  std::cout << "fluterWindow OnDestroy called" << std::endl;
  Win32Window::OnDestroy();
}

LRESULT
FlutterWindow::MessageHandler(HWND hwnd, UINT const message,
                              WPARAM const wparam,
                              LPARAM const lparam) noexcept {

  if (message == WM_CLOSE) {
    std::cout << "fwin mh wm_close" << std::endl;
  }
  // Give Flutter, including plugins, an opportunity to handle window messages.
  if (flutter_controller_) {
    std::optional<LRESULT> result =
        flutter_controller_->HandleTopLevelWindowProc(hwnd, message, wparam,
                                                      lparam);
    if (result) {
      return *result;
    }
  }

  switch (message) {
    case WM_FONTCHANGE:
      flutter_controller_->engine()->ReloadSystemFonts();
      break;
  }

  if (MdMultiWindowPluginCApiHandleMessagee(id_, message)) {
    return 0;
  }
  if (message == WM_DESTROY) {
    std::cout << "fwin mh wm_destroy" << std::endl;
    return 0;
  }
  return Win32Window::MessageHandler(hwnd, message, wparam, lparam);
}
