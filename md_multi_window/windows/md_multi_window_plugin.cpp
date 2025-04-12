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

void MdMultiWindowPlugin::RegisterWindow(
    const std::string& id,
    std::shared_ptr<FlutterWindow> fw, 
    flutter::PluginRegistry *registry,
    bool hide_on_launch,
    bool last_window_should_terminate_app
  ) {
    auto registrar = registry->GetRegistrarForPlugin("MdMultiWindowPluginCApi");
    auto window_registrar = flutter::PluginRegistrarManager::GetInstance()
      ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar);
    auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
        window_registrar->messenger(), "magicd/md_multi_window/method",
          &flutter::StandardMethodCodec::GetInstance());
    auto plugin = std::make_unique<MdMultiWindowPlugin>();
    channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result) {
          plugin_pointer->HandleMethodCall(call, std::move(result));
        });
    window_registrar->AddPlugin(std::move(plugin));
    auto window_handle = ::GetAncestor(window_registrar->GetView()->GetNativeWindow(), GA_ROOT);
    MdWindowManager::Instance()->RegisterWindow(id, window_handle, std::move(fw), std::move(channel),hide_on_launch,last_window_should_terminate_app);
}

// HandleMessage true: break false: continue
bool MdMultiWindowPlugin::HandleMessage(const std::string& id, UINT message,  WPARAM const wparam,
  LPARAM const lparam){
    return MdWindowManager::Instance()->HandleMesssage(id, message, wparam, lparam);
}

MdMultiWindowPlugin::MdMultiWindowPlugin(){}

MdMultiWindowPlugin::~MdMultiWindowPlugin() {}

void MdMultiWindowPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  // std::cout << "windows method: "<< method_call.method_name() <<std::endl;
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
      auto new_window_id = md_multi_window::MdWindowManager::Instance()->CreateWindowAndRegister(val);
      result->Success(new_window_id);
    } else {
      std::cout << "EncodableValue is not a string!" << std::endl;
      result->Success("");
    }
  } else if (method_call.method_name() == "action"){
    if (auto str_value = std::get_if<std::string>(method_call.arguments())) {
      std::string val = *str_value;
      this->ActionToNative(val, std::move(result));
    } else {
      std::cout << "EncodableValue is not a string!" << std::endl;
      result->Success(flutter::EncodableValue(false));
    }
  } else if (method_call.method_name() == "mainScreenSize"){
    auto size = md_multi_window::GetCurrentMonitorSize();
    result->Success(flutter::EncodableMap{
      {flutter::EncodableValue("w"), flutter::EncodableValue(static_cast<double>(size.cx))},
      {flutter::EncodableValue("h"), flutter::EncodableValue(static_cast<double>(size.cy))},
   });
  } else {
    result->NotImplemented();
  }
}

void MdMultiWindowPlugin::ActionToNative(const std::string& value, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
    auto arguments = ParseMdCallArguments(value);
    if (arguments == std::nullopt) {
      std::cerr << "EncodableValue is not a string!";
      result->Success(flutter::EncodableValue(false));
      return;
    }
    // 1. targetWindowID
    if (!arguments->targetWindowID.has_value()) {
      std::cerr << "[md_multi_window] failed: missing targetWindowID\n";
      result->Success(flutter::EncodableValue(false));
      return;
    }
    const std::string& tid = arguments->targetWindowID.value();

    // 2. 获取目标窗口
    MdWindow* window = MdWindowManager::Instance()->GetWindow(tid);
    if (window == nullptr) {
      std::cerr << "[md_multi_window] failed: no window for ID = " << tid << "\n";
      result->Success(flutter::EncodableValue(false));
      return;
    }

    // 3. extraParams
    if (!arguments->extraParams.has_value()) {
      std::cerr << "[md_multi_window] failed: missing extraParams\n";
      result->Success(flutter::EncodableValue(false));
      return;
    }
    const auto& params = arguments->extraParams.value();

    // 4. name 参数
    auto it = params.find("name");
    if (it == params.end()) {
      std::cerr << "[md_multi_window] failed: 'name' not found in extraParams\n";
      result->Success(flutter::EncodableValue(false));
      return;
    }
    const std::string& action = it->second;
    std::cout << "windows:" << action << " from:" << arguments->windowID <<std::endl;
    if (action == "canBeShown") {
      result->Success(flutter::EncodableValue(false));
    }else if (action == "sendData") {
      result->Success(flutter::EncodableValue(true));
      flutter::EncodableMap encodable_map;
      for (const auto& pair : params) {
          if (pair.first == "name") {
            continue;
          }
          encodable_map[flutter::EncodableValue(pair.first)] = flutter::EncodableValue(pair.second);
      }
      encodable_map[flutter::EncodableValue("sender")] = flutter::EncodableValue(arguments->windowID);
      window->SendData(std::make_unique<flutter::EncodableValue>(encodable_map));
    }else if (action == "broadcastData") {
      result->Success(flutter::EncodableValue(true));
      flutter::EncodableMap encodable_map;
      for (const auto& pair : params) {
          if (pair.first == "name") {
            continue;
          }
          encodable_map[flutter::EncodableValue(pair.first)] = flutter::EncodableValue(pair.second);
      }
      encodable_map[flutter::EncodableValue("sender")] = flutter::EncodableValue(arguments->windowID);
      auto wins = MdWindowManager::Instance()->GetAllWindows();
      for (MdWindow* win : wins) {
        if (win) {
            win->SendData(std::make_unique<flutter::EncodableValue>(encodable_map));
        }
     }
    }else if (action == "close") {
      window->Close();
      result->Success(flutter::EncodableValue(true));
    }else if (action == "performClose"){
      window->PerformClose();
      result->Success(flutter::EncodableValue(true));
    }else if (action == "show"){
      window->Show();
      result->Success(flutter::EncodableValue(true));
    }else if (action == "hide"){
      window->Hide();
      result->Success(flutter::EncodableValue(true));
    }else if (action == "center"){
      window->Center();
      result->Success(flutter::EncodableValue(true));
    }else if (action == "setTitle"){
      auto k = params.find("setTitle");
      if (k != params.end()) {
        std::string title = k->second;
        window->SetTitle(title);
        result->Success(flutter::EncodableValue(true));
        return;
      }
      result->Success(flutter::EncodableValue(false));
    }else if (action == "setFrame"){
      result->Success(flutter::EncodableValue(false));
    }else if (action == "preventClose"){
      auto k = params.find("preventClose");
      if (k != params.end()) {
        std::string re = k->second;
        if (re == "true") {
          window->PreventClose(true);
        }else{
          window->PreventClose(false);
        }
        result->Success(flutter::EncodableValue(true));
        return;
      }
      result->Success(flutter::EncodableValue(false));
    }else if (action == "preventCloseEnd"){
      auto k = params.find("preventCloseEnd");
      if (k != params.end()) {
        std::string re = k->second;
        if (re == "true") {
          window->PreventCloseEnd(true);
        }else{
          window->PreventCloseEnd(false);
        }
        result->Success(flutter::EncodableValue(true));
        return;
      }
      result->Success(flutter::EncodableValue(false));
    }else {
      std::cerr << "[md_multi_window] action not found in extraParams\n";
      result->Success(flutter::EncodableValue(false));
      return;
    }
    //result->Success(flutter::EncodableValue(false));
    return;
}

}  // namespace md_multi_window
