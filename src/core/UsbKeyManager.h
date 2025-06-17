#pragma once
#include <string>
#include <vector>

struct UsbDrive {
    std::string driveLetter;    // E:
    std::string deviceID;       // \\.\PHYSICALDRIVE2
    std::string pnpDeviceID;
    std::string serialNumber;
    std::string volumeSerial;   // <-- newly added
};

class UsbKeyManager {
public:
    UsbKeyManager();

    std::vector<UsbDrive> getUsbDrives();
    void saveKey(const UsbDrive& key);
    bool loadKey(UsbDrive& key);
    void forgetKey();
    bool isKeyPresent(const UsbDrive& savedKey, const std::vector<UsbDrive>& currentDrives);
};
