#include "md_multi_window_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>
#include <memory>
#include <sstream>

#include "md_window_manager.h"

namespace md_multi_window {

// static
void MdMultiWindowPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
    // auto channel =
    //     std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
    //         registrar->messenger(), "magicd/md_multi_window/method",
    //         &flutter::StandardMethodCodec::GetInstance());

    // auto plugin = std::make_unique<MdMultiWindowPlugin>();

    // channel->SetMethodCallHandler(
    //     [plugin_pointer = plugin.get()](const auto &call, auto result) {
    //       plugin_pointer->HandleMethodCall(call, std::move(result));
    //     });

    // registrar->AddPlugin(std::move(plugin));
}

void MdMultiWindowPlugin::AttachChannel(flutter::PluginRegistrarWindows *registrar) {
    auto channel =
        std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
            registrar->messenger(), "magicd/md_multi_window/method",
            &flutter::StandardMethodCodec::GetInstance());

    auto plugin = std::make_unique<MdMultiWindowPlugin>();

    channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result) {
          plugin_pointer->HandleMethodCall(call, std::move(result));
        });

    registrar->AddPlugin(std::move(plugin));
}

void MdMultiWindowPlugin::AttachChannelWithMain(
    flutter::PluginRegistry *registry, const char* main_window_id) {
    
    auto registrar = registry->GetRegistrarForPlugin("MdMultiWindowPluginCApi");
    auto window_registrar = flutter::PluginRegistrarManager::GetInstance()
      ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar);
    auto hwnd = FlutterDesktopViewGetHWND(FlutterDesktopPluginRegistrarGetView(registrar));
    std::string id = main_window_id;
    MdWindowManager::Instance()->AddWindowAndNotifyAll(id, GetAncestor(hwnd, GA_ROOT));
    MdMultiWindowPlugin::AttachChannel(window_registrar);
}

MdMultiWindowPlugin::MdMultiWindowPlugin() {}

MdMultiWindowPlugin::~MdMultiWindowPlugin() {}

void MdMultiWindowPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("getPlatformVersion") == 0) {
    std::ostringstream version_stream;
    version_stream << "Windows ";
    if (IsWindows10OrGreater()) {
      version_stream << "10+";
    } else if (IsWindows8OrGreater()) {
      version_stream << "8";
    } else if (IsWindows7OrGreater()) {
      version_stream << "7";
    }
    result->Success(flutter::EncodableValue(version_stream.str()));
  } else if (method_call.method_name() == "getAllWindowIDs") {
    result->Success(MdWindowManager::Instance()->GetAllWindowIDs());
    return;
  } else {
    result->NotImplemented();
  }
}

}  // namespace md_multi_window
