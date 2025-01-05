# md_multi_window

a mulit window support for macos

Refer to the code of the three plugins

- desktop_mulit_window [https://github.com/MixinNetwork/flutter-plugins/tree/main/packages/desktop_multi_window]
- window_manager [https://github.com/leanflutter/window_manager]
- bitsdojo_window [https://github.com/bitsdojo/bitsdojo_window]

MacOS support only

## mac

create first window manually without xib

1. remove MainMenu.xib from project
2. modify AppDelegate.swift

```dart
//macos/Runner/AppDelegate.swift
//import pacakge
import md_multi_window
```

```dart
// applicationDidFinishLaunching, create custom window, make its lifecycle controlled by md_multi_window
override func applicationDidFinishLaunching(_ notification: Notification) {
    // frame size
    let rect = NSRect(x: 0, y: 0, width: 800, height: 600)
    // create window (customize your window)
    let window = MdFlutterWindow(
      contentRect: rect,
      styleMask: [.titled, .fullSizeContentView, .closable, .resizable, .miniaturizable],
      backing: .buffered,
      defer: false)
    // very important!
    window.isReleasedWhenClosed = false
    // create view controller with flutter engine
    let flutterViewController = FlutterViewController()
    // set window's view controllerr
    window.contentViewController = flutterViewController
    // customize your window style as your wish
    window.titleVisibility = .hidden
    window.titlebarAppearsTransparent = true
    // set the frame size, keep it resize to size exactly
    window.setFrame(rect, display: true)
    window.center()
    // hide on launch ( main() MdMultiWindow.widgetsDidLoad((){}, showWindow: true))
    window.hideOnLaunch = true
    // whether to terminate the app while this window is the last closed window
    window.lastWindowClosedShouldTerminateApp = true
    // register the flutter's plugins with this controller
    RegisterGeneratedPlugins(registry: flutterViewController)
    // important!!! create channel with this view controller with this window
    MdMultiWindowPlugin.attachChannelWithMain(with: flutterViewController, window: window)
    // important!!! set callback after window creation, it is used to register flutter plugin with other created window
    MdMultiWindowPlugin.setOnWindowCreated { viewController in
      RegisterGeneratedPlugins(registry: viewController)
    }
    // show the window (if hideOnLaunch is true, winodw will not be shown)
    window.makeKeyAndOrderFront(nil)
  }
```

```dart
// applicationShouldTerminateAfterLastWindowClosed (you can make app terminated or not by last window is closed/hide)
// determined by thie last MdFlutterWindow's property [lastWindowClosedShouldTerminateApp]
override func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
    return MdMultiWindowPlugin.shouldTerminateApp
}
```

```dart
// applicationShouldHandleReopen which is called while user is clicking the app icon on the dock
// it is used to make window shown after the window is hidden by 'window.hide()'
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
```

## flutter

change main() function to main(List<String> args)

```dart
void main(List<String> arguments) async {
  WidgetsFlutterBinding.ensureInitialized();
  // make md_mulit_window initialized
  await MdMultiWindow.ensureInitialized(arguments);
  runApp(const MyApp());
  // you can do something after the wigets is loaded
  // important: if you make hideOnLaunch = true, showWindow should be true, otherwise false
  MdMultiWindow.widgetsDidLoad(() {}, showWindow: true);
}

```

Optional

1. prevent close, you could do some external logical when 'window.performClose' is trigger , or the close button is clicked on title bar

```dart
// prevent any window on close action
void preventClose(String id) {
    // get any window
    var window = MdMultiWindow.getWindow(id);
    if (window != null) {
        // the window will not be closeable
        window.preventClose(true);
    } else {
        logMessage('window not exist');
    }
}
```

```dart
// delegate on shouldClose to do something
class _MyAppState extends State<MyApp> with MdWindowDelegate
    @override
    void initState() {
        super.initState();
        // make this state to a delegator
        MdMultiWindow.delegate = this;
    }

    // the callback is called , when the window which prevented close in close lifecycle (window.performClose is trigger , or the close button on titlebar is clicked)
    // the window will be closed , while the future result is true,
    // otherwise nothing happen.
    @override
    Future<bool> shouldClose() async {
        logMessage("onShouldClose");
        // delay close (example delay return true)
        return Future.delayed(Duration(seconds: 5), () => true);
    }
}
```

2. Commuication with windows

```dart
class _MyAppState extends State<MyApp> with MdWindowBridge
    @override
    void initState() {
        super.initState();
        // make this state to a messager
        MdMultiWindow.messager = this;
    }

    //send data to window specified
    void exampleSendData() {
        // this function is defined in mixin MdWindowBridge
        sendData('targetWindowID', {'any key': 'any value'});
    }

    //send data to all windows (include self)
    void broadcastDataToAll() {
        broadcastData({'send from (brocast)': MdMultiWindow.currentWindow.id});
    }

    //recieve data
    @override
    void recieveData(String senderWindowID, Map<String, String> params) {
        logMessage('recievedata $senderWindowID, $params');
    }

```

3. title bar button position (traffic lights)

refer to [https://github.com/leanflutter/window_manager/issues/333]

![titlebar](https://github.com/magicdvd/flutter_plugins/blob/main/md_multi_window/resources/titlebar.jpg)

- swift, create MdFlutterWindow

```dart
// trafficLightsOffset: title content traffli lights (close button)view's bottomLeft point's offset (x: negative value = to left, positive = to right, y: negative value = down, positive = up)
// trafficLightsSpacingFix: the fix is for the space between buttons, two spaces ([close]<->[min]<-> [fullscreen]), they are equal ( negative value: means reduce the space, positive value: increase the space)
let window = MdFlutterWindow(
      contentRect: NSRect(x:0,y:0,widht:100,height:100),
      styleMask: [.titled, .closable, .resizable, .miniaturizable],
      backing: .buffered,
      defer: false, trafficLightsOffset: CGPoint(0,0),trafficLightsSpacingFix: -5)
```

- dart, create new window

```dart
MdMultiWindow.createWindow(
          windowID: nid,
          style: WindowStyle(
              style: StyleMaskMix.normal,
              hideOnLaunch: true,
              titleShow: true,
              title: nid,
              center: true,
              size: Size(800, 600),
              trafficLightsOffset: Offset(0,0),
              trafficLightsSpacingFix: -5
              ));
```
