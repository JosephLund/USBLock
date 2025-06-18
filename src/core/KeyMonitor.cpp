#include "KeyMonitor.h"
#include <chrono>
#include <stdexcept>

KeyMonitor& KeyMonitor::getInstance() {
    static KeyMonitor instance;
    return instance;
}

void KeyMonitor::initialize(UsbKeyManager& manager,
                            std::atomic<bool>& locked,
                            std::atomic<bool>& override)
{
    usbManager = &manager;
    isLocked = &locked;
    overrideActive = &override;
}

void KeyMonitor::start() {
    if (!usbManager || !isLocked || !overrideActive) {
        throw std::runtime_error("KeyMonitor not initialized.");
    }
    monitorThread = std::thread(&KeyMonitor::monitorLoop, this);
    monitorThread.detach();
}

void KeyMonitor::monitorLoop() {
    UsbDrive savedKey;

    while (true) {
        if (overrideActive->load()) {
            isLocked->store(false);
        } else {
            if (usbManager->loadKey(savedKey)) {
                auto drives = usbManager->getUsbDrives();
                bool keyPresent = usbManager->isKeyPresent(savedKey, drives);
                isLocked->store(!keyPresent);
            } else {
                isLocked->store(true);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
