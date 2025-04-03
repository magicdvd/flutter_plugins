#include "md_window_manager.h"

#include <memory>
#include <string>
#include "md_window.h"


namespace md_multi_window {

// static
MdWindowManager* MdWindowManager::Instance() {
  static std::shared_ptr<MdWindowManager> manager = std::make_shared<MdWindowManager>();
  return manager.get();
}

MdWindowManager::MdWindowManager() : windows_() {
}

std::string MdWindowManager::Create(std::string id, std::string args) {
 
  //auto window = std::make_unique<MdWindow>(id, std::move(args), shared_from_this());
// //   auto channel = window->GetWindowChannel();
// //   channel->SetMethodCallHandler([this](int64_t from_window_id,
// //                                        int64_t target_window_id,
// //                                        const std::string &call,
// //                                        flutter::EncodableValue *arguments,
// //                                        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
// //     HandleWindowChannelCall(from_window_id, target_window_id, call, arguments, std::move(result));
// //   });
  //windows_[id] = std::move(window);
  return id;
}

void MdWindowManager::AddWindowAndNotifyAll(std::string id, HWND window_handle) {
  auto window = std::make_unique<MdWindow>(id, window_handle, shared_from_this());
  windows_[id] = std::move(window);
}

flutter::EncodableList MdWindowManager::GetAllWindowIDs() {
  flutter::EncodableList resList = flutter::EncodableList();
  for (const auto &window : windows_) {
      resList.push_back(flutter::EncodableValue(window.first));
  }
  return resList;
}

void MdWindowManager::OnWindowClose(std::string id) {
}

void MdWindowManager::OnWindowDestroy(std::string id) {
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