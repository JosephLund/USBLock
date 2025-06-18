#pragma once
#include <string>
#include <vector>

struct UsbDrive {
    std::string driveLetter;    // E:
    std::string deviceID;       // \\.\PHYSICALDRIVE2
    std::string pnpDeviceID;
    std::string serialNumber;
    std::string volumeSerial;
};

class UsbKeyManager {
public:
    static UsbKeyManager& getInstance();

    std::vector<UsbDrive> getUsbDrives();
    void saveKey(const UsbDrive& key);
    bool loadKey(UsbDrive& key);
    void forgetKey();
    bool isKeyPresent(const UsbDrive& savedKey, const std::vector<UsbDrive>& currentDrives);

private:
    UsbKeyManager();
    ~UsbKeyManager();

    UsbKeyManager(const UsbKeyManager&) = delete;
    UsbKeyManager& operator=(const UsbKeyManager&) = delete;
};
