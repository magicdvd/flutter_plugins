import Cocoa
import FlutterMacOS
import md_multi_window

class MainFlutterWindow: NSWindow {
  override func awakeFromNib() {
    let flutterViewController = FlutterViewController()
    let windowFrame = self.frame
    self.contentViewController = flutterViewController
    self.setFrame(windowFrame, display: true)

    RegisterGeneratedPlugins(registry: flutterViewController)

    MdMultiWindowPlugin.attachChannelWithMain(with: flutterViewController)
    MdMultiWindowPlugin.setOnWindowCreated { viewController in
      RegisterGeneratedPlugins(registry: viewController)
    }
    super.awakeFromNib()

  }

  // override public func order(_ place: NSWindow.OrderingMode, relativeTo otherWin: Int) {
  //   super.order(place, relativeTo: otherWin)
  //   self.setIsVisible(false)
  // }
}
