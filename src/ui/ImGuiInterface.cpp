#include "ImGuiInterface.h"
#include "../../vendor/imgui/imgui.h"

ImGuiInterface::ImGuiInterface(UsbKeyManager& manager, std::atomic<bool>& locked)
    : usbManager(manager), isLocked(locked)
{
    hasSavedKey = usbManager.loadKey(savedKey);
    drives = usbManager.getUsbDrives();
}

void ImGuiInterface::render() {
    ImGui::Begin("USB Key Manager");

    // Refresh devices automatically every frame
    drives = usbManager.getUsbDrives();

    ImGui::Text("Detected USB Drives:");
    if (drives.empty()) {
        ImGui::Text("No USB drives found.");
    } else {
        // Combo box for selecting drives
        std::vector<std::string> comboItems;
        for (const auto& drive : drives)
            comboItems.push_back(drive.deviceID + " | " + drive.driveLetter);

        std::vector<const char*> itemsCStr;
        for (const auto& item : comboItems)
            itemsCStr.push_back(item.c_str());

        ImGui::Combo("Select USB Drive", &selectedDriveIndex, itemsCStr.data(), (int)itemsCStr.size());
    }

    if (!hasSavedKey) {
        if (ImGui::Button("Assign Selected Drive")) {
            if (!drives.empty()) {
                usbManager.saveKey(drives[selectedDriveIndex]);
                savedKey = drives[selectedDriveIndex];
                hasSavedKey = true;
            }
        }
    } else {
        ImGui::Separator();
        ImGui::Text("Saved Key:");
        ImGui::Text("DeviceID: %s", savedKey.deviceID.c_str());
        ImGui::Text("PNP ID: %s", savedKey.pnpDeviceID.c_str());
        ImGui::Text("Volume Serial: %s", savedKey.volumeSerial.c_str());
        ImGui::Text("Serial: %s", savedKey.serialNumber.c_str());
        ImGui::Text("Key Status: %s", usbManager.isKeyPresent(savedKey, drives) ? "PRESENT" : "MISSING");

        if (ImGui::Button("Forget Key")) {
            usbManager.forgetKey();
            hasSavedKey = false;
        }
    }

    ImGui::End();

    // Lock Screen Overlay
    if (isLocked) {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("LOCKED", nullptr,
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        ImGui::SetCursorPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 150, ImGui::GetIO().DisplaySize.y / 2));
        ImGui::Text("🔒 USB Key Missing 🔒");
        ImGui::End();
    }
}
