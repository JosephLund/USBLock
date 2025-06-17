#include "FailsafeMonitor.h"
#include <Windows.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <atomic>

FailsafeMonitor::FailsafeMonitor(std::atomic<bool>& locked, std::atomic<bool>& override)
    : isLocked(locked), overrideActive(override) {}

void FailsafeMonitor::start() {
    std::thread(&FailsafeMonitor::monitorLoop, this).detach();
}

bool FailsafeMonitor::isKeyPressed(int key) {
    return GetAsyncKeyState(key) & 0x8000;
}

void FailsafeMonitor::monitorLoop() {
    while (true) {
        if (this->isKeyPressed('Z') && this->isKeyPressed('E') && 
            this->isKeyPressed('U') && this->isKeyPressed('S')) {

            std::cout << "Failsafe ZEUS override triggered.\n";
            isLocked = false;
            overrideActive = true;
            std::this_thread::sleep_for(std::chrono::seconds(1));  // prevent retriggers
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
