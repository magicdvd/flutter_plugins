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
        let rect = NSRect(x: style.x, y: style.y, width: style.width, height: style.height)
        let window = MdFlutterWindow(
            contentRect: rect,
            styleMask: style.styleMask(),
            backing: .buffered,
            defer: false,
            trafficLightsOffset: CGPoint(
                x: style.trafficLightsOffsetX, y: style.trafficLightsOffsetY),
            trafficLightsSpacingFix: style.trafficLightsSpacingFix)
        let project = FlutterDartProject()
        let initRoute = route ?? ""
        if let r = params {
            let args = encodeToString(r)
            project.dartEntrypointArguments = ["md_multi_window", "\(id)", initRoute, args]
        } else {
            project.dartEntrypointArguments = ["md_multi_window", "\(id)", initRoute]
        }
        let flutterViewController = FlutterViewController(project: project)
        let plugin = flutterViewController.registrar(forPlugin: "MdMultiWindowPlugin")
        let methodCh = MdMultiWindowPlugin.attachChannel(with: plugin)
        MdMultiWindowPlugin.onWindowCreated?(flutterViewController)
        window.contentViewController = flutterViewController
        window.isReleasedWhenClosed = true
        window.title = style.title
        if style.titleShow {
            window.titleVisibility = .visible
        } else {
            window.titleVisibility = .hidden
        }
        window.hideOnLaunch = style.hideOnLaunch
        window.titlebarAppearsTransparent = style.titlebarAppearsTransparent
        window.setFrame(rect, display: true)
        if style.center {
            window.center()  // Center the window
        }
        return MdWindow(id: id, window: window, methodChannel: methodCh)
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
