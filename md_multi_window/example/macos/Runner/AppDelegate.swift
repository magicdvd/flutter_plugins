import Cocoa
import FlutterMacOS
import md_multi_window

@main
class AppDelegate: FlutterAppDelegate {

  override func applicationDidFinishLaunching(_ notification: Notification) {
    let rect = NSRect(x: 0, y: 0, width: 800, height: 600)
    let window = MdFlutterWindow(
      contentRect: rect,
      styleMask: [.titled, .fullSizeContentView, .closable, .resizable, .miniaturizable],
      backing: .buffered,
      defer: false)
    let flutterViewController = FlutterViewController()
    window.contentViewController = flutterViewController
    window.isReleasedWhenClosed = false
    window.titleVisibility = .hidden
    window.titlebarAppearsTransparent = true
    window.setFrame(rect, display: true)
    window.center()
    RegisterGeneratedPlugins(registry: flutterViewController)

    // hide on launch ( main() MdMultiWindow.widgetsDidLoad((){}, showWindow: true))
    window.hideOnLaunch = true

    MdMultiWindowPlugin.attachChannelWithMain(with: flutterViewController, window: window)
    MdMultiWindowPlugin.setOnWindowCreated { viewController in
      RegisterGeneratedPlugins(registry: viewController)
    }
    window.makeKeyAndOrderFront(nil)
  }

  override func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
    print("Number of windows: \(NSApp.windows.count)")
    if NSApp.windows.count == 1 {
      debugPrint(NSApp.windows[0].identifier?.rawValue)
    }
    return false
  }

  override func applicationSupportsSecureRestorableState(_ app: NSApplication) -> Bool {
    return true
  }

  override func applicationShouldHandleReopen(_ sender: NSApplication, hasVisibleWindows flag: Bool)
    -> Bool
  {
    if !flag {
      for window in sender.windows {
        // 如果没有可见窗口，则重新显示窗口
        window.makeKeyAndOrderFront(nil)
      }
    }
    return true
  }
}
