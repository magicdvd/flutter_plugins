import Cocoa
import Foundation

open class MdFlutterWindow: NSWindow {
  var windowCanBeShown: Bool = false

  open var hideOnLaunch: Bool = false

  override public var canBecomeKey: Bool {
    return true
  }

  override public func order(_ place: NSWindow.OrderingMode, relativeTo otherWin: Int) {
    super.order(place, relativeTo: otherWin)
    if !(windowCanBeShown) && hideOnLaunch && self.isVisible {
      self.setIsVisible(false)
    }
  }
}