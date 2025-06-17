#pragma once

#include "../core/UsbKeyManager.h"
#include <atomic>

class ImGuiInterface {
public:
    ImGuiInterface(UsbKeyManager& manager, std::atomic<bool>& locked, std::atomic<bool>& overrideActiveRef);

    void render();

private:
    UsbKeyManager& usbManager;
    UsbDrive savedKey;
    bool hasSavedKey = false;
    std::vector<UsbDrive> drives;
    std::atomic<bool>& isLocked;
    std::atomic<bool>& overrideActive;
    int selectedDriveIndex = 0;
};
