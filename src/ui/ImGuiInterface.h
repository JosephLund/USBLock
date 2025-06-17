#pragma once

#include "../core/UsbKeyManager.h"
#include <atomic>

class ImGuiInterface {
public:
    ImGuiInterface(UsbKeyManager& manager, std::atomic<bool>& locked);

    void render();

private:
    UsbKeyManager& usbManager;
    std::atomic<bool>& isLocked;

    UsbDrive savedKey;
    std::vector<UsbDrive> drives;
    int selectedDriveIndex = 0;
    bool hasSavedKey;
};