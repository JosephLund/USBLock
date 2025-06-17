#pragma once

#include "UsbKeyManager.h"
#include <atomic>
#include <thread>

class KeyMonitor {
public:
    KeyMonitor(UsbKeyManager& manager, std::atomic<bool>& locked, std::atomic<bool>& overrideActive);
    void start();
private:
    void monitorLoop();

    UsbKeyManager& usbManager;
    std::atomic<bool>& isLocked;
    std::atomic<bool>& overrideActive;
    std::thread monitorThread;
};
