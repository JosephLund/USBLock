#include <iostream>
#include <atomic>
#include <thread>
#include "src/core/UsbKeyManager.h"
#include "src/core/KeyMonitor.h"
#include "src/core/FailsafeMonitor.h"
#include "src/core/LockScreenManager.h"
#include "src/ui/ImGuiInterface.h"
#include "src/core/KeyMonitor.h"

// ImGui includes (adjust path if needed)
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

int main()
{
    UsbKeyManager usbManager;

    std::atomic<bool> isLocked(false);
    std::atomic<bool> showPasswordPrompt(true);
    std::atomic<bool> overrideActive(false);
    UsbDrive dummyDrive;
    bool hasSavedKey = usbManager.loadKey(dummyDrive);
    if (hasSavedKey)
    {
        KeyMonitor keyMonitor(usbManager, isLocked, overrideActive);
        keyMonitor.start();
    }

    LockScreenManager lockManager(isLocked, showPasswordPrompt, overrideActive);

    FailsafeMonitor failsafe(isLocked, overrideActive);
    failsafe.start();

    ImGuiInterface imguiInterface(usbManager, isLocked, overrideActive);

    // ImGui init:
    if (!glfwInit())
        return -1;
    GLFWwindow *window = glfwCreateWindow(1280, 720, "USB Lock", NULL, NULL);
    glfwMakeContextCurrent(window);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    bool wasLocked = false;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (wasLocked && !isLocked)
        {
            ImGui::SetWindowFocus(nullptr); // remove focus from any open window
            ImGui::SetItemDefaultFocus();   // clear keyboard navigation
        }

        if (!hasSavedKey)
        {
            isLocked = false;
            wasLocked = false;
            showPasswordPrompt = false;
            imguiInterface.render(); // Let user assign a USB key
            hasSavedKey = usbManager.loadKey(dummyDrive);
        }
        else
        {
            // Refresh drive list
            std::vector<UsbDrive> drives = usbManager.getUsbDrives();
            bool keyPresent = usbManager.isKeyPresent(dummyDrive, drives);

            if (!keyPresent)
            {
                isLocked = true;
                showPasswordPrompt = true;
                lockManager.render();
            }
            else if (isLocked)
            {
                isLocked = false; // unlock automatically when key is detected
                imguiInterface.render();
            }
            else
            {
                imguiInterface.render();
            }
        }

        wasLocked = isLocked;

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
