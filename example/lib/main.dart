import 'package:flutter/material.dart';
import 'package:n_window_focus/n_window_focus.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  String activeWindowTitle = 'Unknown';
  bool userIdle = false;
  final _nWindowFocusPlugin = NWindowFocus();

  @override
  void initState() {
    super.initState();
    _nWindowFocusPlugin.setIdleThreshold(duration: const Duration(seconds: 60));
    _nWindowFocusPlugin.addFocusChangeListener((p0) {
      setState(() {
        activeWindowTitle = p0.appName;
      });
    });
    _nWindowFocusPlugin.addUserActiveListener((p0) {
      setState(() {
        userIdle = p0;
      });
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
            child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            Text('Window title in focus: $activeWindowTitle\n'),
            Text('User is idle: ${!userIdle}\n'),
          ],
        )),
      ),
    );
  }
}
