#pragma once

#define VK_ENABLE_BETA_EXTENSIONS
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace sve {

    class SveWindow {

        public:
            SveWindow(int w, int h, std::string name);
            ~SveWindow();

            SveWindow(const SveWindow &) = delete;
            SveWindow &operator=(const SveWindow &) = delete;

            bool shouldClose() {return glfwWindowShouldClose(window);}
            VkExtent2D getExtent() {return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};}
            bool wasWindowResized() {return framebufferResized; }
            void resetWindowResizedFlag() {framebufferResized = false; }

            void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

        private: 

            static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
            void initWindow();

            int width;
            int height;
            bool framebufferResized = false;

            std::string windowName;

            GLFWwindow *window;
    };

}
