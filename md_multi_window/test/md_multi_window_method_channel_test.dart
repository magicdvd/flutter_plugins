import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:md_multi_window/md_multi_window_platform_macos.dart';

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();

  MethodChannelMdMultiWindow platform = MethodChannelMdMultiWindow();
  const MethodChannel channel = MethodChannel('md_multi_window');

  setUp(() {
    TestDefaultBinaryMessengerBinding.instance.defaultBinaryMessenger
        .setMockMethodCallHandler(
      channel,
      (MethodCall methodCall) async {
        return '42';
      },
    );
  });

  tearDown(() {
    TestDefaultBinaryMessengerBinding.instance.defaultBinaryMessenger
        .setMockMethodCallHandler(channel, null);
  });

  test('getPlatformVersion', () async {
    expect(await platform.getPlatformVersion(), '42');
  });
}
