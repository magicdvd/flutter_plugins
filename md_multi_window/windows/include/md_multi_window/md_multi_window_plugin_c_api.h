#ifndef FLUTTER_PLUGIN_MD_MULTI_WINDOW_PLUGIN_C_API_H_
#define FLUTTER_PLUGIN_MD_MULTI_WINDOW_PLUGIN_C_API_H_

#include <flutter_plugin_registrar.h>
#include <flutter/plugin_registry.h>

#include <string>
#include <vector>
#include <memory>

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

FLUTTER_PLUGIN_EXPORT void MdMultiWindowPluginCApiRegisterMain(
    std::shared_ptr<FlutterWindow> window, const char* main_window_id = "md_mulit_window_main");

typedef std::shared_ptr<FlutterWindow> (
    *MdMultiWindowPluginCreateWindowCallback)(
    std::vector<std::string> command_line_arguments);
FLUTTER_PLUGIN_EXPORT void MdMultiWindowPluginCApiSetCreateWindowCallback(
    MdMultiWindowPluginCreateWindowCallback callback);

#if defined(__cplusplus)
}  // extern "C"
#endif

#endif  // FLUTTER_PLUGIN_MD_MULTI_WINDOW_PLUGIN_C_API_H_
