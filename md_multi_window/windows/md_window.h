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

class MdWindow {

 public:
  MdWindow(
    const std::string& id,
    HWND window_handle,
    std::shared_ptr<FlutterWindow> window,
    std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel,
    bool hide_on_launch,
    bool last_window_should_terminate_app
  );
  ~MdWindow();

  void SendData(
    std::unique_ptr<flutter::EncodableValue> data, 
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result = nullptr);
  
  void NotifyFlutter(
      const std::string& name,
      const std::string& from_window_id,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result = nullptr);

  void SendToFlutter(
    const std::string& name,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result = nullptr);
  
  void Close();

  void PerformClose();

  void PreventClose(bool yesOrNo);

  void PreventCloseEnd(bool yesOrNo);

  void Show();

  void Hide();

  void Center();

  void SetFrame(SIZE size, bool keepCenter);

  void SetTitle(std::string title);

  void SetCanBeShown();

  bool DoCloseCheck();

  bool GetLastWindowClosedShouldTerminateApp();

  HWND HANDLE();

private:
  std::shared_ptr<FlutterWindow> window_;

  std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel_;

  std::string id_;

  HWND window_handle_;

  bool shouldClose_;

  bool preventCloseForceClose_;

  bool preventCloseProcessing_;

  bool canBeShown_;

  bool destroyed_;

  bool lastWindowClosedShouldTerminateApp_;

  bool hideOnLaunch_;

  void Destroy();
};

}  // namespace md_multi_window

#endif //MD_MULTI_WINDOW_PLUGIN_MD_WINDOW_H_