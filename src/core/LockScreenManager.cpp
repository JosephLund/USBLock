#include "LockScreenManager.h"
#include "imgui.h"

LockScreenManager::LockScreenManager(std::atomic<bool>& locked, std::atomic<bool>& showPrompt, std::atomic<bool>& overrideActiveRef)
    : isLocked(locked), showPasswordPrompt(showPrompt), overrideActive(overrideActiveRef)
{
    passwordLoaded = adminConfig.loadPasswordHash(storedHash, storedSalt);
}

void LockScreenManager::activateLock() {
    isLocked = true;
}

void LockScreenManager::deactivateLock() {
    isLocked = false;
    overrideActive = true;
}

void LockScreenManager::render() {
    if (!isLocked) return;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("LOCKED", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::SetCursorPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 100, ImGui::GetIO().DisplaySize.y / 2 - 50));
    ImGui::Text("🔒 USB Key Missing 🔒");

    if (showPasswordPrompt) {
        ImGui::SetCursorPosY(ImGui::GetIO().DisplaySize.y / 2 + 10);
        char buf[64] = { 0 };
        strncpy_s(buf, passwordInput.c_str(), sizeof(buf) - 1);
        if (ImGui::InputText("Password", buf, sizeof(buf), ImGuiInputTextFlags_Password)) {
            passwordInput = buf;
        }

        if (ImGui::Button("Unlock")) {
            std::string inputHash = AdminConfig::hashPassword(passwordInput, storedSalt);

            if (!passwordLoaded) {
                // First-time setup — store password hash
                adminConfig.savePasswordHash(inputHash, storedSalt);
                storedHash = inputHash;
                passwordLoaded = true;
                deactivateLock();
            }
            else if (inputHash == storedHash) {
                deactivateLock();
            }
            passwordInput.clear();
        }
    }

    ImGui::End();
}
