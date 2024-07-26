import 'package:flutter_test/flutter_test.dart';
import 'package:n_window_focus/data/n_app_window_dto.dart';
import 'package:n_window_focus/n_window_focus.dart';
import 'package:n_window_focus/n_window_focus_platform_interface.dart';
import 'package:n_window_focus/n_window_focus_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockNWindowFocusPlatform
    with MockPlatformInterfaceMixin
    implements NWindowFocusPlatform {
  @override
  Future<String?> getPlatformVersion() => Future.value('42');

  @override
  void addFocusChangeListener(Function(NAppWindowDto p1) listener) {}

  @override
  void addUserActiveListener(Function(bool p1) listener) {}

  @override
  Future<Duration> get idleThreshold => throw UnimplementedError();

  @override
  bool get isUserActive => throw UnimplementedError();

  @override
  void removeFocusChangeListener(Function(NAppWindowDto p1) listener) {}

  @override
  void removeUserActiveListener(Function(bool p1) listener) {}

  @override
  void setIdleThreshold(Duration duration) {}
}

void main() {
  final NWindowFocusPlatform initialPlatform = NWindowFocusPlatform.instance;

  test('$MethodChannelNWindowFocus is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelNWindowFocus>());
  });

  test('getPlatformVersion', () async {
    NWindowFocus nWindowFocusPlugin = NWindowFocus();
    MockNWindowFocusPlatform fakePlatform = MockNWindowFocusPlatform();
    NWindowFocusPlatform.instance = fakePlatform;

    expect(await nWindowFocusPlugin.getPlatformVersion(), '42');
  });
}
