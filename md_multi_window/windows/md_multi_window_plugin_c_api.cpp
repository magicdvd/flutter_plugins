#include "include/md_multi_window/md_multi_window_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "md_multi_window_plugin.h"

void MdMultiWindowPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  md_multi_window::MdMultiWindowPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
