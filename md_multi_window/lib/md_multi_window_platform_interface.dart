import 'dart:ui';

import 'package:md_multi_window/md_multi_window.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

abstract class MdMultiWindowPlatform extends PlatformInterface {
  /// Constructs a MdMultiWindowPlatform.
  MdMultiWindowPlatform() : super(token: _token);

  static final Object _token = Object();

  static MdMultiWindowPlatform? _instance;

  /// The default instance of [MdMultiWindowPlatform] to use.
  ///
  /// Defaults to [MethodChannelMdMultiWindow].
  static MdMultiWindowPlatform get instance {
    if (_instance == null) {
      throw Exception(
          '[MdMultiWindow.ensureInitialized] should be called first');
    }
    return _instance!;
  }

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [MdMultiWindowPlatform] when
  /// they register themselves.
  static set instance(MdMultiWindowPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion();

  Future<List<String>> getAllWindowIDs();

  Future<Size?> getMainScreenSize();

  Future<void> createWindow(MdChannelArguments args);

  Future<bool> doAction(String caller, String callee, String action,
      {Map<String, String>? params});
}
