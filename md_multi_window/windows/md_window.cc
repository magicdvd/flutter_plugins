#include "md_window.h"

#include "flutter_windows.h"
#include <flutter/plugin_registrar_windows.h>

#include "tchar.h"

#include <iostream>
#include <utility>
#include "md_debug.h"

#include "md_multi_window_plugin.h"
#include "md_call_arguments.h"

namespace md_multi_window {

MdWindow::MdWindow(
    const std::string& id,
    HWND window_handle,
    std::shared_ptr<FlutterWindow> window,
    std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel
) : id_(id), window_handle_(window_handle), window_(window), channel_(std::move(channel)),
shouldClose_(true), preventCloseForceClose_(false), preventCloseProcessing_(false){

}

void MdWindow::Destroy() {
  if (window_handle_) {
    window_handle_ = nullptr;
  }
  if (window_) {
    window_.reset();
  }
  if (channel_) {
    channel_ = nullptr;
  }
  DEBUG_LOG("mdwindow destroyed " << window_.use_count());
}

MdWindow::~MdWindow() {
  Destroy();
}

void MdWindow::Close(){
  shouldClose_ = true;
  auto handle = window_handle_;
  if (IsWindow(handle)) {
    PostMessage(handle, WM_CLOSE, 0, 0);
  }else{
    std::cerr << "close window window is not window " << handle << std::endl;
  }
}

void MdWindow::PerformClose(){
  shouldClose_ = true;
  auto handle = window_handle_;
  if (handle) {
    PostMessage(handle, WM_CLOSE, 0, 0);
  }
}

void MdWindow::PreventClose(bool yesOrNo){
  shouldClose_ = !yesOrNo;
}

void MdWindow::PreventCloseEnd(bool yesOrNo){
  preventCloseProcessing_ = false;
  if (yesOrNo) {
    preventCloseForceClose_ = true;
    this->Close();
    return;
  }
}

void MdWindow::Show(){
  ShowWindow(window_handle_, SW_SHOWNORMAL);
}

void MdWindow:: Hide(){
  SendToFlutter("onHide");
  ShowWindow(window_handle_, SW_HIDE);
}

void MdWindow::Center(){
  RECT rc;
  GetWindowRect(window_handle_, &rc);
  MdWindowStyle style;
  POINT pt = style.GetCenterOrigin({rc.right-rc.left, rc.bottom - rc.top});
  SetWindowPos(window_handle_, nullptr, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void MdWindow::SetFrame(POINT pt, SIZE size){
  MoveWindow(window_handle_, pt.x, pt.y,
  size.cx,
  size.cy,
  TRUE);
}

void MdWindow::SetTitle(std::wstring title){
  SetWindowText(window_handle_, title.c_str());
}

void MdWindow::SendData(
    std::unique_ptr<flutter::EncodableValue> data,  
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
    channel_->InvokeMethod("recieveData", std::move(data), std::move(result));
}

void MdWindow::NotifyFlutter(
    const std::string& name,
    const std::string& from_window_id,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
    channel_->InvokeMethod(name, std::make_unique<flutter::EncodableValue>(flutter::EncodableValue(from_window_id)), std::move(result));
}

void MdWindow::SendToFlutter(
  const std::string& name,
  std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  channel_->InvokeMethod(name, std::make_unique<flutter::EncodableValue>(flutter::EncodableValue(id_)), std::move(result));
}

bool MdWindow::DoCloseCheck(){
  DEBUG_LOG("preventCloseProcessing:" << preventCloseProcessing_ << " preventCloseForceClose:" << preventCloseForceClose_ << " shouldClose:" << shouldClose_);
  if (preventCloseProcessing_) {
    return false;
  }
  if (!preventCloseForceClose_){
    if (!shouldClose_) {
      preventCloseProcessing_ = true;
      this->SendToFlutter("onShouldClose");
      return false;
    }
  }
  preventCloseForceClose_ = false;
  preventCloseProcessing_ =  false;
  if (window_handle_) {
    DestroyWindow(window_handle_);
  }
  return true;
}

HWND  MdWindow::HANDLE(){
  return window_handle_;
}


}  // namespace md_multi_window
