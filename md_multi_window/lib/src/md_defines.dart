import 'dart:ui';

enum StyleMask {
  titled(1),
  closable(2),
  resizable(3),
  miniaturizable(4),
  fullScreen(5),
  borderless(6),
  fullSizeContentView(7);

  final int value;
  const StyleMask(this.value);
}

class StyleMaskMix {
  final bool titlebarAppearsTransparent;
  static const normal = StyleMaskMix([
    StyleMask.titled,
    StyleMask.miniaturizable,
    StyleMask.closable,
    StyleMask.resizable,
  ], false);
  static const hidden = StyleMaskMix([
    StyleMask.titled,
    StyleMask.miniaturizable,
    StyleMask.closable,
    StyleMask.resizable,
    StyleMask.fullSizeContentView
  ], true);
  final List<StyleMask> list;
  const StyleMaskMix(this.list, this.titlebarAppearsTransparent);
  List<int> get masks {
    List<int> r = [];
    for (var v in list) {
      r.add(v.value);
    }
    return r;
  }
}

class WindowStyle {
  static const WindowStyle defaultStyle = WindowStyle();
  static const double mainScreenHeight =
      -4321; //magic number for main screen size (only use for native code, do not use it in flutter side)
  static const double mainScreenWidth =
      -1234; //magic number for main screen size (only use for native code, do not use it in flutter side)
  final Size size;
  final StyleMaskMix style;
  final Offset offset;
  final Size minSize;
  final Size maxSize;
  final bool center;
  final String title;
  final bool titleShow;
  final bool hideOnLaunch;
  final bool lastWindowClosedShouldTerminateApp;
  final double trafficLightsSpacingFix;
  final Offset trafficLightsOffset;
  const WindowStyle(
      {this.offset = const Offset(0, 0),
      this.size = const Size(800, 600),
      this.style = StyleMaskMix.normal,
      this.center = true,
      this.title = '',
      this.titleShow = true,
      this.hideOnLaunch = false,
      this.lastWindowClosedShouldTerminateApp = false,
      this.trafficLightsOffset = const Offset(0, 0),
      this.trafficLightsSpacingFix = 0,
      this.minSize = const Size(0, 0),
      this.maxSize = const Size(mainScreenWidth, mainScreenHeight)});

  Map<String, dynamic> toJson() {
    return {
      'h': size.height,
      'w': size.width,
      'x': offset.dx,
      'y': offset.dy,
      't': style.masks,
      'ts': style.titlebarAppearsTransparent,
      'c': center,
      'tt': title,
      'tts': titleShow,
      'i': hideOnLaunch,
      'l': lastWindowClosedShouldTerminateApp,
      'tb': trafficLightsSpacingFix,
      'tx': trafficLightsOffset.dx,
      'ty': trafficLightsOffset.dy,
      'iw': minSize.width,
      'ih': minSize.height,
      'aw': maxSize.width,
      'ah': maxSize.height,
    };
  }
}

// MdChannelArguments 参数转化成json传递给
// {
//  'id': 'xxxx-xxxx-xxxx-x-xxx', //selfWindowID
//  'tid': 'xxxx-xxxx-xxxx-xxxxx', //targetWindowID
//  'st': {
//     'h': 600,
//     'w': 800,
//     'x': 0,
//     'y': 0,
//     't': 3,
//     'ts': true,
//     'c': true,
//   },
//   'route': '/main',
//   'params' {
//     'argc1': 'argv1',
//     'argc2': 'argv2',
//     'argc3': 'argv3',
//     ...
//   }
//
class MdChannelArguments {
  final String windowID;
  final String? targetWindowID;
  final WindowStyle? windowStyle;
  final String? initRoute;
  final Map<String, String>? params;
  MdChannelArguments(
      {required this.windowID,
      this.targetWindowID,
      this.windowStyle,
      this.initRoute,
      this.params});
  Map<String, dynamic> toJson() {
    final Map<String, dynamic> mp = {'id': windowID};
    if (targetWindowID != null) {
      mp['tid'] = targetWindowID!;
    }
    if (windowStyle != null) {
      mp['st'] = windowStyle!.toJson();
    }
    if (initRoute != null) {
      mp['route'] = initRoute!;
    }
    if (params != null) {
      mp['params'] = params;
    }
    return mp;
  }
}
