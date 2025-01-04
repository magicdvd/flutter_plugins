import 'dart:convert';
import 'package:flutter/services.dart';
import 'package:md_multi_window/md_multi_window.dart';
import 'md_multi_window_platform_interface.dart';

const methodChannel = MethodChannel('magicd/md_multi_window/method');

/// An implementation of [MdMultiWindowPlatform] that uses method channels.
class MdMultiWindowPlatformMacOS extends MdMultiWindowPlatform {
  /// The method channel used to interact with the native platform.
  @override
  Future<String?> getPlatformVersion() async {
    final version =
        await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }

  @override
  Future<List<String>> getAllWindowIDs() async {
    final result =
        await methodChannel.invokeMethod<List<dynamic>>('getAllWindowIDs');
    final ids = result?.cast<String>() ?? const [];
    return ids;
  }

  @override
  Future<Size?> getMainScreenSize() async {
    final map = await methodChannel
        .invokeMethod<Map<dynamic, dynamic>>('mainScreenSize');
    return Size(
      map!['w'],
      map['h']!,
    );
  }

  @override
  Future<void> createWindow(MdChannelArguments args) async {
    await methodChannel.invokeMethod<String>('createWindow', jsonEncode(args));
  }

  MdMultiWindowPlatformMacOS() {
    methodChannel.setMethodCallHandler(_handle);
  }

  @override
  Future<bool> doAction(String caller, String callee, String action,
      {Map<String, String>? params}) async {
    Map<String, String> iparams = params ?? {};
    iparams['name'] = action;
    final args = MdChannelArguments(
        windowID: caller, targetWindowID: callee, params: iparams);
    var ret =
        await methodChannel.invokeMethod<bool>('action', jsonEncode(args));
    return ret ?? false;
  }

  Future<dynamic> _handle(MethodCall call) async {
    switch (call.method) {
      case 'notifyWindowCreated':
        final windowID = call.arguments;
        MdMultiWindow.addWindow(windowID);
      case 'notifyWindowClose':
        final windowID = call.arguments;
        MdMultiWindow.removeWindow(windowID);
      case 'recieveData':
        final args = call.arguments as Map<dynamic, dynamic>;
        Map<String, String> params = args.map((key, value) {
          return MapEntry(key.toString(), value.toString());
        });
        if (params.containsKey('sender')) {
          MdMultiWindow.messager?.recieveData(params['sender']!, params);
        }
      case 'onHide':
        MdMultiWindow.delegate?.onHide();
      case 'onClose':
        MdMultiWindow.delegate?.onClose();
      case 'onMinmize':
        MdMultiWindow.delegate?.onMinimize();
      case 'onShouldClose':
        final String windowID = call.arguments;
        if (MdMultiWindow.delegate != null) {
          final ret = await MdMultiWindow.delegate!.shouldClose();
          if (ret) {
            doAction(windowID, windowID, "preventCloseEnd",
                params: {'preventCloseEnd': 'true'});
          } else {
            doAction(windowID, windowID, "preventCloseEnd",
                params: {'preventCloseEnd': 'false'});
          }
        }
      default:
        throw UnimplementedError('${call.method} is not supported');
    }
  }
}
