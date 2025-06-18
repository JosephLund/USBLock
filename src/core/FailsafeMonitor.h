#pragma once

#include <atomic>

class FailsafeMonitor {
public:
    static FailsafeMonitor& getInstance();

    void initialize(std::atomic<bool>& locked, std::atomic<bool>& override);
    void start();

private:
    FailsafeMonitor() = default;
    ~FailsafeMonitor() = default;

    FailsafeMonitor(const FailsafeMonitor&) = delete;
    FailsafeMonitor& operator=(const FailsafeMonitor&) = delete;

    void monitorLoop();
    bool isKeyPressed(int key);

    std::atomic<bool>* isLocked = nullptr;
    std::atomic<bool>* overrideActive = nullptr;
};
