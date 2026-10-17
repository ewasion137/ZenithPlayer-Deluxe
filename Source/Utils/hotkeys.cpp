#include "hotkeys.hpp"

#if defined(_WIN32)
#include <windows.h>

namespace {
HWND g_hotkeyHwnd = nullptr;
Utils::GlobalHotkeys* g_instance = nullptr;

LRESULT CALLBACK HotKeyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_HOTKEY && g_instance) {
        switch (wParam) {
        case 1: // Alt + P
        case 4: // Media Play/Pause
            emit g_instance->playPauseTriggered();
            return 0;
        case 2: // Alt + Right
        case 5: // Media Next
            emit g_instance->nextTriggered();
            return 0;
        case 3: // Alt + Left
        case 6: // Media Prev
            emit g_instance->prevTriggered();
            return 0;
        default:
            break;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
} // namespace
#endif

namespace Utils {

GlobalHotkeys::GlobalHotkeys(QObject* parent)
    : QObject(parent) {
    registerKeys();
}

GlobalHotkeys::~GlobalHotkeys() {
    unregisterKeys();
}

void GlobalHotkeys::registerKeys() {
#if defined(_WIN32)
    g_instance = this;

    WNDCLASSEXW wx = {};
    wx.cbSize = sizeof(WNDCLASSEXW);
    wx.lpfnWndProc = HotKeyWndProc;
    wx.hInstance = GetModuleHandleW(nullptr);
    wx.lpszClassName = L"ZenithGlobalHotkeysClass";

    RegisterClassExW(&wx);

    g_hotkeyHwnd = CreateWindowExW(
        0,
        L"ZenithGlobalHotkeysClass",
        L"ZenithGlobalHotkeys",
        0, 0, 0, 0, 0,
        HWND_MESSAGE,
        nullptr,
        GetModuleHandleW(nullptr),
        nullptr
    );

    if (g_hotkeyHwnd) {
        RegisterHotKey(g_hotkeyHwnd, 1, MOD_ALT | MOD_NOREPEAT, 'P');
        RegisterHotKey(g_hotkeyHwnd, 2, MOD_ALT | MOD_NOREPEAT, VK_RIGHT);
        RegisterHotKey(g_hotkeyHwnd, 3, MOD_ALT | MOD_NOREPEAT, VK_LEFT);
        RegisterHotKey(g_hotkeyHwnd, 4, MOD_NOREPEAT, VK_MEDIA_PLAY_PAUSE);
        RegisterHotKey(g_hotkeyHwnd, 5, MOD_NOREPEAT, VK_MEDIA_NEXT_TRACK);
        RegisterHotKey(g_hotkeyHwnd, 6, MOD_NOREPEAT, VK_MEDIA_PREV_TRACK);
    }
#endif
}

void GlobalHotkeys::unregisterKeys() {
#if defined(_WIN32)
    if (g_hotkeyHwnd) {
        for (int i = 1; i <= 6; ++i) {
            UnregisterHotKey(g_hotkeyHwnd, i);
        }
        DestroyWindow(g_hotkeyHwnd);
        g_hotkeyHwnd = nullptr;
    }
    UnregisterClassW(L"ZenithGlobalHotkeysClass", GetModuleHandleW(nullptr));
    g_instance = nullptr;
#endif
}

} // namespace Utils