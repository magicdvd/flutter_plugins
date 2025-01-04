import 'dart:convert';
import 'dart:io';
import 'package:flutter/widgets.dart';
import 'package:md_multi_window/md_multi_window_platform_macos.dart';
import 'package:md_multi_window/src/md_window.dart';
import 'package:md_multi_window/src/md_defines.dart';
import 'package:md_multi_window/src/md_window_bridge.dart';
import 'package:md_multi_window/src/md_window_delegate.dart';
import 'package:uuid/uuid.dart';

import 'md_multi_window_platform_interface.dart';

export 'package:md_multi_window/src/md_defines.dart';
export 'package:md_multi_window/src/md_window.dart';
export 'package:md_multi_window/src/md_window_delegate.dart';
export 'package:md_multi_window/src/md_window_bridge.dart';

const _callArgName = 'md_multi_window';
const _windowIdMain = 'md_mulit_window_main';

T? _ambiguate<T>(T? value) => value;

class MdMultiWindow {
  static get defaultMainWindowID => _windowIdMain;
  static Future<String?> getPlatformVersion() {
    return MdMultiWindowPlatform.instance.getPlatformVersion();
  }

  static MdWindow? _current;
  static MdWindowBridge? messager;
  static MdWindowDelegate? delegate;
  static MdWindow get currentWindow {
    if (_current == null) {
      throw Exception(
          '[MdMultiWindow.ensureInitialized] should be called first');
    }
    return _current!;
  }

  static final Map<String, MdWindow> _windowsMap = {};
  static bool _intialized = false;

  // getWindow get the [MdWindow] with ID specified
  static MdWindow? getWindow(String id) {
    if (_windowsMap.containsKey(id)) {
      return _windowsMap[id];
    }
    return null;
  }

  // removeWindow do not use it outside
  static void removeWindow(String id) {
    _windowsMap.remove(id);
  }

  // addWindow add window
  static void addWindow(String id) {
    _windowsMap[id] = MdWindow(id);
  }

  // getWindowIDs get all windowID
  static List<String> getWindowIDs() {
    return _windowsMap.keys.toList();
  }

  // get main screen size
  static Future<Size?> getMainScreenSize() async {
    return MdMultiWindowPlatform.instance.getMainScreenSize();
  }

  // ensureInitialized
  // flutter main entry should be changed to main(List<String> args)
  // the args is the parameters for this function
  static Future<(MdWindow, String, Map<String, dynamic>)> ensureInitialized(
      List<String> args,
      {String firstWindowName = _windowIdMain}) async {
    if (Platform.isMacOS) {
      MdMultiWindowPlatform.instance = MdMultiWindowPlatformMacOS();
    } else {
      throw UnsupportedError('this plugin support macos only');
    }
    final String windowID;
    final Map<String, dynamic> arguments;
    final String initRoute;
    if (args.firstOrNull == _callArgName) {
      windowID = args[1];
      initRoute = args[2];
      arguments = args[3].isEmpty
          ? const {}
          : jsonDecode(args[3]) as Map<String, dynamic>;
    } else {
      windowID = firstWindowName;
      arguments = const {};
      initRoute = '';
    }
    if (_intialized) {
      throw Exception('duplicate MdMultiWindow.ensureInitialized called');
    }
    // temp method channel to ensure initialized
    final ids = await MdMultiWindowPlatform.instance.getAllWindowIDs();
    for (String id in ids) {
      final window = MdWindow(id);
      if (id == windowID) {
        if (_current != null) {
          throw ArgumentError('duplicate windowID:$windowID');
        }
        _current = window;
      }
      _windowsMap[id] = window;
    }
    if (_current == null) {
      throw ArgumentError('current windowID:$windowID has not been created');
    }
    _intialized = true;
    return (_current!, initRoute, arguments);
  }

  // widgetsDidLoad after widgets has loaded
  static widgetsDidLoad(VoidCallback callback, {bool showWindow = false}) {
    _ambiguate(WidgetsBinding.instance)!
        .waitUntilFirstFrameRasterized
        .then((value) {
      MdMultiWindowPlatform.instance
          .doAction(currentWindow.id, currentWindow.id, "canBeShown");
      callback();
      if (showWindow) {
        currentWindow.show();
      }
    });
  }

  // createWindow create window with specified style
  static Future<String?> createWindow(
      {String? windowID,
      WindowStyle style = WindowStyle.defaultStyle,
      String? initRoute,
      Map<String, String>? extraParams = const {'test': 'test2'}}) async {
    String wid;
    if (windowID == null) {
      var uuid = Uuid();
      wid = uuid.v4();
    } else {
      wid = windowID;
    }
    var args = MdChannelArguments(
        windowID: wid,
        windowStyle: style,
        initRoute: initRoute,
        params: extraParams);
    MdMultiWindowPlatform.instance.createWindow(args);
    return null;
  }
}
