#pragma once

#include "sve_window.hpp"
#include "sve_pipeline.hpp"
#include "sve_device.hpp"
#include "sve_swap_chain.hpp"
#include "sve_model.hpp"

// std
#include <memory>
#include <vector>

namespace sve {
    class FirstApp {
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 800;

            FirstApp();
            ~FirstApp();

            FirstApp(const FirstApp &) = delete;
            FirstApp &operator=(const FirstApp &) = delete;

            void run(); 


        private:
            void loadModels();
            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();
            void freeCommandBuffers();
            void drawFrame();
            void recreateSwapChain();
            void recordCommandBuffer(int imageIndex);

            SveWindow sveWindow{WIDTH, HEIGHT, "Hello World!"};
            SveDevice sveDevice{sveWindow};
            std::unique_ptr<SveSwapChain> sveSwapChain;
            std::unique_ptr<SvePipeline> svePipeline; 
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
            std::unique_ptr<SveModel> sveModel;
    };
}