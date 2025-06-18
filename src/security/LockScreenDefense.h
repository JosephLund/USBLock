#pragma once
#include <windows.h>
#include <atomic>
#include <thread>

class LockScreenDefense {
public:
    static LockScreenDefense& getInstance();

    void activate(HWND hwnd);
    void deactivate();

private:

    LockScreenDefense();
    ~LockScreenDefense();

    LockScreenDefense(const LockScreenDefense&) = delete;
    LockScreenDefense& operator=(const LockScreenDefense&) = delete;


    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK LockWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void startTaskManagerBlocker();
    void stopTaskManagerBlocker();

    HHOOK keyboardHook = nullptr;
    HWND hwnd = nullptr;
    std::atomic<bool> active = false;
    std::thread killerThread;
};
