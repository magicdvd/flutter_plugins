import Cocoa
import FlutterMacOS
import Foundation

public class MdWindowManager: NSObject {
    // 静态常量，只有第一次访问时会被初始化
    static let instance: MdWindowManager = MdWindowManager()

    private var windows: [String: MdWindow] = [:]

    private override init() {
        super.init()
    }

    // deinit {
    //     windows.removeAll()
    //     logMessage("MdWindowManager released")
    // }

    internal func addWindowAndNotifyAll(windowID id: String, window: MdWindow) {
        for (_, window) in windows {
            window.notifyFlutter(name: "notifyWindowCreated", fromWindowID: id)
        }
        windows[id] = window
    }

    internal func removeWindowAndNotifyAll(id: String) {
        for (wid, window) in windows {
            if wid == id {
                continue
            }
            window.notifyFlutter(name: "notifyWindowClose", fromWindowID: id)
        }
        windows.removeValue(forKey: id)
    }

    internal func createWindow(
        id: String, windowStyle style: MdWindowStyle, route: String?, params: [String: String]?
    ) -> MdWindow {
        return MdWindow(id: id, windowStyle: style, route: route, params: params)
    }

    public func getAllWindowIDs() -> [String] {
        return Array(windows.keys)
    }

    public func getWindow(id: String) -> MdWindow? {
        if let value = windows[id] {
            return value
        } else {
            return nil  // 键不存在时返回 nil
        }
    }
}
