import Cocoa
import Foundation

open class MdFlutterWindow: NSWindow {
  var windowCanBeShown: Bool = false

  private var _hideOnLaunch: Bool = false

  public var hideOnLaunch: Bool {
    get {
      return _hideOnLaunch
    }
    set {
      if newValue {
        MdMultiWindowPlugin.windowInCreation = true
        MdMultiWindowPlugin.shouldTerminateApp = false
      }
      _hideOnLaunch = newValue
    }
  }

  private var inFullScreenTransition = false

  // whether self is the last window closed should terminate app
  open var lastWindowClosedShouldTerminateApp: Bool = false

  public init(
    contentRect: NSRect, styleMask style: NSWindow.StyleMask,
    backing backingStoreType: NSWindow.BackingStoreType, defer flag: Bool,
    trafficLightsOffset: CGPoint = CGPoint.zero, trafficLightsSpacingFix: CGFloat = CGFloat.zero
  ) {
    super.init(
      contentRect: contentRect, styleMask: style, backing: backingStoreType, defer: flag)

    if trafficLightsOffset == CGPoint.zero && trafficLightsSpacingFix == CGFloat.zero {
      return
    }
    // traffic lights position must be recalculated on any resize step
    NotificationCenter.default.addObserver(
      forName: NSWindow.didResizeNotification, object: self, queue: OperationQueue.main
    ) { [weak self] Notification in
      guard let self = self else {
        return
      }
      self.adjustTrafficLights(offset: trafficLightsOffset, spacingFix: trafficLightsSpacingFix)
    }

    // during exiting from full screen reposition works bad. This part fixes behavior
    NotificationCenter.default.addObserver(
      forName: NSWindow.willExitFullScreenNotification, object: self, queue: OperationQueue.main
    ) { [weak self] Notification in
      guard let self = self else {
        return
      }
      self.inFullScreenTransition = true
      self.adjustTrafficLights(offset: trafficLightsOffset, spacingFix: trafficLightsSpacingFix)
    }
    NotificationCenter.default.addObserver(
      forName: NSWindow.didExitFullScreenNotification, object: self, queue: OperationQueue.main
    ) { [weak self] Notification in
      guard let self = self else {
        return
      }
      self.inFullScreenTransition = false
      self.adjustTrafficLights(offset: trafficLightsOffset, spacingFix: trafficLightsSpacingFix)
    }
  }

  // public func unbindObserver() {
  //   NotificationCenter.default.removeObserver(self)
  // }

  deinit {
    NotificationCenter.default.removeObserver(self)
    logMessage("MdFlutterWindow released")
  }

  override public func order(_ place: NSWindow.OrderingMode, relativeTo otherWin: Int) {
    super.order(place, relativeTo: otherWin)
    if !(windowCanBeShown) && hideOnLaunch && self.isVisible {
      self.setIsVisible(false)
    }
  }

  func adjustTrafficLights(offset: CGPoint, spacingFix spacing: CGFloat) {
    let close = self.standardWindowButton(NSWindow.ButtonType.closeButton)!
    let miniaturize = self.standardWindowButton(NSWindow.ButtonType.miniaturizeButton)!
    let zoom = self.standardWindowButton(NSWindow.ButtonType.zoomButton)!
    let titleBarContainerView = close.superview?.superview

    if titleBarContainerView == nil {
      return
    }
    if inFullScreenTransition {
      titleBarContainerView?.isHidden = true
      return
    }
    var titleView: NSView?
    if let subviews = close.superview?.subviews {
      for subview in subviews {
        if subview as? NSTextField != nil {
          titleView = subview
          break
        }
      }
    }
    titleBarContainerView!.isHidden = false
    let buttonHeight = close.frame.size.height
    //let titleBarFrameHeight = buttonHeight + offset.y * 2
    var titleBarRect = titleBarContainerView!.frame
    if titleBarRect.size.height - 2 * offset.y > buttonHeight {
      titleBarRect.size.height -= 2 * offset.y
      titleBarRect.origin.y += 2 * offset.y
      titleBarContainerView!.frame = titleBarRect
      if titleView != nil {
        var titleFrame = titleView!.frame
        titleFrame.origin.y += offset.y
        titleView!.frame = titleFrame
      }
    } else {
      logMessage("macos:", "titleBar offset y is more than the height of button, do nothing")
    }
    let windowButtons = [close, miniaturize, zoom]
    var spaceBetween = miniaturize.frame.origin.x - close.frame.origin.x - close.frame.size.width
    if spaceBetween + spacing > 0 {
      spaceBetween = spaceBetween + spacing
    }
    var offsetX = close.frame.origin.x + offset.x
    for i in 0...2 {
      let view = windowButtons[i]
      var rect = view.frame
      rect.origin.x = offsetX
      rect.origin.y = (titleBarRect.size.height - rect.size.height) / 2
      offsetX = rect.origin.x + rect.size.width + spaceBetween
      view.setFrameOrigin(rect.origin)
    }
  }
}
