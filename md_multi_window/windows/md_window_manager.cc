#include "md_window_manager.h"

#include <memory>
#include <string>
#include "md_call_arguments.h"
#include <thread>
#include <mutex>
#include "md_debug.h"

namespace md_multi_window {

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
      auto frameSize = style.GetFrame();
      unsigned int width =  static_cast<unsigned int>(frameSize.cx);
      unsigned int height =  static_cast<unsigned int>(frameSize.cy);
      if (style.center) {
        auto origin = style.GetCenterOrigin(frameSize);
        x = static_cast<unsigned int>(origin.x);
        y = static_cast<unsigned int>(origin.y);
      }
      auto wtitle = String2WString(style.title);
      auto fw = g_create_window_callback(
        {
          "md_multi_window", 
          id, 
          init_route, 
          to_json(call_args.extraParams)
        }, 
        id, 
        wtitle, 
        x, 
        y, 
        width, 
        height
      );
      return id;
    }
  }
  return "";
}

std::string MdWindowManager::RegisterWindow(
  const std::string& id,
  HWND window_handle,
  std::shared_ptr<FlutterWindow> fwin,
  std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel
){
  auto window = std::make_unique<MdWindow>(id, window_handle, fwin, std::move(channel));
  AddWindowAndNotifyAll(id, std::move(window));
  return id;
}

void MdWindowManager::AddWindowAndNotifyAll(const std::string& id, std::unique_ptr<MdWindow> window) {
  for (auto& pair : windows_) {
      std::unique_ptr<MdWindow>& window_ptr = pair.second;
      if (window_ptr) {
          window_ptr->NotifyFlutter("notifyWindowCreated", id);
      }
  }
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

std::vector<MdWindow*> MdWindowManager::GetAllWindows() {
  std::vector<MdWindow*> result;
  for (auto& pair : windows_) {
    if (pair.second) {
      result.push_back(pair.second.get());
    }
  }
  return result;
}

#define WM_EXECUTE_CALLBACK (WM_USER + 100)

 std::mutex threadMtx;
bool MdWindowManager::HandleMesssage(const std::string& id, UINT message,  WPARAM const wparam,
  LPARAM const lparam) {
  switch (message) {
    case WM_EXECUTE_CALLBACK: {
        std::string* str = reinterpret_cast<std::string*>(lparam);
        if (str) {
            windows_.erase(*str);
            delete str;
            DEBUG_LOG("window size: " << windows_.size());
        }
        return true;
      }
    case WM_SIZE: {
        auto window = GetWindow(id);
        if (!window){
          return false;
        }
        if (wparam == SIZE_MINIMIZED) {
          // minimize window
          window->SendToFlutter("onMinimize");
        }
        return false;
      }
      // SIZE_RESTORED SIZE_MAXIMIZED
    case WM_DESTROY: {
        if (windows_.size() == 1) {
          PostQuitMessage(0);
        }
        return true;
      }
    case WM_CLOSE: {
      auto window = GetWindow(id);
      if (!window){
        return false;
      }
      bool posted = false;
      if (window->DoCloseCheck()) {
        for (auto& pair : windows_) {
          if (pair.first == id) {
            continue;
          }
          std::unique_ptr<MdWindow>& window_ptr = pair.second;
          if (window_ptr) {
              window_ptr->NotifyFlutter("notifyWindowClose", id);
              if (!posted) {
                auto* payload = new std::string(id);
                PostMessage(window_ptr->HANDLE(), WM_EXECUTE_CALLBACK, 0, reinterpret_cast<LPARAM>(payload));
                posted = true;
              }
          }
        }
        window->SendToFlutter("onClose");
      }
      return true;
    }
  }
  return false;
}

}  // namespace md_multi_window
