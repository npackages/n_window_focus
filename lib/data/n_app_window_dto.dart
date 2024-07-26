class NAppWindowDto {
  final String appName;
  final String windowTitle;

  NAppWindowDto({required this.appName, required this.windowTitle});

  @override
  String toString() {
    return 'Window title: $windowTitle. AppName $appName';
  }
}
