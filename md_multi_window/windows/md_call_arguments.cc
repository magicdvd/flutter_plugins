#include "md_call_arguments.h"
#include <iostream>

namespace md_multi_window {

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