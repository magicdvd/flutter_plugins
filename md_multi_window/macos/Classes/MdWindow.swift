import Cocoa
import FlutterMacOS
import Foundation

public class MdWindow: NSObject {
  public let id: String
  private var window: MdFlutterWindow?
  private var methodChannel: FlutterMethodChannel

  private var shouldClose: Bool = true
  private var preventCloseProcessing: Bool = false
  private var preventCloseForceClose: Bool = false

  weak var delegate: MdWindowManagerDelegate?

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
    let rect = style.getFrame()
    var window: MdFlutterWindow?
    window = MdFlutterWindow(
      contentRect: rect,
      styleMask: style.styleMask(),
      backing: .buffered,
      defer: false,
      trafficLightsOffset: CGPoint(
        x: style.trafficLightsOffsetX, y: style.trafficLightsOffsetY),
      trafficLightsSpacingFix: style.trafficLightsSpacingFix)
    window!.isReleasedWhenClosed = false
    let project = FlutterDartProject()
    let initRoute = route ?? ""
    if let r = params {
      let args = encodeToString(r)
      project.dartEntrypointArguments = ["md_multi_window", "\(id)", initRoute, args]
    } else {
      project.dartEntrypointArguments = ["md_multi_window", "\(id)", initRoute]
    }
    let flutterViewController = FlutterViewController(project: project)
    window!.contentViewController = flutterViewController
    window!.title = style.title
    window!.maxSize = style.getMaxSize()
    window!.minSize = style.getMinSize()
    if style.titleShow {
      window!.titleVisibility = .visible
    } else {
      window!.titleVisibility = .hidden
    }
    window!.hideOnLaunch = style.hideOnLaunch
    window!.lastWindowClosedShouldTerminateApp = style.lastWindowClosedShouldTerminateApp
    window!.titlebarAppearsTransparent = style.titlebarAppearsTransparent
    window!.setFrame(rect, display: true)
    if style.center {
      window!.center()  // Center the window
    }
    window!.makeKeyAndOrderFront(nil)
    let plugin = flutterViewController.registrar(forPlugin: "MdMultiWindowPlugin")
    let methodCh = MdMultiWindowPlugin.attachChannel(with: plugin)
    MdMultiWindowPlugin.onWindowCreated?(flutterViewController)
    self.init(id: id, window: window, methodChannel: methodCh)
  }

  deinit {
    debugPrint("macos:", "release window resource:\(id) \(String(describing: window))")
    printRetainCount(of: window!)
    window?.delegate = nil
    if let flutterViewController = window?.contentViewController as? FlutterViewController {
      flutterViewController.engine.shutDownEngine()
    }
    window?.contentViewController = nil
    window?.windowController = nil
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
    debugPrint(newFrame)
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
    DispatchQueue.main.async { [weak self] in
      guard let self = self else {
        return
      }
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
    DispatchQueue.main.async { [weak self] in
      guard let self = self else {
        return
      }
      self.methodChannel.invokeMethod(
        name, arguments: fromWindowID
      )
    }
  }
}

extension MdWindow: NSWindowDelegate {
  internal func sendToFlutter(event: String) {
    debugPrint("macos:", "send to flutter", event)
    self.methodChannel.invokeMethod(
      event, arguments: self.id
    )
  }

  public func windowWillClose(_ notification: Notification) {
    MdMultiWindowPlugin.shouldTerminateApp = window!.lastWindowClosedShouldTerminateApp
    delegate?.willClose(windowID: id)
    preventCloseForceClose = false
    preventCloseProcessing = false
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
