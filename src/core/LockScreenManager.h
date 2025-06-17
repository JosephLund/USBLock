#pragma once
#include <windows.h>

class LockScreenManager {
public:
    void activateLock();
    void deactivateLock();

private:
    HWND lockWindow = nullptr;
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
