#pragma once

#include <atomic>
#include <string>
#include "AdminConfig.h"

class LockScreenManager {
public:
    LockScreenManager(std::atomic<bool>& locked, std::atomic<bool>& showPasswordPrompt, std::atomic<bool>& overrideActive);

    void render();
    void activateLock();
    void deactivateLock();

private:
    std::atomic<bool>& isLocked;
    std::atomic<bool>& showPasswordPrompt;
    std::atomic<bool>& overrideActive;

    AdminConfig adminConfig;
    std::string passwordInput;
    std::string storedHash;
    std::string storedSalt;
    bool passwordLoaded = false;
};
