import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'data/n_app_window_dto.dart';
import 'n_window_focus_method_channel.dart';

abstract class NWindowFocusPlatform extends PlatformInterface {
  /// Constructs a NWindowFocusPlatform.
  NWindowFocusPlatform() : super(token: _token);

  static final Object _token = Object();

  static NWindowFocusPlatform _instance = MethodChannelNWindowFocus();

  /// The default instance of [NWindowFocusPlatform] to use.
  ///
  /// Defaults to [MethodChannelNWindowFocus].
  static NWindowFocusPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [NWindowFocusPlatform] when
  /// they register themselves.
  static set instance(NWindowFocusPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }

  void addFocusChangeListener(Function(NAppWindowDto) listener) {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }

  void removeFocusChangeListener(Function(NAppWindowDto) listener) {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }

  void setIdleThreshold(Duration duration) {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }

  bool get isUserActive {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }

  Future<Duration> get idleThreshold {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }

  void addUserActiveListener(Function(bool) listener) {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }

  void removeUserActiveListener(Function(bool) listener) {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
}
