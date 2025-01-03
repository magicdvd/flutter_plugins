import Cocoa
import FlutterMacOS

public func logMessage(_ items: Any..., terminator: String = "\n") {
  #if DEBUG
    debugPrint(items, terminator: terminator)
  #else
    // 发布模式下不打印
  #endif
}

public class MdMultiWindowPlugin: NSObject, FlutterPlugin {
  private static let instance = MdMultiWindowPlugin()

  public static var windowInCreation: Bool = false
  private static var _shouldTerminateApp = false
  public static var shouldTerminateApp: Bool {
    get {
      if windowInCreation {
        return false
      }
      return _shouldTerminateApp
    }
    set {
      _shouldTerminateApp = newValue
    }
  }

  // register do nothing
  public static func register(with registrar: FlutterPluginRegistrar) {
    // do nothing
  }

  // [attachChannelWithMain] it is called by system (after first window created, need )
  public static func attachChannelWithMain(
    with registry: FlutterViewController,
    firstWindowID name: String = "md_mulit_window_main", window iwindow: MdFlutterWindow? = nil
  ) {
    let registar = registry.registrar(forPlugin: "MdMultiWindowPlugin")
    // attach channel for flutterViewController(flutter engine)
    let methodCh = attachChannel(with: registar)
    var mWindow: MdFlutterWindow
    if iwindow == nil {
      guard let app = NSApplication.shared.delegate as? FlutterAppDelegate else {
        logMessage(
          "macos:",
          "failed to find flutter main window, application delegate is not FlutterAppDelegate")
        return
      }
      guard let window = app.mainFlutterWindow as? MdFlutterWindow else {
        logMessage("macos:", "failed to find flutter main window(make sure it is MdFlutterWindow)")
        return
      }
      mWindow = window
    } else {
      mWindow = iwindow!
    }
    // attach the NSWindow(created by flutter xib)
    let mainWindow = MdWindow(
      id: name, window: mWindow, methodChannel: methodCh)
    MdWindowManager.instance.addWindowAndNotifyAll(windowID: name, window: mainWindow)
  }

  internal static func attachChannel(with registrar: FlutterPluginRegistrar) -> FlutterMethodChannel
  {
    let channel = FlutterMethodChannel(
      name: "magicd/md_multi_window/method", binaryMessenger: registrar.messenger)
    // 只处理Flutter来的消息
    registrar.addMethodCallDelegate(instance, channel: channel)
    return channel
  }

  private func actionToNative(_ args: String, _ result: @escaping FlutterResult) {
    logMessage("macos:", args)
    guard let arguments = decodeJSON(from: args, to: MdCallArguments.self),
      let tid = arguments.targetWindowID,
      let window = MdWindowManager.instance.getWindow(id: tid),
      let params = arguments.extraParams,
      let action = params["name"]
    else {
      logMessage("macos:", "failed not enough params", args)
      result(false)
      return
    }
    switch action {
    case "canBeShown":
      window.setCanBeShown()
    case "sendData":
      result(true)
      var mp = params
      mp.removeValue(forKey: "name")
      mp["sender"] = arguments.windowID
      window.sendData(mp)
    case "brocastData":
      result(true)
      var mp = params
      mp.removeValue(forKey: "name")
      mp["sender"] = arguments.windowID
      let wids = MdWindowManager.instance.getAllWindowIDs()
      for wid in wids {
        if let window = MdWindowManager.instance.getWindow(id: wid) {
          window.sendData(mp)
        } else {
          continue
        }
      }
    case "close":
      DispatchQueue.main.async {
        window.close()
        result(true)
      }
    case "performClose":
      DispatchQueue.main.async {
        window.performClose()
        result(true)
      }
    case "show":
      DispatchQueue.main.async {
        window.show()
        result(true)
      }
    case "hide":
      DispatchQueue.main.async {
        window.hide()
        result(true)
      }
    case "center":
      DispatchQueue.main.async {
        window.center()
        result(true)
      }
    case "setTitle":
      DispatchQueue.main.async {
        guard let t = params["setTitle"] else {
          result(false)
          return
        }
        window.setTitle(t)
        result(true)
      }
    case "setFrame":
      DispatchQueue.main.async {
        guard let x = params["setFrame.x"],
          let y = params["setFrame.y"],
          let w = params["setFrame.w"],
          let h = params["setFrame.h"],
          let k = params["setFrame.k"]
        else {
          result(false)
          return
        }
        guard let xx = Double(x),
          let yy = Double(y),
          let ww = Double(w),
          let hh = Double(h)
        else {
          result(false)
          return
        }
        if k == "false" {
          window.setFrame(
            NSRect(x: CGFloat(xx), y: CGFloat(yy), width: CGFloat(ww), height: CGFloat(hh)), false)
        } else {
          window.setFrame(
            NSRect(x: CGFloat(xx), y: CGFloat(yy), width: CGFloat(ww), height: CGFloat(hh)), true)
        }
        result(true)
      }
    case "preventClose":
      DispatchQueue.main.async {
        guard let b = params["preventClose"] else {
          result(false)
          return
        }
        if b == "true" {
          window.preventClose(true)
        } else {
          window.preventClose(false)
        }
        result(true)
      }
    case "preventCloseEnd":
      DispatchQueue.main.async {
        guard let b = params["preventCloseEnd"] else {
          result(false)
          return
        }
        if b == "true" {
          window.preventCloseEnd(true)
        } else {
          window.preventCloseEnd(false)
        }
        result(true)
      }
    default:
      logMessage("macos:", action, "failed action not found")
      result(false)
    }
  }

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
    switch call.method {
    case "createWindow":
      DispatchQueue.main.async {
        if let args = call.arguments as? String,
          let arguments = decodeJSON(from: args, to: MdCallArguments.self)
        {
          let window = MdWindowManager.instance.createWindow(
            id: arguments.windowID, windowStyle: arguments.windowStyle!, route: arguments.initRoute,
            params: arguments.extraParams)
          MdWindowManager.instance.addWindowAndNotifyAll(
            windowID: arguments.windowID, window: window)
          result(window.id)
        } else {
          result("")
        }
      }
    case "action":
      guard let args = call.arguments as? String else {
        result(false)
        return
      }
      actionToNative(args, result)
    case "getAllWindowIDs":
      DispatchQueue.main.async {
        result(MdWindowManager.instance.getAllWindowIDs())
      }
    case "getPlatformVersion":
      result("macOS " + ProcessInfo.processInfo.operatingSystemVersionString)
    default:
      result(FlutterMethodNotImplemented)
    }
  }

  public static var onWindowCreated: ((FlutterViewController) -> Void)? = nil

  public static func setOnWindowCreated(_ callback: @escaping (FlutterViewController) -> Void) {
    onWindowCreated = callback
  }

}
