import Cocoa
import FlutterMacOS
import md_multi_window

@main
class AppDelegate: FlutterAppDelegate {

  override func applicationDidFinishLaunching(_ notification: Notification) {
    let rect = NSRect(x: 0, y: 0, width: 800, height: 600)
    let window = MdFlutterWindow(
      contentRect: rect,
      styleMask: [.titled, .closable, .resizable, .miniaturizable],
      backing: .buffered,
      defer: false, trafficLightsOffset: CGPoint(x: 10, y: -10), trafficLightsSpacingFix: -5)
    let flutterViewController = FlutterViewController()
    window.isReleasedWhenClosed = true
    window.makeKeyAndOrderFront(nil)
    window.contentViewController = flutterViewController
    window.title = "BigTitle"
    window.titleVisibility = .visible
    window.titlebarAppearsTransparent = false
    window.setFrame(rect, display: true)
    window.center()
    RegisterGeneratedPlugins(registry: flutterViewController)

    // hide on launch ( main() MdMultiWindow.widgetsDidLoad((){}, showWindow: true))
    window.hideOnLaunch = true
    window.lastWindowClosedShouldTerminateApp = true

    MdMultiWindowPlugin.attachChannelWithMain(with: flutterViewController, window: window)
    MdMultiWindowPlugin.setOnWindowCreated { viewController in
      RegisterGeneratedPlugins(registry: viewController)
    }
  }

  override func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
    return false
  }

  override func applicationSupportsSecureRestorableState(_ app: NSApplication) -> Bool {
    return true
  }

  override func applicationShouldHandleReopen(_ sender: NSApplication, hasVisibleWindows flag: Bool)
    -> Bool
  {
    logMessage("reopen", sender.windows.count)
    if !flag {
      for window in sender.windows {
        // 如果没有可见窗口，则重新显示窗口
        logMessage("reopen \(window)")
        window.makeKeyAndOrderFront(nil)
      }
    }
    return true
  }
}
