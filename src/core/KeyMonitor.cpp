#include "KeyMonitor.h"
#include <thread>
#include <iostream>

KeyMonitor::KeyMonitor(UsbKeyManager& manager, std::atomic<bool>& locked)
    : usbManager(manager), isLocked(locked)
{
    hasSavedKey = usbManager.loadKey(savedKey);
}

void KeyMonitor::run() {
    if (!hasSavedKey) {
        std::cout << "No USB key configured\n";
        return;
    }

    while (true) {
        auto drives = usbManager.getUsbDrives();
        bool keyPresent = usbManager.isKeyPresent(savedKey, drives);
        isLocked = !keyPresent;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
