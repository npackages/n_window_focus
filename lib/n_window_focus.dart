import 'data/n_app_window_dto.dart';
import 'n_window_focus_platform_interface.dart';

class NWindowFocus {
  Future<String?> getPlatformVersion() {
    return NWindowFocusPlatform.instance.getPlatformVersion();
  }

  void addFocusChangeListener(Function(NAppWindowDto) listener) {
    return NWindowFocusPlatform.instance.addFocusChangeListener(listener);
  }

  void removeFocusChangeListener(Function(NAppWindowDto) listener) {
    return NWindowFocusPlatform.instance.removeFocusChangeListener(listener);
  }

  void setIdleThreshold({Duration duration = const Duration(seconds: 10)}) {
    return NWindowFocusPlatform.instance.setIdleThreshold(duration);
  }

  void addUserActiveListener(Function(bool) listener) {
    return NWindowFocusPlatform.instance.addUserActiveListener(listener);
  }

  void removeUserActiveListener(Function(bool) listener) {
    return NWindowFocusPlatform.instance.removeUserActiveListener(listener);
  }

  bool get isUserActive {
    return NWindowFocusPlatform.instance.isUserActive;
  }

  Future<Duration> get idleThreshold async {
    return NWindowFocusPlatform.instance.idleThreshold;
  }
}
