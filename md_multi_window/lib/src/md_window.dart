import 'dart:ui';
import 'package:md_multi_window/md_multi_window.dart';
import 'package:md_multi_window/md_multi_window_platform_interface.dart';

class MdWindow {
  final String id;
  final MdMultiWindowPlatform _platform;
  MdWindow(this.id) : _platform = MdMultiWindowPlatform.instance;

  Future<bool> close() async {
    return _platform.doAction(MdMultiWindow.currentWindow.id, id, "close");
  }

  Future<bool> show() async {
    return _platform.doAction(MdMultiWindow.currentWindow.id, id, "show");
  }

  Future<bool> hide() async {
    return _platform.doAction(MdMultiWindow.currentWindow.id, id, "hide");
  }

  Future<bool> center() async {
    return _platform.doAction(MdMultiWindow.currentWindow.id, id, "center");
  }

  Future<bool> setTitle(
    String title,
  ) async {
    return _platform.doAction(MdMultiWindow.currentWindow.id, id, "center",
        params: {'setTitle': title});
  }

  Future<bool> setFrame(Offset offset, Size size,
      {bool keepCenter = false}) async {
    return _platform
        .doAction(MdMultiWindow.currentWindow.id, id, "setFrame", params: {
      'setFrame.x': offset.dx.toString(),
      'setFrame.y': offset.dy.toString(),
      'setFrame.w': size.width.toString(),
      'setFrame.h': size.height.toString(),
      "setFrame.k": keepCenter.toString(),
    });
  }

  Future<bool> preventClose(
    bool b,
  ) async {
    var params = {'preventClose': 'true'};
    if (!b) {
      params['preventClose'] = 'false';
    }
    return _platform.doAction(
        MdMultiWindow.currentWindow.id, id, "preventClose",
        params: params);
  }
}
