#include "first_app.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <stdexcept>
#include <array>

namespace sve {

    struct SimplePushConstantData {
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    FirstApp::FirstApp() {
        loadModels();
        createPipelineLayout();
        recreateSwapChain();
        createCommandBuffers(); 
    }

    FirstApp::~FirstApp() {
        vkDestroyPipelineLayout(sveDevice.device(), pipelineLayout, nullptr);
    }

    void FirstApp::run() {
        while (!sveWindow.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(sveDevice.device());
    }

    void FirstApp::loadModels() {
  std::vector<SveModel::Vertex> vertices{
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}}, 
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}, 
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
  sveModel = std::make_unique<SveModel>(sveDevice, vertices);
}

    void FirstApp::createPipelineLayout() {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);
        
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if (vkCreatePipelineLayout(sveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout"); 
        }
    }

    void FirstApp::createPipeline() {

        assert(sveSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        SvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = sveSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        svePipeline = std::make_unique<SvePipeline>(
            sveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig); 
    }

    void FirstApp::recreateSwapChain() {
        auto extent = sveWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = sveWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(sveDevice.device());

        if (sveSwapChain == nullptr) {
            sveSwapChain = std::make_unique<SveSwapChain>(sveDevice, extent);
        } else {
            sveSwapChain = std::make_unique<SveSwapChain>(sveDevice, extent, std::move(sveSwapChain));
            if (sveSwapChain->imageCount() != commandBuffers.size()) {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        createPipeline();
    }

    void FirstApp::createCommandBuffers(){

        commandBuffers.resize(sveSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; 
        allocInfo.commandPool = sveDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(sveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

    }


    void FirstApp::freeCommandBuffers() {
        vkFreeCommandBuffers(
            sveDevice.device(), 
            sveDevice.getCommandPool(), 
            static_cast<uint32_t>(commandBuffers.size()) ,  
            commandBuffers.data());
        commandBuffers.clear();
    }

    void FirstApp::recordCommandBuffer(int imageIndex) {

            static int frame = 0;
            frame = (frame + 1) % 1000;

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = sveSwapChain->getRenderPass();
            renderPassInfo.framebuffer = sveSwapChain->getFrameBuffer(imageIndex);
            
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = sveSwapChain->getSwapChainExtent(); 

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0}; 
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data(); 

            vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(sveSwapChain->getSwapChainExtent().width);
            viewport.height = static_cast<float>(sveSwapChain->getSwapChainExtent().height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            VkRect2D scissor{{0, 0}, sveSwapChain->getSwapChainExtent()};
            vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
            vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

            svePipeline->bind(commandBuffers[imageIndex]);
            sveModel->bind(commandBuffers[imageIndex]);

            for (int j=0; j <4; j++) {
                SimplePushConstantData push{};
                push.offset = {-0.5f + frame * 0.002f, -0.4f + j * 0.25f};
                push.color = {0.0f, 0.0f, 0.2f + 0.2f * j};

                vkCmdPushConstants(
                    commandBuffers[imageIndex], 
                    pipelineLayout, 
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(SimplePushConstantData),
                    &push);

                sveModel->draw(commandBuffers[imageIndex]);

            }

            vkCmdEndRenderPass(commandBuffers[imageIndex]);
            if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            } 

    }


    void FirstApp::drawFrame(){

        uint32_t imageIndex;
        auto result = sveSwapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }
 
        recordCommandBuffer(imageIndex);
        result = sveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || sveWindow.wasWindowResized()) {
            sveWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS) {
                    throw std::runtime_error("failed to present swap chain image!");
                }
    }
}