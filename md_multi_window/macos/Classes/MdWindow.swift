import Cocoa
import FlutterMacOS
import Foundation

public class MdWindow: NSObject {
  public let id: String
  private weak var window: MdFlutterWindow?
  private var methodChannel: FlutterMethodChannel

  private var shouldClose: Bool = true
  private var preventCloseProcessing: Bool = false
  private var preventCloseForceClose: Bool = false

  init(
    id: String, window: MdFlutterWindow?, methodChannel: FlutterMethodChannel
  ) {
    window?.identifier = NSUserInterfaceItemIdentifier(rawValue: id)
    self.window = window
    self.id = id
    self.methodChannel = methodChannel
    super.init()
    self.window?.delegate = self
  }

  convenience init(
    id: String, windowStyle style: MdWindowStyle, route: String?, params: [String: String]?
  ) {
    let rect = NSRect(x: style.x, y: style.y, width: style.width, height: style.height)
    var window: MdFlutterWindow?
    window = MdFlutterWindow(
      contentRect: rect,
      styleMask: style.styleMask(),
      backing: .buffered,
      defer: false,
      trafficLightsOffset: CGPoint(
        x: style.trafficLightsOffsetX, y: style.trafficLightsOffsetY),
      trafficLightsSpacingFix: style.trafficLightsSpacingFix)
    window!.isReleasedWhenClosed = true
    window?.makeKeyAndOrderFront(nil)
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
    window?.contentViewController = flutterViewController
    window?.title = style.title
    if style.titleShow {
      window?.titleVisibility = .visible
    } else {
      window?.titleVisibility = .hidden
    }
    window?.hideOnLaunch = style.hideOnLaunch
    window?.titlebarAppearsTransparent = style.titlebarAppearsTransparent
    window?.setFrame(rect, display: true)
    if style.center {
      window?.center()  // Center the window
    }
    self.init(id: id, window: window, methodChannel: methodCh)
  }

  deinit {
    logMessage("macos:", "release window resource:\(id) \(String(describing: window))")
    window?.delegate = nil
    if let flutterViewController = window?.contentViewController as? FlutterViewController {
      flutterViewController.engine.shutDownEngine()
    }
    window?.contentViewController = nil
    window?.windowController = nil
    window?.close()
    window = nil
  }

  func show() {
    window?.makeKeyAndOrderFront(nil)
    NSApp.activate(ignoringOtherApps: true)
  }

  func hide() {
    MdMultiWindowPlugin.shouldTerminateApp = false
    sendToFlutter(event: "onHide")
    window?.orderOut(nil)
  }

  func center() {
    window?.center()
  }

  func setFrame(_ frame: NSRect, _ keepCenter: Bool = false) {
    var newFrame = frame
    if keepCenter {
      let cFrame = window?.frame
      if cFrame != nil {
        let nx = cFrame!.origin.x + (cFrame!.size.width - frame.width) / 2
        let ny = cFrame!.origin.y + (cFrame!.size.height - frame.height) / 2
        newFrame = NSRect(x: nx, y: ny, width: frame.width, height: frame.height)
      }
    }
    window?.setFrame(newFrame, display: false, animate: true)
  }

  func setTitle(_ title: String) {
    window?.title = title
  }

  func setCanBeShown() {
    MdMultiWindowPlugin.windowInCreation = false
    window?.windowCanBeShown = true
  }

  // close the window
  func close() {
    window?.close()
  }

  // close the window, trigger should close callback
  func performClose() {
    window?.performClose(nil)
  }

  func preventClose(_ b: Bool) {
    shouldClose = !b
  }

  func sendData(_ data: [String: String]) {
    DispatchQueue.main.async {
      self.methodChannel.invokeMethod(
        "recieveData", arguments: data
      )
    }
  }

  func preventCloseEnd(_ b: Bool) {
    if b {
      preventCloseForceClose = true
      window?.close()
    } else {
      preventCloseProcessing = false
    }
  }

  internal func notifyFlutter(name: String, fromWindowID: String) {
    DispatchQueue.main.async {
      self.methodChannel.invokeMethod(
        name, arguments: fromWindowID
      )
    }
  }
}

extension MdWindow: NSWindowDelegate {
  internal func sendToFlutter(event: String) {
    logMessage("macos:", "send to flutter", event)
    //DispatchQueue.main.async {
    self.methodChannel.invokeMethod(
      event, arguments: self.id
    )
    //}
  }

  public func windowWillClose(_ notification: Notification) {
    MdWindowManager.instance.removeWindowAndNotifyAll(id: id)
    preventCloseForceClose = false
    preventCloseProcessing = false
    MdMultiWindowPlugin.shouldTerminateApp = window!.lastWindowClosedShouldTerminateApp
    sendToFlutter(event: "onClose")
  }

  public func windowShouldClose(_ sender: NSWindow) -> Bool {
    // waiting for response for preventCloseProcessing
    if preventCloseProcessing {
      return false
    }
    if preventCloseForceClose {
      return true
    }
    if !shouldClose {
      // start process, wait result via [preventCloseEnd]
      preventCloseProcessing = true
      sendToFlutter(event: "onShouldClose")
      return false
    }
    return true
  }

  public func windowWillMiniaturize(_ notification: Notification) {
    sendToFlutter(event: "onMinimize")
  }

}
