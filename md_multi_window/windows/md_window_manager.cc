#include "md_window_manager.h"

#include <memory>
#include <string>
#include "md_window.h"
#include "md_call_arguments.h"

namespace md_multi_window {

MdMultiWindowPluginCreateWindowCallback MdWindowManager::g_create_window_callback = nullptr;

// static
MdWindowManager* MdWindowManager::Instance() {
  static std::shared_ptr<MdWindowManager> manager = std::make_shared<MdWindowManager>();
  return manager.get();
}

MdWindowManager::MdWindowManager() : windows_() {
}

std::string MdWindowManager::CreateWindowAndRegister(const std::string& args) {
  auto data = ParseMdCallArguments(args);
  if (data != std::nullopt) {
    if (MdWindowManager::g_create_window_callback) {
      MdCallArguments call_args = data.value();
      std::string init_route = call_args.initRoute.value_or("");
      std::string id = call_args.windowID;
      auto fw = g_create_window_callback({"md_multi_window", id, init_route, to_json(call_args.extraParams)});
      auto window = std::make_unique<MdWindow>(id, fw, shared_from_this());
      AddWindowAndNotifyAll(id, std::move(window));
      return id;
    }
  }
  return "";
}

std::string MdWindowManager::RegisterMainWindow(const std::string& id, std::shared_ptr<FlutterWindow> fwin) {
  auto window = std::make_unique<MdWindow>(id, fwin, shared_from_this());
  AddWindowAndNotifyAll(id, std::move(window));
  return id;
}

void MdWindowManager::AddWindowAndNotifyAll(const std::string& id, std::unique_ptr<MdWindow> window) {
  windows_[id] = std::move(window);
}

flutter::EncodableList MdWindowManager::GetAllWindowIDs() {
  flutter::EncodableList resList = flutter::EncodableList();
  for (const auto &window : windows_) {
      resList.push_back(flutter::EncodableValue(window.first));
  }
  return resList;
}

MdWindow* MdWindowManager::GetWindow(const std::string& id) {
  auto it = windows_.find(id);
  if (it != windows_.end()) {
    return it->second.get();
  }
  return nullptr;
}

void MdWindowManager::OnWindowClose(const std::string& id) {
}

void MdWindowManager::OnWindowDestroy(const std::string& id) {
    windows_.erase(id);
}

// void MultiWindowManager::HandleWindowChannelCall(
//     int64_t from_window_id,
//     int64_t target_window_id,
//     const std::string &call,
//     flutter::EncodableValue *arguments,
//     std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result
// ) {
//   auto target_window_entry = windows_.find(target_window_id);
//   if (target_window_entry == windows_.end()) {
//     result->Error("-1", "target window not found.");
//     return;
//   }
//   auto target_window_channel = target_window_entry->second->GetWindowChannel();
//   if (!target_window_channel) {
//     result->Error("-1", "target window channel not found.");
//     return;
//   }
//   target_window_channel->InvokeMethod(from_window_id, call, arguments, std::move(result));
// }


}  // namespace md_multi_window
