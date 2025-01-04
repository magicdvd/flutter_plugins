import Cocoa
import Foundation

func printRetainCount(of object: AnyObject) {
    let count = CFGetRetainCount(object)
    print("Retain count of \(object): \(count)")
}

//MdWindowStyle 调用的窗口样式表述
struct MdWindowStyle: Codable {
    static let gTitleStyle: [Int: NSWindow.StyleMask] = [
        1: .titled, 2: .closable, 3: .resizable, 4: .miniaturizable, 5: .fullScreen,
        6: .borderless, 7: .fullSizeContentView,
    ]
    // it is magic negative number for specified meaning
    static let gMainScreenSize = NSSize(width: -1234, height: -4321)
    let width: CGFloat
    let height: CGFloat
    let x: CGFloat
    let y: CGFloat
    let titlebarAppearsTransparent: Bool
    let styleMaskRaw: [Int]
    let center: Bool
    let title: String
    let titleShow: Bool
    let hideOnLaunch: Bool
    let lastWindowClosedShouldTerminateApp: Bool
    let trafficLightsOffsetX: CGFloat
    let trafficLightsOffsetY: CGFloat
    let trafficLightsSpacingFix: CGFloat
    let minSizeW: CGFloat
    let minSizeH: CGFloat
    let maxSizeW: CGFloat
    let maxSizeH: CGFloat
    enum CodingKeys: String, CodingKey {
        case width = "w"
        case height = "h"
        case x = "x"
        case y = "y"
        case titlebarAppearsTransparent = "ts"
        case styleMaskRaw = "t"
        case center = "c"
        case title = "tt"
        case titleShow = "tts"
        case hideOnLaunch = "i"
        case lastWindowClosedShouldTerminateApp = "l"
        case trafficLightsOffsetX = "tx"
        case trafficLightsOffsetY = "ty"
        case trafficLightsSpacingFix = "tb"
        case minSizeW = "iw"
        case minSizeH = "ih"
        case maxSizeW = "aw"
        case maxSizeH = "ah"
    }
    func styleMask() -> NSWindow.StyleMask {
        var mask: NSWindow.StyleMask = []
        for number in styleMaskRaw {
            mask.insert(MdWindowStyle.gTitleStyle[number]!)
        }
        return mask
    }
    func getMinSize() -> NSSize {
        var rw: CGFloat = minSizeW
        var rh = minSizeH
        if rw == MdWindowStyle.gMainScreenSize.width || rh == MdWindowStyle.gMainScreenSize.height {
            let size = getMainScreenSize()
            rw = (rw == MdWindowStyle.gMainScreenSize.width) ? size.width : rw
            rh = (rh == MdWindowStyle.gMainScreenSize.height) ? size.height : rh
        }
        return NSSize(width: rw, height: rh)
    }
    func getMaxSize() -> NSSize {
        var rw = maxSizeW
        var rh = maxSizeH
        if rw == MdWindowStyle.gMainScreenSize.width || rh == MdWindowStyle.gMainScreenSize.height {
            let size = getMainScreenSize()
            rw = (rw == MdWindowStyle.gMainScreenSize.width) ? size.width : rw
            rh = (rh == MdWindowStyle.gMainScreenSize.height) ? size.height : rh
        }
        return NSSize(width: rw, height: rh)
    }
    func getFrame() -> NSRect {
        var rw = width
        var rh = height
        if rw == MdWindowStyle.gMainScreenSize.width || rh == MdWindowStyle.gMainScreenSize.height {
            let size = getMainScreenSize()
            rw = (rw == MdWindowStyle.gMainScreenSize.width) ? size.width : rw
            rh = (rh == MdWindowStyle.gMainScreenSize.height) ? size.height : rh
        }
        return NSRect(x: x, y: y, width: rw, height: rh)
    }
}

//MdCallArguments 调用的参数映射
struct MdCallArguments: Codable {
    let windowID: String
    let targetWindowID: String?
    let windowStyle: MdWindowStyle?
    let initRoute: String?
    let extraParams: [String: String]?
    enum CodingKeys: String, CodingKey {
        case windowID = "id"
        case targetWindowID = "tid"
        case windowStyle = "st"
        case initRoute = "route"
        case extraParams = "params"
    }
}

func getMainScreenSize(_ defaultValue: NSSize = NSSize(width: 800, height: 600)) -> NSSize {
    if let screen = NSScreen.main {
        let screenSize = screen.frame.size
        return screenSize
    }
    return defaultValue
}

// decode json
func decodeJSON<T: Codable>(from jsonString: String, to type: T.Type) -> T? {
    guard let jsonData = jsonString.data(using: .utf8) else {
        debugPrint("macos:", "unable to convert string to data.")
        return nil
    }
    let decoder = JSONDecoder()
    do {
        let decodedObject = try decoder.decode(T.self, from: jsonData)
        return decodedObject
    } catch {
        debugPrint("macos:", "decoding JSON: \(error)")
        return nil
    }
}

// 通用编码为 String 的函数
func encodeToString<T: Codable>(_ object: T) -> String {
    let encoder = JSONEncoder()
    // 设置编码选项，例如将输出格式化为易读的 JSON
    encoder.outputFormatting = .prettyPrinted
    do {
        // 尝试编码
        let encodedData = try encoder.encode(object)
        // 将 Data 转换为 String
        if let jsonString = String(data: encodedData, encoding: .utf8) {
            return jsonString
        } else {
            debugPrint("Error: Unable to convert Data to String.")
            return "{}"
        }
    } catch {
        // 错误处理
        debugPrint("Error encoding object: \(error)")
        return "{}"
    }
}
