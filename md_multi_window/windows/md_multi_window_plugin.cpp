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
#include "md_call_arguments.h"

namespace md_multi_window {

// static
void MdMultiWindowPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
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

// void MdMultiWindowPlugin::AttachChannel(flutter::PluginRegistrarWindows *registrar) {
//     auto channel =
//         std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
//             registrar->messenger(), "magicd/md_multi_window/method",
//             &flutter::StandardMethodCodec::GetInstance());

//     auto plugin = std::make_unique<MdMultiWindowPlugin>();

//     channel->SetMethodCallHandler(
//         [plugin_pointer = plugin.get()](const auto &call, auto result) {
//           plugin_pointer->HandleMethodCall(call, std::move(result));
//         });

//     registrar->AddPlugin(std::move(plugin));
// }

void MdMultiWindowPlugin::RegisterMainWindow(
    std::shared_ptr<FlutterWindow> window, const char* main_window_id) {
    std::string id = main_window_id;
    MdWindowManager::Instance()->RegisterMainWindow(id, std::move(window));
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
  } else if (method_call.method_name() == "createWindow"){
    if (auto str_value = std::get_if<std::string>(method_call.arguments())) {
      std::string val = *str_value;
      md_multi_window::MdWindowManager::Instance()->CreateWindowAndRegister(val);
    } else {
      std::cout << "EncodableValue is not a string!" << std::endl;
      result->Success("");
    }
  } else if (method_call.method_name() == "action"){
    result->NotImplemented();
  } else if (method_call.method_name() == "mainScreenSize"){
    result->NotImplemented();
  } else {
    result->NotImplemented();
  }
}

void MdMultiWindowPlugin::ActionToNative(const std::string& value, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
    auto arguments = ParseMdCallArguments(value);
    if (arguments == std::nullopt) {
      std::cerr << "EncodableValue is not a string!";
      result->Success(false);
    }
    // 1. targetWindowID
    if (!arguments->targetWindowID.has_value()) {
      std::cerr << "[md_multi_window] failed: missing targetWindowID\n";
      result->Success(false);
      return;
    }
    const std::string& tid = arguments->targetWindowID.value();

    // 2. 获取目标窗口
    MdWindow* window = MdWindowManager::Instance()->GetWindow(tid);
    if (window == nullptr) {
      std::cerr << "[md_multi_window] failed: no window for ID = " << tid << "\n";
      result->Success(false);
      return;
    }

    // 3. extraParams
    if (!arguments->extraParams.has_value()) {
      std::cerr << "[md_multi_window] failed: missing extraParams\n";
      result->Success(false);
      return;
    }
    const auto& params = arguments->extraParams.value();

    // 4. name 参数
    auto it = params.find("name");
    if (it == params.end()) {
      std::cerr << "[md_multi_window] failed: 'name' not found in extraParams\n";
      result->Success(false);
      return;
    }
    const std::string& action = it->second;
   
    if (action == "canBeShown") {

    }else if (action == "sendData") {

    }else if (action == "broadcastData") {
      
    }else if (action == "close") {
      
    }else if (action == "performClose"){

    }else if (action == "show"){
      
    }else if (action == "hide"){
      
    }else if (action == "center"){
      
    }else if (action == "setTitle"){
      
    }else if (action == "setFrame"){
      
    }else if (action == "preventClose"){
      
    }else if (action == "preventCloseEnd"){
      
    }else {
      std::cerr << "[md_multi_window] action not found in extraParams\n";
      result->Success(false);
      return;
    }
    result->Success(false);
    return;
}


}  // namespace md_multi_window
