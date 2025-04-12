#ifndef FLUTTER_PLUGIN_MD_MULTI_WINDOW_PLUGIN_C_API_H_
#define FLUTTER_PLUGIN_MD_MULTI_WINDOW_PLUGIN_C_API_H_

#include <flutter_plugin_registrar.h>
#include <flutter/plugin_registry.h>

#include <string>
#include <vector>
#include <memory>
#include <windows.h>

#ifdef FLUTTER_PLUGIN_IMPL
#define FLUTTER_PLUGIN_EXPORT __declspec(dllexport)
#else
#define FLUTTER_PLUGIN_EXPORT __declspec(dllimport)
#endif

#if defined(__cplusplus)
extern "C" {
#endif

FLUTTER_PLUGIN_EXPORT void MdMultiWindowPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar);

// make FlutterWinodow exist for compilation, it is redefined by flutter 
// generated code flutter_window.h in user's project
#ifndef RUNNER_FLUTTER_WINDOW_H_
#define RUNNER_FLUTTER_WINDOW_H_
class FlutterWindow {
    public:
    virtual void Destroy() = 0;
};

#endif  // RUNNER_FLUTTER_WINDOW_H_

FLUTTER_PLUGIN_EXPORT void MdMultiWindowPluginCApiRegister(
    const std::string& id,
    flutter::PluginRegistry *registry,
    std::shared_ptr<FlutterWindow> window,
    bool hide_on_launch,
    bool last_window_should_terminate_app
);

typedef std::shared_ptr<FlutterWindow> (
    *MdMultiWindowPluginCreateWindowCallback)(
    std::vector<std::string> command_line_arguments, 
    const std::string& id, 
    const std::wstring& title, 
    unsigned int x, 
    unsigned int y,
    unsigned int width, 
    unsigned int height,
    bool hide_on_launch,
    bool last_window_should_terminate_app
);
FLUTTER_PLUGIN_EXPORT void MdMultiWindowPluginCApiSetCreateWindowCallback(
    MdMultiWindowPluginCreateWindowCallback callback);

FLUTTER_PLUGIN_EXPORT bool MdMultiWindowPluginCApiHandleMessagee(
    const std::string& window_id,
    const UINT message,
    WPARAM const wparam,
    LPARAM const lparam
);

#if defined(__cplusplus)
}  // extern "C"
#endif

#endif  // FLUTTER_PLUGIN_MD_MULTI_WINDOW_PLUGIN_C_API_H_
