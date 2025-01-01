enum MdWindowDelegateMethod {
  willClose(0),
  shouldClose(1);

  final int value;
  const MdWindowDelegateMethod(this.value);
}

// ony refer the window which hold the flutter engine, per window per delegate
mixin MdWindowDelegate {
  // overridedMethods override this method to reduce channel call
  List<MdWindowDelegateMethod>? overridedMethods() {
    return null;
  }

  // window is about to close
  void onClose() {}

  // window is about to hidden
  void onHide() {}

  // window is about to minimized
  void onMinimize() {}

  // shouldClose [action:preventClose] called, when call window.close, or system close
  // 1. windw.preventClose(true)
  // 2. window.close()
  // 3. delegate: shouldClose
  //    a. false: no thing happen
  //    b. true: deleate: onClose
  Future<bool> shouldClose() async => true;
}
