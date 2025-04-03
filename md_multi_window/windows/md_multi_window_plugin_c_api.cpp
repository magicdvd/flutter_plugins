#include "include/md_multi_window/md_multi_window_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "md_multi_window_plugin.h"
#include "md_window.h"

void MdMultiWindowPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  md_multi_window::MdMultiWindowPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}

void MdMultiWindowPluginCApiAttachChannelWithMain(
    flutter::PluginRegistry *registry, const char* main_window_id) {
  md_multi_window::MdMultiWindowPlugin::AttachChannelWithMain(registry,main_window_id);
}

typedef void (*WindowCreatedCallback)(void *flutter_view_controller);
void MdMultiWindowPluginCApiSetWindowCreatedCallback(WindowCreatedCallback callback){
  md_multi_window::MdWindow::window_callback = callback;
}