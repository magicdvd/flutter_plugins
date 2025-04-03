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

void MdMultiWindowPluginCApiRegisterMain(
    std::shared_ptr<FlutterWindow> window, const char* main_window_id) {
  md_multi_window::MdMultiWindowPlugin::RegisterMainWindow(window, main_window_id);
}

void MdMultiWindowPluginCApiSetCreateWindowCallback(MdMultiWindowPluginCreateWindowCallback callback){
  md_multi_window::MdWindowManager::g_create_window_callback = callback;
}