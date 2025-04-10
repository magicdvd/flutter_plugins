#include "md_window.h"

#include "flutter_windows.h"
#include <flutter/plugin_registrar_windows.h>

#include "tchar.h"

#include <iostream>
#include <utility>


#include "md_multi_window_plugin.h"

namespace md_multi_window {

  // bool shouldClose_;

  // bool preventCloseForceClose_;

  // bool preventCloseProcessing_;

  // bool canBeShown_;
MdWindow::MdWindow(
    const std::string& id,
    HWND window_handle,
    std::shared_ptr<FlutterWindow> window,
    std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel,
    const std::shared_ptr<MdWindowCallback> &callback
) : callback_(callback), id_(id), window_handle_(window_handle), window_(window), channel_(std::move(channel)),
shouldClose_(true), preventCloseForceClose_(false), preventCloseProcessing_(false),canBeShown_(true),destroyed_(false) {

}

void MdWindow::Destroy() {
  if (channel_) {
    channel_ = nullptr;
  }
  std::cout << "winodow destroyed" << std::endl;
}

MdWindow::~MdWindow() {
}

void MdWindow::Close(){
  shouldClose_ = true;
  auto handle = window_handle_;
  if (IsWindow(handle)) {
    PostMessage(handle, WM_CLOSE, 0, 0);
  }else{
    std::cout << "close window window is not window " << handle << std::endl;
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
}

void MdWindow:: Hide(){
}

void MdWindow::Center(){
}

void MdWindow::SetFrame(SIZE size, bool keepCenter){
}

void MdWindow::SetTitle(std::string title){
}

void MdWindow::SetCanBeShown(){
  canBeShown_ = true;
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

bool MdWindow::HandleMessage(const UINT message){
  switch (message) {
    case WM_DESTROY:
      Destroy();
      if (!destroyed_) {
        destroyed_ = true;
        if (auto callback = callback_.lock()) {
          callback->OnWindowDestroy(id_);
        }
      }
      return true;
    case WM_CLOSE:
      std::cout << "preventCloseProcessing_" << preventCloseProcessing_ << std::endl;
      std::cout << "preventCloseForceClose_" << preventCloseForceClose_ << std::endl;
      std::cout << "shouldClose_" << shouldClose_ << std::endl;
      if (preventCloseProcessing_) {
        return true;
      }
      if (!preventCloseForceClose_){
        if (!shouldClose_) {
          preventCloseProcessing_ = true;
          this->SendToFlutter("onShouldClose");
          return true;
        }
      }
      
      if (auto callback = callback_.lock()) {
        callback->OnWindowClose(id_);
      }

      preventCloseForceClose_ = false;
      preventCloseProcessing_ =  false;
      this->SendToFlutter("onClose");
      std::cout << "use_count: " << window_.use_count() << std::endl; 
      break;
  }
  return false;
}


}  // namespace md_multi_window
