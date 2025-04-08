#include "md_window.h"

#include "flutter_windows.h"
#include <flutter/plugin_registrar_windows.h>

#include "tchar.h"

#include <iostream>
#include <utility>


#include "md_multi_window_plugin.h"

namespace md_multi_window {

MdWindow::MdWindow(
    const std::string& id,
    std::shared_ptr<FlutterWindow> window,
    HWND window_handle,
    std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel,
    const std::shared_ptr<MdWindowCallback> &callback
) : callback_(callback), id_(id), window_(window), channel_(std::move(channel)),window_handle_(window_handle) {

}
void MdWindow::Destroy() {

}

MdWindow::~MdWindow() {
}
}  // namespace md_multi_window
