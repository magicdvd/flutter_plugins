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
  final Size size;
  final StyleMaskMix style;
  final Offset offset;
  final bool center;
  final String title;
  final bool titleShow;
  const WindowStyle(
      {this.offset = const Offset(0, 0),
      this.size = const Size(800, 600),
      this.style = StyleMaskMix.normal,
      this.center = true,
      this.title = '',
      this.titleShow = true});
  // 自定义 toJson 方法
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
      'tts': titleShow
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
