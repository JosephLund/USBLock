#include "KeyMonitor.h"
#include <chrono>

KeyMonitor::KeyMonitor(UsbKeyManager& manager, std::atomic<bool>& locked, std::atomic<bool>& override)
    : usbManager(manager), isLocked(locked), overrideActive(override) {}

void KeyMonitor::start() {
    monitorThread = std::thread(&KeyMonitor::monitorLoop, this);
    monitorThread.detach();
}

void KeyMonitor::monitorLoop() {
    UsbDrive savedKey;

    while (true) {
        if (overrideActive.load()) {
            isLocked.store(false);
        } else {
            if (usbManager.loadKey(savedKey)) {
                auto drives = usbManager.getUsbDrives();
                bool keyPresent = usbManager.isKeyPresent(savedKey, drives);
                isLocked.store(!keyPresent);
            } else {
                isLocked.store(true);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
