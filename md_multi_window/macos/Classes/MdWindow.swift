import Cocoa
import FlutterMacOS
import Foundation

public class MdWindow: NSObject {
  public let id: String
  public let window: MdFlutterWindow
  private var methodChannel: FlutterMethodChannel

  private var shouldClose: Bool = true
  private var preventCloseProcessing: Bool = false
  private var preventCloseForceClose: Bool = false

  init(
    id: String, window: MdFlutterWindow, methodChannel: FlutterMethodChannel
  ) {
    window.identifier = NSUserInterfaceItemIdentifier(rawValue: id)
    self.window = window
    self.id = id
    self.methodChannel = methodChannel
    super.init()
    self.window.delegate = self
  }

  deinit {
    logMessage("macos:", "release window resource:\(id)")
    window.delegate = nil
    if let flutterViewController = window.contentViewController as? FlutterViewController {
      flutterViewController.engine.shutDownEngine()
    }
    window.contentViewController = nil
    window.windowController = nil
  }

  func show() {
    window.makeKeyAndOrderFront(nil)
    NSApp.activate(ignoringOtherApps: true)
  }

  func hide() {
    MdMultiWindowPlugin.shouldTerminateApp = false
    sendToFlutter(event: "onHide")
    window.orderOut(nil)
  }

  func center() {
    window.center()
  }

  func setFrame(_ frame: NSRect, _ keepCenter: Bool = false) {
    var newFrame = frame
    if keepCenter {
      let cFrame = window.frame
      let nx = cFrame.origin.x + (cFrame.size.width - frame.width) / 2
      let ny = cFrame.origin.y + (cFrame.size.height - frame.height) / 2
      newFrame = NSRect(x: nx, y: ny, width: frame.width, height: frame.height)

    }
    window.setFrame(newFrame, display: false, animate: true)
  }

  func setTitle(_ title: String) {
    window.title = title
  }

  func setCanBeShown() {
    MdMultiWindowPlugin.windowInCreation = false
    window.windowCanBeShown = true
  }

  // close the window
  func close() {
    window.close()
  }

  // close the window, trigger should close callback
  func performClose() {
    window.performClose(nil)
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
      window.close()
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
    MdMultiWindowPlugin.shouldTerminateApp = window.lastWindowClosedShouldTerminateApp
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
