#pragma once

#include "sve_device.hpp"

namespace sve {
    class SveModel {
        public:
        SveModel();
        ~SveModel();

        SveModel(const SveModel &) = delete;
        SveModel &operator=(const SveModel &) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

        private:
        SveDevice& sveDevice;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;
    }
}