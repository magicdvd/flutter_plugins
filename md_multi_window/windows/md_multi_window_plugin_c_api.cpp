#include "include/md_multi_window/md_multi_window_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "md_multi_window_plugin.h"
#include "md_window_manager.h"

void MdMultiWindowPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  md_multi_window::MdMultiWindowPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}

FLUTTER_PLUGIN_EXPORT void MdMultiWindowPluginCApiRegister(
  const std::string& id,
  flutter::PluginRegistry *registry,
  std::shared_ptr<FlutterWindow> window
){
  md_multi_window::MdMultiWindowPlugin::RegisterWindow(id, window, registry);
}

void MdMultiWindowPluginCApiSetCreateWindowCallback(MdMultiWindowPluginCreateWindowCallback callback){
  md_multi_window::MdWindowManager::g_create_window_callback = callback;
}

FLUTTER_PLUGIN_EXPORT bool MdMultiWindowPluginCApiHandleMessagee(
  const std::string& window_id,
  const UINT message
){
  return md_multi_window::MdMultiWindowPlugin::HandleMessage(window_id, message);
}

