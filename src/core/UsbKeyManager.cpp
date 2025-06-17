#include "UsbKeyManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <map>
#include "../utils/Utils.h"

UsbKeyManager::UsbKeyManager() {}

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
    std::ofstream file("config/usbkey.cfg");
    if (file.is_open()) {
        file << key.driveLetter << std::endl;
        file << key.deviceID << std::endl;
        file << key.pnpDeviceID << std::endl;
        file << key.serialNumber << std::endl;
        file.close();
    }
}

bool UsbKeyManager::loadKey(UsbDrive& key) {
    std::ifstream file("config/usbkey.cfg");
    if (file.is_open()) {
        std::getline(file, key.driveLetter);
        std::getline(file, key.deviceID);
        std::getline(file, key.pnpDeviceID);
        std::getline(file, key.serialNumber);
        file.close();
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
