#include <iostream>
#include <thread>
#include <atomic>

#include "src/core/UsbKeyManager.h"
#include "src/core/KeyMonitor.h"
#include "src/core/FailsafeMonitor.h"
#include "src/core/LockScreenManager.h"
#include "src/ui/ImGuiInterface.h"

#include "vendor/imgui/imgui.h"
#include "vendor/backends/imgui_impl_glfw.h"
#include "vendor/backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

void runFailsafe() {
    FailsafeMonitor failsafe;
    failsafe.run();
}

int main() {
    // Launch failsafe monitor (optional)
    std::thread failsafeThread(runFailsafe);

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(800, 600, "USB Key Manager", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Core components
    UsbKeyManager usbManager;
    std::atomic<bool> isLocked(false);
    KeyMonitor keyMonitor(usbManager, isLocked);
    ImGuiInterface ui(usbManager, isLocked);

    std::thread monitorThread([&]() { keyMonitor.run(); });

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ui.render();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    monitorThread.detach();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    failsafeThread.detach();

    return 0;
}
