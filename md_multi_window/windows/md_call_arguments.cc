#include "md_call_arguments.h"
#include <iostream>

namespace md_multi_window {

    float MdWindowStyle::gMainscreenWidth = -1234.0f;
    float MdWindowStyle::gMainscreenHeight = -4321.0f;

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

    SIZE MdWindowStyle::GetMinSize() {
        float rw = minSizeW;
        float rh = minSizeH;
        if (rw == MdWindowStyle::gMainscreenWidth || rh == MdWindowStyle::gMainscreenHeight) {
            auto size = GetCurrentMonitorSize();
            rw = (rw == MdWindowStyle::gMainscreenWidth) ? static_cast<float>(size.cx) : rw;
            rh = (rh == MdWindowStyle::gMainscreenHeight) ? static_cast<float>(size.cy) : rh;
        }
        return {static_cast<int>(rw), static_cast<int>(rh)};
    }

    SIZE MdWindowStyle::GetMaxSize() {
        float rw = maxSizeW;
        float rh = maxSizeH;
        if (rw == MdWindowStyle::gMainscreenWidth || rh == MdWindowStyle::gMainscreenHeight) {
            auto size = GetCurrentMonitorSize();
            rw = (rw == MdWindowStyle::gMainscreenWidth) ? static_cast<float>(size.cx) : rw;
            rh = (rh == MdWindowStyle::gMainscreenHeight) ? static_cast<float>(size.cy) : rh;
        }
        return {static_cast<int>(rw), static_cast<int>(rh)};
    }

    SIZE MdWindowStyle::GetFrame() {
        float rw = width;
        float rh = height;
        if (rw == MdWindowStyle::gMainscreenWidth || rh == MdWindowStyle::gMainscreenHeight) {
            auto size = GetCurrentMonitorSize();
            rw = (rw == MdWindowStyle::gMainscreenWidth) ? static_cast<float>(size.cx) : rw;
            rh = (rh == MdWindowStyle::gMainscreenHeight) ? static_cast<float>(size.cy) : rh;
        }
        return {static_cast<int>(rw), static_cast<int>(rh)};
    }

    POINT MdWindowStyle::GetCenterOrigin(const SIZE& size) {
        auto msize = GetCurrentMonitorSize();
        int ox = static_cast<int>(x);
        int oy = static_cast<int>(y);
        if (msize.cx > size.cx) {
            ox = (msize.cx - size.cx) / 2;
        }
        if (msize.cy > size.cy) {
            oy = (msize.cy - size.cy) / 2;
        }
        return {static_cast<LONG>(ox), static_cast<LONG>(oy)};
    }

    void from_json(const json& j, MdWindowStyle& style) {
        j.at("w").get_to(style.width);
        j.at("h").get_to(style.height);
        j.at("x").get_to(style.x);
        j.at("y").get_to(style.y);
        j.at("ts").get_to(style.titlebarAppearsTransparent);
        j.at("t").get_to(style.styleMaskRaw);
        j.at("c").get_to(style.center);
        j.at("tt").get_to(style.title);
        j.at("tts").get_to(style.titleShow);
        j.at("i").get_to(style.hideOnLaunch);
        j.at("l").get_to(style.lastWindowClosedShouldTerminateApp);
        j.at("tx").get_to(style.trafficLightsOffsetX);
        j.at("ty").get_to(style.trafficLightsOffsetY);
        j.at("tb").get_to(style.trafficLightsSpacingFix);
        j.at("iw").get_to(style.minSizeW);
        j.at("ih").get_to(style.minSizeH);
        j.at("aw").get_to(style.maxSizeW);
        j.at("ah").get_to(style.maxSizeH);
    }
    
    void from_json(const json& j, MdCallArguments& args) {
        j.at("id").get_to(args.windowID);
        if (j.contains("tid")) args.targetWindowID = j.at("tid").get<std::string>();
        if (j.contains("st")) args.windowStyle = j.at("st").get<MdWindowStyle>();
        if (j.contains("route")) args.initRoute = j.at("route").get<std::string>();
        if (j.contains("params")) args.extraParams = j.at("params").get<std::map<std::string, std::string>>();
    }

    std::string to_json(const std::optional<std::map<std::string, std::string>>& data) {
        if (data.has_value()) {
            return json(*data).dump();  // encode to JSON string
        } else {
            return "null";  // or ""
        }
    }
    
    std::optional<MdCallArguments> ParseMdCallArguments(const std::string& json_str) {
        try {
            json j = json::parse(json_str);
            return j.get<MdCallArguments>();
        } catch (const json::exception& e) {
            std::cerr << "JSON Parsing Error: " << e.what() << std::endl;
            return std::nullopt;
        }
    }

} // namespace md_multi_window