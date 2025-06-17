#include "LockScreenManager.h"

void LockScreenManager::activateLock() {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = "USBLockClass";
    RegisterClass(&wc);

    lockWindow = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        "USBLockClass",
        "",
        WS_POPUP,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

    ShowWindow(lockWindow, SW_SHOW);
    SetForegroundWindow(lockWindow);
    SetWindowPos(lockWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void LockScreenManager::deactivateLock() {
    if (lockWindow) {
        DestroyWindow(lockWindow);
        lockWindow = nullptr;
    }
}

LRESULT CALLBACK LockScreenManager::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Disable close, ALT+F4, etc.
    if (uMsg == WM_CLOSE || uMsg == WM_DESTROY)
        return 0;

    // Prevent ESC key from closing
    if (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)
        return 0;

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
