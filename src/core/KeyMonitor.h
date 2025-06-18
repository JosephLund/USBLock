#pragma once

#include "UsbKeyManager.h"
#include <atomic>
#include <thread>

class KeyMonitor {
public:
    static KeyMonitor& getInstance();

    void initialize(UsbKeyManager& manager,
                    std::atomic<bool>& locked,
                    std::atomic<bool>& overrideActive);

    void start();

private:
    KeyMonitor() = default;
    ~KeyMonitor() = default;

    KeyMonitor(const KeyMonitor&) = delete;
    KeyMonitor& operator=(const KeyMonitor&) = delete;

    void monitorLoop();

    UsbKeyManager* usbManager = nullptr;
    std::atomic<bool>* isLocked = nullptr;
    std::atomic<bool>* overrideActive = nullptr;
    std::thread monitorThread;
};
