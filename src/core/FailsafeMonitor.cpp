#include "FailsafeMonitor.h"
#include <iostream>
#include <windows.h>
#include <thread>

bool is_key_pressed(int key) {
    return GetAsyncKeyState(key) & 0x8000;
}

void FailsafeMonitor::run() {
    bool z_pressed = false;
    bool e_pressed = false;
    bool u_pressed = false;
    bool s_pressed = false;

    while (true) {
        if (is_key_pressed('Z')) z_pressed = true;
        if (z_pressed && is_key_pressed('E')) e_pressed = true;
        if (e_pressed && is_key_pressed('U')) u_pressed = true;
        if (u_pressed && is_key_pressed('S')) s_pressed = true;

        if (z_pressed && e_pressed && u_pressed && s_pressed) {
            std::cout << "\n[FAILSAFE TRIGGERED] Exiting...\n";
            exit(0);
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}
