#include "include/n_window_focus/n_window_focus_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "n_window_focus_plugin.h"

void NWindowFocusPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  n_window_focus::NWindowFocusPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
