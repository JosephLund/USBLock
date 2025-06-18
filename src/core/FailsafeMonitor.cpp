#include "FailsafeMonitor.h"
#include <Windows.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <atomic>
#include <stdexcept>

FailsafeMonitor &FailsafeMonitor::getInstance()
{
    static FailsafeMonitor instance;
    return instance;
}

void FailsafeMonitor::initialize(std::atomic<bool> &locked, std::atomic<bool> &override)
{
    isLocked = &locked;
    overrideActive = &override;
}

void FailsafeMonitor::start()
{
    if (!isLocked || !overrideActive)
    {
        throw std::runtime_error("FailsafeMonitor not initialized.");
    }
    std::thread(&FailsafeMonitor::monitorLoop, this).detach();
}

bool FailsafeMonitor::isKeyPressed(int key)
{
    return GetAsyncKeyState(key) & 0x8000;
}

void FailsafeMonitor::monitorLoop()
{
    while (true)
    {
        if (isKeyPressed('Z') && isKeyPressed('E') &&
            isKeyPressed('U') && isKeyPressed('S'))
        {

            std::cout << "Failsafe ZEUS override triggered.\n";
            isLocked->store(false);
            overrideActive->store(true);
            std::this_thread::sleep_for(std::chrono::seconds(1)); // prevent retriggers
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
