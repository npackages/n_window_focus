#include "n_window_focus_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>
#include <flutter/binary_messenger.h>
#include <flutter/encodable_value.h>

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <codecvt>
#include <locale>
#include <optional>
#include <tlhelp32.h>
#include <psapi.h>
#include <chrono>
#include <sstream>

namespace n_window_focus {

HHOOK keyboardHook;
HHOOK mouseHook;
std::chrono::steady_clock::time_point lastActivityTime;
int inactivityThreshold = 5;  // Default threshold in seconds

using CallbackMethod = std::function<void(const std::wstring&)>;

void UpdateLastActivityTime() {
    lastActivityTime = std::chrono::steady_clock::now();
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        UpdateLastActivityTime();
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        UpdateLastActivityTime();
    }
    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

void SetHooks() {
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, nullptr, 0);
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, nullptr, 0);
}

void RemoveHooks() {
    if (keyboardHook) UnhookWindowsHookEx(keyboardHook);
    if (mouseHook) UnhookWindowsHookEx(mouseHook);
}

std::string ConvertWindows1251ToUTF8(const std::string& windows1251_str) {
    int size_needed = MultiByteToWideChar(1251, 0, windows1251_str.c_str(), -1, nullptr, 0);
    std::wstring utf16_str(size_needed, 0);
    MultiByteToWideChar(1251, 0, windows1251_str.c_str(), -1, &utf16_str[0], size_needed);

    size_needed = WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8_str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(), -1, &utf8_str[0], size_needed, nullptr, nullptr);

    return utf8_str;
}

std::string ConvertWStringToUTF8(const std::wstring& wstr) {
    if (wstr.empty()) return {};
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
    std::string utf8_str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &utf8_str[0], size_needed, nullptr, nullptr);
    return utf8_str;
}

void NWindowFocusPlugin::SetMethodChannel(std::shared_ptr<flutter::MethodChannel<flutter::EncodableValue>> method_channel) {
    channel = std::move(method_channel);
}

// static
void NWindowFocusPlugin::RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar) {
    auto channel = std::make_shared<flutter::MethodChannel<flutter::EncodableValue>>(
        registrar->messenger(), "n_window_focus", &flutter::StandardMethodCodec::GetInstance());

    auto plugin = std::make_unique<NWindowFocusPlugin>();
    plugin->SetMethodChannel(channel);

    channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result) {
            plugin_pointer->HandleMethodCall(call, std::move(result));
        });

    SetHooks();
    plugin->CheckForInactivity();
    plugin->StartFocusListener();

    registrar->AddPlugin(std::move(plugin));
}

std::string GetFocusedWindowTitle() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) return {};

    int length = GetWindowTextLength(hwnd);
    if (length == 0) return {};

    std::string buffer(length + 1, '\0');
    GetWindowTextA(hwnd, &buffer[0], length + 1);

    return buffer;
}

NWindowFocusPlugin::NWindowFocusPlugin() {}

NWindowFocusPlugin::~NWindowFocusPlugin() {
    RemoveHooks(); // Clean up hooks when the plugin is destroyed
}

void NWindowFocusPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
    if (method_call.method_name() == "getPlatformVersion") {
        std::ostringstream version_stream;
        version_stream << "Windows ";
        if (IsWindows10OrGreater()) {
            version_stream << "10+";
        } else if (IsWindows8OrGreater()) {
            version_stream << "8";
        } else if (IsWindows7OrGreater()) {
            version_stream << "7";
        }
        result->Success(flutter::EncodableValue(version_stream.str()));
    } else if (method_call.method_name() == "setIdleThreshold") {
        if (const auto* args = std::get_if<flutter::EncodableMap>(method_call.arguments())) {
            auto it = args->find(flutter::EncodableValue("threshold"));
            if (it != args->end()) {
                if (const auto* value = std::get_if<int>(&it->second)) {
                    inactivityThreshold = *value;
                    std::cout << "Updated idleThreshold to " << inactivityThreshold << " seconds" << std::endl;
                    result->Success(flutter::EncodableValue(inactivityThreshold));
                    return;
                }
            }
        }
        result->Error("INVALID_ARGUMENTS", "Invalid arguments for setIdleThreshold");
    } else {
        result->NotImplemented();
    }
}

std::string GetProcessName(DWORD processID) {
    std::wstring processName = L"<unknown>";

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32W);

        if (Process32FirstW(hProcessSnap, &pe32)) {
            do {
                if (pe32.th32ProcessID == processID) {
                    processName = pe32.szExeFile;
                    break;
                }
            } while (Process32NextW(hProcessSnap, &pe32));
        }
        CloseHandle(hProcessSnap);
    }

    return ConvertWStringToUTF8(processName);
}

std::string GetFocusedWindowAppName() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) return "<no window in focus>";
    
    DWORD processID;
    GetWindowThreadProcessId(hwnd, &processID);
    return GetProcessName(processID);
}

void NWindowFocusPlugin::CheckForInactivity() {
    std::thread([this]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastActivityTime).count();
            bool isActive = (duration <= inactivityThreshold * 1000); // Convert seconds to milliseconds

            std::cout << "TimeOut: " << inactivityThreshold << " seconds" << std::endl;
            std::cout << "User is active: " << isActive << std::endl;

            flutter::EncodableValue args(isActive);
            channel->InvokeMethod("onUserActiveChange", std::make_unique<flutter::EncodableValue>(args));
        }
    }).detach();
}

void NWindowFocusPlugin::StartFocusListener() {
    std::thread([this]() {
        HWND last_focused = nullptr;
        while (true) {
            HWND current_focused = GetForegroundWindow();
            if (current_focused != last_focused) {
                last_focused = current_focused;
                char title[256];
                GetWindowTextA(current_focused, title, sizeof(title));
                std::string appName = GetFocusedWindowAppName();
                std::string windowTitle = GetFocusedWindowTitle();

                std::cout << "Current window title: " << windowTitle << std::endl;
                std::cout << "Current window appName: " << appName << std::endl;

                flutter::EncodableMap data;
                data[flutter::EncodableValue("title")] = flutter::EncodableValue(ConvertWindows1251ToUTF8(title));
                data[flutter::EncodableValue("appName")] = flutter::EncodableValue(appName);
                data[flutter::EncodableValue("windowTitle")] = flutter::EncodableValue(ConvertWindows1251ToUTF8(windowTitle));

                channel->InvokeMethod("onFocusChange", std::make_unique<flutter::EncodableValue>(data));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }).detach();
}

} // namespace n_window_focus
