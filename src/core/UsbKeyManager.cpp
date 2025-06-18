#include "UsbKeyManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <map>
#include "AdminConfig.h"
#include "../utils/Utils.h"


// 👇 ADD THESE TWO LINES
UsbKeyManager::UsbKeyManager() = default;
UsbKeyManager::~UsbKeyManager() = default;


UsbKeyManager& UsbKeyManager::getInstance() {
    static UsbKeyManager instance;
    return instance;
}

std::vector<UsbDrive> UsbKeyManager::getUsbDrives() {
    std::vector<UsbDrive> drives;

    // First query diskdrive
    FILE* pipeDisk = _popen("wmic diskdrive where \"InterfaceType='USB'\" get DeviceID,PNPDeviceID,SerialNumber,Index /format:csv", "r");
    if (!pipeDisk) {
        std::cerr << "Failed to query diskdrive." << std::endl;
        return drives;
    }

    std::map<std::string, UsbDrive> diskMap;  // index -> UsbDrive

    char buffer[512];
    while (fgets(buffer, sizeof(buffer), pipeDisk)) {
        std::string line(buffer);
        if (line.find("PNPDeviceID") != std::string::npos || line.empty())
            continue;

        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (std::getline(ss, token, ',')) {
            tokens.push_back(Utils::trim(token));
        }

        if (tokens.size() < 5)
            continue;

        std::string index = tokens[4];
        std::string deviceID = tokens[1];
        std::string pnpDeviceID = Utils::unescape(tokens[2]);
        std::string serialNumber = tokens[3];

        diskMap[index] = { "", deviceID, pnpDeviceID, serialNumber };
    }
    _pclose(pipeDisk);

    // Now query logicaldisk
    FILE* pipeLogical = _popen("wmic logicaldisk where \"drivetype=2\" get DeviceID,VolumeSerialNumber /format:csv", "r");
    if (!pipeLogical) {
        std::cerr << "Failed to query logicaldisk." << std::endl;
        return drives;
    }

    while (fgets(buffer, sizeof(buffer), pipeLogical)) {
    std::string line(buffer);
    if (line.find("DeviceID") != std::string::npos || line.empty())
        continue;

    std::stringstream ss(line);
    std::vector<std::string> tokens;
    std::string token;
    while (std::getline(ss, token, ',')) {
        tokens.push_back(Utils::trim(token));
    }

    if (tokens.size() < 3)
        continue;

    std::string logicalDrive = tokens[1];
    std::string volSerial = tokens[2];

    for (auto& [idx, usb] : diskMap) {
        if (usb.driveLetter.empty()) {
            usb.driveLetter = logicalDrive;
            usb.volumeSerial = volSerial;  // <-- add this line
            drives.push_back(usb);
            break;
        }
    }
}

    _pclose(pipeLogical);

    // Any unassigned drives (in case logicaldisk fails) still get pushed:
    for (auto& [idx, usb] : diskMap) {
        if (usb.driveLetter.empty())
            drives.push_back(usb);
    }

    return drives;
}

void UsbKeyManager::saveKey(const UsbDrive& key) {
    std::string combined = key.driveLetter + "\n" + key.deviceID + "\n" + key.pnpDeviceID + "\n" + key.serialNumber;
    std::string salt = Utils::generateRandomSalt(); // same as for passwords
    std::string encrypted = AdminConfig::encryptString(combined, salt);

    std::ofstream file("config/usbkey.cfg");
    if (file.is_open()) {
        file << salt << "\n" << encrypted;
        file.close();
    }
}

bool UsbKeyManager::loadKey(UsbDrive& key) {
    std::ifstream file("config/usbkey.cfg");
    if (file.is_open()) {
        std::string salt, encrypted;
        std::getline(file, salt);
        std::getline(file, encrypted); // read entire encrypted block
        file.close();

        std::string decrypted = AdminConfig::decryptString(encrypted, salt);

        std::stringstream ss(decrypted);
        std::getline(ss, key.driveLetter);
        std::getline(ss, key.deviceID);
        std::getline(ss, key.pnpDeviceID);
        std::getline(ss, key.serialNumber);
        return true;
    }
    return false;
}

void UsbKeyManager::forgetKey() {
    std::remove("config/usbkey.cfg");
}

bool UsbKeyManager::isKeyPresent(const UsbDrive& savedKey, const std::vector<UsbDrive>& currentDrives) {
    for (const auto& drive : currentDrives) {
        if (drive.pnpDeviceID == savedKey.pnpDeviceID && drive.serialNumber == savedKey.serialNumber)
            return true;
    }
    return false;
}
