#pragma once

#include <atomic>

class FailsafeMonitor {
public:
    FailsafeMonitor(std::atomic<bool>& locked, std::atomic<bool>& override);
    void start();

private:
    void monitorLoop();
    bool isKeyPressed(int key);

    std::atomic<bool>& isLocked;
    std::atomic<bool>& overrideActive;
};
