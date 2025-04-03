#include "md_window.h"

#include "flutter_windows.h"
#include <flutter/plugin_registrar_windows.h>

#include "tchar.h"

#include <iostream>
#include <utility>


#include "md_multi_window_plugin.h"

namespace md_multi_window {

MdWindow::MdWindow(
    std::string id,
    std::shared_ptr<FlutterWindow> window,
    const std::shared_ptr<MdWindowCallback> &callback
) : callback_(callback), id_(id), window_(window) {

}
void MdWindow::Destroy() {

}

MdWindow::~MdWindow() {
}
}  // namespace md_multi_window
