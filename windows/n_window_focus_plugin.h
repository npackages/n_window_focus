#ifndef FLUTTER_PLUGIN_N_WINDOW_FOCUS_PLUGIN_H_
#define FLUTTER_PLUGIN_N_WINDOW_FOCUS_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace n_window_focus {

class NWindowFocusPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);
  void SetMethodChannel(std::shared_ptr<flutter::MethodChannel<flutter::EncodableValue>> method_channel);

  NWindowFocusPlugin();

  virtual ~NWindowFocusPlugin();

  // Disallow copy and assign.
  NWindowFocusPlugin(const NWindowFocusPlugin&) = delete;
  NWindowFocusPlugin& operator=(const NWindowFocusPlugin&) = delete;

  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
private:

    std::shared_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel;
    void CheckForInactivity();
    void StartFocusListener();
};

}  // namespace n_window_focus

#endif  // FLUTTER_PLUGIN_N_WINDOW_FOCUS_PLUGIN_H_
