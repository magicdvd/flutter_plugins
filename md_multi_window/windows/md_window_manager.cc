#include "md_window_manager.h"

#include <memory>
#include <string>
#include "md_window.h"
#include "md_call_arguments.h"

namespace md_multi_window {

SIZE GetCurrentMonitorSize() {
  HMONITOR monitor = MonitorFromPoint({0,0}, MONITOR_DEFAULTTONEAREST);
  if (monitor){
      MONITORINFO monitor_info = { sizeof(MONITORINFO) };
      if (GetMonitorInfo(monitor, &monitor_info)) {
        RECT monitor_rect = monitor_info.rcMonitor;
        int width = monitor_rect.right - monitor_rect.left;
        int height = monitor_rect.bottom - monitor_rect.top;
        return {width, height};
      }
  }
  return {0,0};
}

std::wstring String2WString(const std::string &string) {
  int size_needed = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, nullptr, 0);
  if (size_needed == 0) {
    return {};
  }
  std::wstring wstrTo(size_needed, 0);
  int converted_length = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, &wstrTo[0], size_needed);
  if (converted_length == 0) {
    return {};
  }
  return wstrTo;
}

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
      MdWindowStyle style = call_args.windowStyle.value();
      unsigned int x =  static_cast<unsigned int>(style.x);
      unsigned int y =  static_cast<unsigned int>(style.y);
      unsigned int width =  static_cast<unsigned int>(style.width);
      unsigned int height =  static_cast<unsigned int>(style.height);
      SIZE screen_size = GetCurrentMonitorSize();
      if (style.center) {
        if (static_cast<unsigned int>(screen_size.cx)  > width) {
          x = (screen_size.cx - width) / 2;
        }
        if (static_cast<unsigned int>(screen_size.cy)  > height) {
          y = (screen_size.cy - height) / 2;
        }
      }
      auto wtitle = String2WString(style.title);
      auto fw = g_create_window_callback({"md_multi_window", id, init_route, to_json(call_args.extraParams)}, id, wtitle, x, y, width, height);
      // auto window = std::make_unique<MdWindow>(id, fw, shared_from_this());
      // AddWindowAndNotifyAll(id, std::move(window));
      return id;
    }
  }
  return "";
}

std::string MdWindowManager::RegisterWindow(
  const std::string& id, 
  std::shared_ptr<FlutterWindow> fwin,
  HWND window_handle,
  std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel
){
  auto window = std::make_unique<MdWindow>(id, fwin, window_handle, std::move(channel), shared_from_this());
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
