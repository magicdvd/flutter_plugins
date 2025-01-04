import 'package:flutter/foundation.dart';
import 'package:md_multi_window/md_multi_window.dart';
import 'package:md_multi_window/md_multi_window_platform_interface.dart';

mixin MdWindowBridge {
  Future<bool> sendData(String recvWindowID, Map<String, String> params) async {
    final win = MdMultiWindow.getWindow(recvWindowID);
    if (win == null) {
      debugPrint('window: $recvWindowID does not exist');
      return false;
    }
    return MdMultiWindowPlatform.instance.doAction(
        MdMultiWindow.currentWindow.id, recvWindowID, 'sendData',
        params: params);
  }

  Future<bool> broadcastData(Map<String, String> params) async {
    return MdMultiWindowPlatform.instance.doAction(
        MdMultiWindow.currentWindow.id,
        MdMultiWindow.currentWindow.id,
        'broadcastData',
        params: params);
  }

  // recieveData recieve data from sender
  void recieveData(String senderWindowID, Map<String, String> params) {}
}
