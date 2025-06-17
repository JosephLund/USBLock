#pragma once

#include "UsbKeyManager.h"
#include <atomic>

class KeyMonitor {
public:
    KeyMonitor(UsbKeyManager& manager, std::atomic<bool>& locked);
    void run();

private:
    UsbKeyManager& usbManager;
    std::atomic<bool>& isLocked;
    UsbDrive savedKey;
    bool hasSavedKey;
};
