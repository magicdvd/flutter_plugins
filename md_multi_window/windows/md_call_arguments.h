#ifndef MD_CALL_ARGUMENTS_H_
#define MD_CALL_ARGUMENTS_H_

#include <string>
#include <map>
#include <vector>
#include <optional>
#include "include/md_multi_window/json.hpp"

using json = nlohmann::json;

namespace md_multi_window {

struct MdWindowStyle {
    float width = 0;
    float height = 0;
    float x = 0;
    float y = 0;
    bool titlebarAppearsTransparent = false;
    std::vector<int> styleMaskRaw;
    bool center = false;
    std::string title;
    bool titleShow = false;
    bool hideOnLaunch = false;
    bool lastWindowClosedShouldTerminateApp = false;
    float trafficLightsOffsetX = 0;
    float trafficLightsOffsetY = 0;
    float trafficLightsSpacingFix = 0;
    float minSizeW = 0;
    float minSizeH = 0;
    float maxSizeW = 0;
    float maxSizeH = 0;
};

struct MdCallArguments {
    std::string windowID;
    std::optional<std::string> targetWindowID;
    std::optional<MdWindowStyle> windowStyle;
    std::optional<std::string> initRoute;
    std::optional<std::map<std::string, std::string>> extraParams;
};

void from_json(const json& j, MdWindowStyle& style);
void from_json(const json& j, MdCallArguments& args);
std::optional<MdCallArguments> ParseMdCallArguments(const std::string& json_str);

}  // namespace md_multi_window

#endif  // MD_CALL_ARGUMENTS_H_
