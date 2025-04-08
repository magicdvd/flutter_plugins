#ifndef FLUTTER_PLUGIN_MD_MULTI_WINDOW_PLUGIN_H_
#define FLUTTER_PLUGIN_MD_MULTI_WINDOW_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registry.h>
#include <flutter/plugin_registrar_windows.h>
#include "include/md_multi_window/md_multi_window_plugin_c_api.h"

#include <memory>

namespace md_multi_window {

class MdMultiWindowPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);
  static void RegisterMainWindow(std::shared_ptr<FlutterWindow> window, const char* main_window_id);
  MdMultiWindowPlugin();

  virtual ~MdMultiWindowPlugin();

  // Disallow copy and assign.
  MdMultiWindowPlugin(const MdMultiWindowPlugin&) = delete;
  MdMultiWindowPlugin& operator=(const MdMultiWindowPlugin&) = delete;

  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
 private:
  void ActionToNative(const std::string& value, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace md_multi_window

#endif  // FLUTTER_PLUGIN_MD_MULTI_WINDOW_PLUGIN_H_
