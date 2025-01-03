import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:md_multi_window/md_multi_window.dart';

void main(List<String> arguments) async {
  WidgetsFlutterBinding.ensureInitialized();
  await MdMultiWindow.ensureInitialized(arguments);
  runApp(const MyApp());
  MdMultiWindow.widgetsDidLoad(() {}, showWindow: true);
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> with MdWindowDelegate, MdWindowBridge {
  String _platformVersion = 'Unknown';

  @override
  void initState() {
    super.initState();
    MdMultiWindow.delegate = this;
    MdMultiWindow.messager = this;
    initPlatformState();
  }

  void preventClose(String id) {
    var window = MdMultiWindow.getWindow(id);
    if (window != null) {
      window.preventClose(true);
    } else {
      debugPrint('window not exist');
    }
  }

  void openTest() {
    var ids = MdMultiWindow.getWindowIDs();
    if (ids.length == 3) {
      debugPrint('exampe create two specified window [w1, w2] only');
      return;
    }
    Map<String, bool> mp = {};
    for (var id in ids) {
      mp[id] = true;
    }
    for (var nid in [MdMultiWindow.defaultMainWindowID, 'w1', 'w2']) {
      if (mp.containsKey(nid)) {
        continue;
      }
      if (nid == 'w2') {
        MdMultiWindow.createWindow(
            windowID: nid,
            style: WindowStyle(
                trafficLightsOffset: Offset(5, -5),
                trafficLightsSpacingFix: 0,
                style: StyleMaskMix.hidden,
                hideOnLaunch: false,
                titleShow: true,
                title: nid,
                center: false,
                size: Size(0, 0)));
        continue;
      }

      MdMultiWindow.createWindow(
          windowID: nid,
          style: WindowStyle(
              trafficLightsOffset: Offset(10, -10),
              trafficLightsSpacingFix: -5,
              style: StyleMaskMix.normal,
              hideOnLaunch: true,
              titleShow: true,
              title: nid,
              center: true,
              size: Size(0, 0)));
    }
    // if (MdMultiWindow.currentWindow.id == MdMultiWindow.defaultMainWindowID) {
    //   MdMultiWindow.currentWindow.close();
    // }
  }

  void closeTest(String id) {
    var window = MdMultiWindow.getWindow(id);
    if (window != null) {
      debugPrint('${window.id} $id');
      window.close();
    } else {
      debugPrint('window not exist');
    }
  }

  void hideTest(String id) {
    var window = MdMultiWindow.getWindow(id);
    if (window != null) {
      debugPrint('${window.id} $id');
      window.hide();
    } else {
      debugPrint('window not exist');
    }
  }

  void showTest(String id) {
    var window = MdMultiWindow.getWindow(id);
    if (window != null) {
      debugPrint('${window.id} $id');
      window.show();
    } else {
      debugPrint('window not exist');
    }
  }

  void sendDataTo(String id) {
    sendData(id, {'send from': MdMultiWindow.currentWindow.id});
  }

  void brocastDataToAll() {
    brocastData({'send from (brocast)': MdMultiWindow.currentWindow.id});
  }

  @override
  void recieveData(String senderWindowID, Map<String, String> params) {
    debugPrint('recievedata $senderWindowID, $params');
  }

  @override
  void onClose() {
    debugPrint("callback onClose");
  }

  @override
  void onMinimize() {
    debugPrint("callback min");
  }

  @override
  void onHide() {
    debugPrint("callback hide");
  }

  @override
  Future<bool> shouldClose() async {
    debugPrint("onShouldClose");
    // delay close
    return Future.delayed(Duration(seconds: 5), () => true);
  }

  // Platform messages are asynchronous, so we initialize in an async method.
  Future<void> initPlatformState() async {
    String platformVersion;
    // Platform messages may fail, so we use a try/catch PlatformException.
    // We also handle the message potentially returning null.
    try {
      platformVersion = await MdMultiWindow.getPlatformVersion() ??
          'Unknown platform version';
    } on PlatformException {
      platformVersion = 'Failed to get platform version.';
    }

    // If the widget was removed from the tree while the asynchronous platform
    // message was in flight, we want to discard the reply rather than calling
    // setState to update our non-existent appearance.
    if (!mounted) return;

    setState(() {
      MdMultiWindow.currentWindow
          .setFrame(Offset.zero, Size(800, 600), keepCenter: true);
      _platformVersion = platformVersion;
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
          appBar: AppBar(
            title: const Text('Plugin example app'),
          ),
          body: Container(
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                Center(
                  child: Text('Running on: $_platformVersion\n'),
                ),
                Center(
                  child: ElevatedButton(
                      onPressed: () {
                        openTest();
                      },
                      child: Text('open w1 and w2')),
                ),
                Center(
                  child: ElevatedButton(
                      onPressed: () {
                        preventClose('w1');
                      },
                      child: Text('preventClose w1')),
                ),
                Center(
                  child: ElevatedButton(
                      onPressed: () {
                        closeTest('w1');
                      },
                      child: Text('close w1')),
                ),
                Center(
                  child: ElevatedButton(
                      onPressed: () {
                        closeTest('w2');
                      },
                      child: Text('close w2')),
                ),
                Center(
                  child: ElevatedButton(
                      onPressed: () {
                        sendDataTo('w2');
                      },
                      child: Text('send data to w2')),
                ),
                Center(
                  child: ElevatedButton(
                      onPressed: () {
                        brocastDataToAll();
                      },
                      child: Text('brocast data to all')),
                ),
                Center(
                  child: ElevatedButton(
                      onPressed: () {
                        hideTest('w1');
                      },
                      child: Text('hide w1')),
                ),
                Center(
                  child: ElevatedButton(
                      onPressed: () {
                        showTest('w1');
                      },
                      child: Text('show w1')),
                ),
              ],
            ),
          )),
    );
  }
}
