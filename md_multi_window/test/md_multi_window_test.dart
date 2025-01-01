import 'package:flutter_test/flutter_test.dart';
import 'package:md_multi_window/md_multi_window.dart';
import 'package:md_multi_window/md_multi_window_platform_interface.dart';
import 'package:md_multi_window/md_multi_window_platform_macos.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockMdMultiWindowPlatform
    with MockPlatformInterfaceMixin
    implements MdMultiWindowPlatform {
  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final MdMultiWindowPlatform initialPlatform = MdMultiWindowPlatform.instance;

  test('$MethodChannelMdMultiWindow is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelMdMultiWindow>());
  });

  test('getPlatformVersion', () async {
    MdMultiWindow mdMultiWindowPlugin = MdMultiWindow();
    MockMdMultiWindowPlatform fakePlatform = MockMdMultiWindowPlatform();
    MdMultiWindowPlatform.instance = fakePlatform;

    expect(await mdMultiWindowPlugin.getPlatformVersion(), '42');
  });
}
