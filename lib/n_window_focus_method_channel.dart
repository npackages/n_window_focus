import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'data/n_app_window_dto.dart';
import 'event_bus.dart';
import 'n_window_focus_platform_interface.dart';

/// An implementation of [NWindowFocusPlatform] that uses method channels.
class MethodChannelNWindowFocus extends NWindowFocusPlatform {
  final _eventBus = EventBus();
  bool userActive = true;

  MethodChannelNWindowFocus() {
    methodChannel.setMethodCallHandler((call) async {
      if (call.method == 'onFocusChange') {
        _eventBus.fireEvent<NAppWindowDto>(NAppWindowDto(
            appName: call.arguments['appName'],
            windowTitle: call.arguments['windowTitle']));
      } else if (call.method == 'onUserActiveChange') {
        userActive = call.arguments;
        _eventBus.fireEvent<bool>(userActive);
      } else {
        print(call.method);
      }
    });
  }

  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('n_window_focus');

  @override
  Future<String?> getPlatformVersion() async {
    final version =
        await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }

  @override
  void addFocusChangeListener(Function(NAppWindowDto) listener) {
    _eventBus.addListener(listener);
  }

  @override
  void removeFocusChangeListener(Function(NAppWindowDto) listener) {
    _eventBus.removeListener(listener);
  }

  @override
  void setIdleThreshold(Duration duration) {
    methodChannel
        .invokeMethod('setIdleThreshold', {'threshold': duration.inSeconds});
  }

  @override
  bool get isUserActive {
    return userActive;
  }

  @override
  void removeUserActiveListener(Function(bool) listener) {
    _eventBus.removeListener(listener);
  }

  @override
  void addUserActiveListener(Function(bool) listener) {
    _eventBus.addListener(listener);
  }

  @override
  Future<Duration> get idleThreshold async {
    try {
      final res = await methodChannel.invokeMethod<int>('getIdleThreshold');
      return Duration(seconds: res ?? 10);
    } catch (e) {
      return Duration(seconds: 10);
    }
  }
}
